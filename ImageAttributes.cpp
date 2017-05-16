#include "stdafx.h"
#include "ImageAttributes.h"
#include <opencv2\imgproc\imgproc.hpp>
#include <iostream>
#include <opencv2\objdetect\objdetect.hpp>
#include <stdio.h>
#include <opencv2\saliency.hpp>
#include <opencv2\xfeatures2d.hpp>

using namespace std;
using namespace cv;
using namespace saliency;
ImageAttribute::ImageAttribute(cv::Mat img, int idVal, std::string path)
{
	image = img;
	resizedImage = image.clone();
	id = idVal;
	filePath = path;
}

ImageAttribute::ImageAttribute()
{
}

ImageAttribute::~ImageAttribute()
{
}

cv::Mat ImageAttribute::resize(cv::Size newSize)
{
	cv::Mat tmp = image;
	cv::resize(image, resizedImage, newSize);
	return resizedImage;
}

void ImageAttribute::calcColorHistogram()
{
	if (bgrHist.empty()) {
		//by changing these values pls update outputHistogram() too
		std::vector<cv::Mat> bgrPlanes;
		if (image.empty()) {
			std::cout << "Image is empty " << std::endl;
		}
		cv::split(image, bgrPlanes);
		int histSize = 256;
		float range[] = { 0, 256 };
		const float* histRange = { range };
		bool uniform = true;
		bool accumulate = false;

		bgrHist.push_back(cv::Mat());
		bgrHist.push_back(cv::Mat());
		bgrHist.push_back(cv::Mat());

		cv::calcHist(&bgrPlanes[0], 1, 0, cv::Mat(), bgrHist.at(0), 1, &histSize, &histRange, uniform, accumulate);
		cv::calcHist(&bgrPlanes[1], 1, 0, cv::Mat(), bgrHist.at(1), 1, &histSize, &histRange, uniform, accumulate);
		cv::calcHist(&bgrPlanes[2], 1, 0, cv::Mat(), bgrHist.at(2), 1, &histSize, &histRange, uniform, accumulate);

		//normalize
		int normFactor = 512;
		cv::normalize(bgrHist.at(0), bgrHist.at(0), 0, normFactor, cv::NORM_MINMAX, -1, cv::Mat());
		cv::normalize(bgrHist.at(1), bgrHist.at(1), 0, normFactor, cv::NORM_MINMAX, -1, cv::Mat());
		cv::normalize(bgrHist.at(2), bgrHist.at(2), 0, normFactor, cv::NORM_MINMAX, -1, cv::Mat());

		//debug
		outputHistogram();
	}
}

void ImageAttribute::calcHOG()
{
	cv::Mat resized;
	cv::resize(image, resized, cv::Size(64, 128));//64, 48
	cv::Mat grayScale;
	cv::cvtColor(resized, grayScale, CV_BGR2GRAY);

	cv::HOGDescriptor des(cv::Size(64, 8), cv::Size(8, 8), cv::Size(4, 4), cv::Size(4, 4), 9);
	//cv::HOGDescriptor des();
	des.compute(grayScale, valuesHOG, cv::Size(0, 0), cv::Size(0, 0), locationsHOG);
}

float ImageAttribute::compareHOGvalue(std::vector<float> otherValues)
{
	//copy vector to mat  
	//create Mat  
	cv::Mat A(valuesHOG.size(), 1, CV_32FC1);
	//copy vector to mat  
	memcpy(A.data, valuesHOG.data(), valuesHOG.size() * sizeof(float));
	//create Mat  
	cv::Mat B(otherValues.size(), 1, CV_32FC1);
	//copy vector to mat  
	memcpy(B.data, otherValues.data(), otherValues.size() * sizeof(float));


	/////////////////////////
	//sum( sqrt( (A.-B)^2 ) )
	cv::Mat C = A - B;
	C = C.mul(C);
	cv::sqrt(C, C);
	cv::Scalar rr = cv::sum(C);
	float rrr = rr(0);
	return rrr;
}

float ImageAttribute::compareHist(std::vector<cv::Mat> otherHistograms)
{
	if (bgrHist.size() == otherHistograms.size()) {
		float correlation = 0.0;
		for (int i = 0; i < bgrHist.size(); i++) {
			correlation += cv::compareHist(bgrHist.at(i), otherHistograms.at(i), CV_COMP_CORREL);
		}
		return correlation / bgrHist.size();
	}
	else {
		return -1;
	}
}

bool ImageAttribute::compareImage(ImageAttribute image)
{
	return id == image.getId();
}

void ImageAttribute::calculateObjectness()
{
	if (objectnessBoundingBox.empty() && objectnessValue.empty()) {
		cv::Ptr<cv::saliency::Saliency> saliencyAlgorithmBing = cv::saliency::Saliency::create("BING");
		if (saliencyAlgorithmBing == NULL) {
			std::cout << "something went wrong :(" << std::endl;
			return;
		}

		saliencyAlgorithmBing.dynamicCast<cv::saliency::ObjectnessBING>()->setTrainingPath("C:/ObjectnessTrainedModel");
		saliencyAlgorithmBing.dynamicCast<cv::saliency::ObjectnessBING>()->setBBResDir("C:/ObjectnessTrainedModel/Results");

		if (saliencyAlgorithmBing->computeSaliency(image, objectnessBoundingBox)) {
			std::cout << "Objectness done" << std::endl;
			objectnessValue = saliencyAlgorithmBing.dynamicCast<cv::saliency::ObjectnessBING>()->getobjectnessValues();

			if (objectnessBoundingBox.size() > 0 && objectnessValue.size() > 0) {
				int lowerLeftX = 0;
				int lowerLeftXCounter = 0;
				int lowerLeftY = 0;
				int lowerLeftYcounter = 0;
				int upperRightX = 0;
				int upperRightXCounter = 0;
				int upperRightY = 0;
				int upperRightYCounter = 0;
				int end = 0;
				if (objectnessBoundingBox.size() > 500) {
					end = 500;
				}
				else {
					end = objectnessBoundingBox.size();
				}
				cv::Mat clone = image.clone();
				for (int i = 0; i < end; i++) {
					float highestObjectnessValue = 0;
					int index = -1;
					for (int k = 0; k < objectnessValue.size(); k++) {
						if (objectnessValue.at(k) > highestObjectnessValue) {
							highestObjectnessValue = objectnessValue.at(k);
							index = k;
						}
					}
					objectnessValue.at(index) = 0;
					cv::Vec4i bb = objectnessBoundingBox[index];
					if (bb[0] < image.cols / 2) {
						lowerLeftX += bb[0];
						lowerLeftXCounter++;
					}
					if (bb[1] < image.rows / 2) {
						lowerLeftY += bb[1];
						lowerLeftYcounter++;
					}
					if (bb[2] > image.cols / 2) {
						upperRightX += bb[2];
						upperRightXCounter++;
					}
					if (bb[3] > image.rows / 2) {
						upperRightY += bb[3];
						upperRightYCounter++;
					}
					cv::rectangle(clone, cv::Point(bb[0], bb[1]), cv::Point(bb[2], bb[3]), cv::Scalar(0, 0, 255), 4);
				}
				lowerLeftX = lowerLeftX / lowerLeftXCounter;
				lowerLeftY = lowerLeftY / lowerLeftYcounter;
				upperRightX = upperRightX / upperRightXCounter;
				upperRightY = upperRightY / upperRightYCounter;
				//std::cout << lowerLeftX << "; " << lowerLeftY << "; " << upperRightX << "; " << upperRightY  << " image: " << image.cols << "X" << image.rows << std::endl;
				croppedImage = image.clone();
				croppedImage = croppedImage(cv::Rect(lowerLeftX, lowerLeftY, upperRightX - lowerLeftX, upperRightY - lowerLeftY));
				cv::rectangle(image, cv::Point(lowerLeftX, lowerLeftY), cv::Point(upperRightX, upperRightY), cv::Scalar(0, 0, 255), 4);
				/// Display
				/*cv::namedWindow(filePath, CV_WINDOW_AUTOSIZE);
				cv::imshow(filePath, croppedImage);*/


			}
		}
	}
}

void ImageAttribute::outputHistogram()
{
	int hist_w = 512;
	int hist_h = 400;
	int histSize = 256;
	int bin_w = cvRound((double)hist_w / histSize);

	cv::Mat histImage(hist_h, hist_w, CV_8UC3, cv::Scalar(0, 0, 0));

	/// Draw for each channel
	for (int i = 1; i < histSize; i++)
	{
		line(histImage, cv::Point(bin_w*(i - 1), hist_h - cvRound(bgrHist[0].at<float>(i - 1))),
			cv::Point(bin_w*(i), hist_h - cvRound(bgrHist[0].at<float>(i))),
			cv::Scalar(255, 0, 0), 2, 8, 0);
		line(histImage, cv::Point(bin_w*(i - 1), hist_h - cvRound(bgrHist[1].at<float>(i - 1))),
			cv::Point(bin_w*(i), hist_h - cvRound(bgrHist[1].at<float>(i))),
			cv::Scalar(0, 255, 0), 2, 8, 0);
		line(histImage, cv::Point(bin_w*(i - 1), hist_h - cvRound(bgrHist[2].at<float>(i - 1))),
			cv::Point(bin_w*(i), hist_h - cvRound(bgrHist[2].at<float>(i))),
			cv::Scalar(0, 0, 255), 2, 8, 0);
	}

	/// Display
	cv::namedWindow("calcHist", CV_WINDOW_AUTOSIZE);
	cv::imshow("calcHist", histImage);
}

void ImageAttribute::calculateKeyPoints()
{
	if (!croppedImage.empty()) {
		cv::Mat greyScale = croppedImage.clone();
		
		cv::cvtColor(greyScale, greyScale, CV_BGR2GRAY);
		
		cv::Ptr<cv::xfeatures2d::SIFT> sift = cv::xfeatures2d::SIFT::create();
		sift->detect(greyScale, keypoints);
		cv::drawKeypoints(croppedImage, keypoints, croppedImage);
		cv::namedWindow(filePath, CV_WINDOW_AUTOSIZE);
		cv::imshow(filePath, croppedImage);
	}
}

