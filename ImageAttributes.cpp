#include "stdafx.h"
#include "ImageAttributes.h"
#include <opencv2\imgproc\imgproc.hpp>
#include <iostream>
#include <opencv2\objdetect\objdetect.hpp>
#include <stdio.h>
#include <opencv2\saliency.hpp>
#include <opencv2\xfeatures2d.hpp>
#include <numeric>

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

cv::Mat ImageAttribute::resize(cv::Mat image, cv::Size newsize) {

	cv::Mat newImage;
	cv::resize(image, newImage, newsize);
	return newImage;

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
	cv::Mat imageToProc;
	bool firstLoop = true;
	if (croppedImage.empty() && croppedImage2.empty()) {
		imageToProc = image.clone();
	}
	else if (!croppedImage.empty() && croppedImage2.empty()) {
		imageToProc = croppedImage.clone();
		firstLoop = false;
	}
	else {
		return;
	}
	if (objectnessBoundingBox.empty() && objectnessValue.empty()) {
		cv::Ptr<cv::saliency::Saliency> saliencyAlgorithmBing = cv::saliency::Saliency::create("BING");
		if (saliencyAlgorithmBing == NULL) {
			std::cout << "something went wrong :(" << std::endl;
			return;
		}

		saliencyAlgorithmBing.dynamicCast<cv::saliency::ObjectnessBING>()->setTrainingPath("C:/ObjectnessTrainedModel");
		saliencyAlgorithmBing.dynamicCast<cv::saliency::ObjectnessBING>()->setBBResDir("C:/ObjectnessTrainedModel/Results");

		if (saliencyAlgorithmBing->computeSaliency(imageToProc, objectnessBoundingBox)) {
			std::cout << "Objectness done" << std::endl;
			objectnessValue = saliencyAlgorithmBing.dynamicCast<cv::saliency::ObjectnessBING>()->getobjectnessValues();

			if (objectnessBoundingBox.size() > 0 && objectnessValue.size() > 0) {
				int end = objectnessBoundingBox.size() / 40;
				cv::Mat clone = imageToProc.clone();
				float avgX = 0;
				float avgY = 0;
				std::vector<cv::Vec4i> salientBoundingBoxes;
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
					salientBoundingBoxes.push_back(objectnessBoundingBox[index]);
					avgX += objectnessBoundingBox[index][0] + objectnessBoundingBox[index][2];
					avgY += objectnessBoundingBox[index][1] + objectnessBoundingBox[index][3];
				}
				avgX = avgX / (2 * end);
				avgY = avgY / (2 * end);
				std::vector<int> lowerLeftX;
				std::vector<int> lowerLeftY;
				std::vector<int> upperRightX;
				std::vector<int> upperRightY;
				for (int i = 0; i < salientBoundingBoxes.size(); i++) {
					cv::Vec4i bb = salientBoundingBoxes.at(i);
					if (bb[0] < avgX) {
						lowerLeftX.push_back(bb[0]);
					}
					if (bb[1] < avgY) {
						lowerLeftY.push_back(bb[1]);
					}
					if (bb[2] > avgX) {
						upperRightX.push_back(bb[2]);
					}
					if (bb[3] > avgY) {
						upperRightY.push_back(bb[3]);
					}
					cv::rectangle(clone, cv::Point(bb[0], bb[1]), cv::Point(bb[2], bb[3]), cv::Scalar(0, 0, 255), 4);
				}

				int finalLowerLeftX = getMean(lowerLeftX) - getVariance(lowerLeftX);
				if (finalLowerLeftX < 0) {
					finalLowerLeftX = 0;
				}
				int finalLowerLeftY = getMean(lowerLeftY) - getVariance(lowerLeftY);
				if (finalLowerLeftY < 0) {
					finalLowerLeftY = 0;
				}
				int finalUpperRightX = getMean(upperRightX) + getVariance(upperRightX);
				if (finalUpperRightX > imageToProc.cols) {
					finalUpperRightX = imageToProc.cols;
				}
				int finalUpperRightY = getMean(upperRightY) + getVariance(upperRightY);
				if (finalUpperRightY > imageToProc.rows) {
					finalUpperRightY = imageToProc.rows;
				}
				//std::cout << finalLowerLeftX << "; " << finalLowerLeftY << "; " << finalUpperRightX << "; " << finalUpperRightY  << " image: " << image.cols << "X" << image.rows << std::endl;
				if (firstLoop) {
					croppedImage = imageToProc.clone();
					croppedImage = croppedImage(cv::Rect(finalLowerLeftX, finalLowerLeftY, finalUpperRightX - finalLowerLeftX, finalUpperRightY - finalLowerLeftY));
					cropped1Coords.clear();
					cropped1Coords.push_back(finalLowerLeftX);
					cropped1Coords.push_back(finalLowerLeftY);
					cropped1Coords.push_back(finalUpperRightX);
					cropped1Coords.push_back(finalUpperRightY);
					//cv::rectangle(imageToProc, cv::Point(finalLowerLeftX, finalLowerLeftY), cv::Point(finalUpperRightX, finalUpperRightY), cv::Scalar(0, 0, 255), 4);
					//Display
					/*cv::namedWindow(filePath + "BING", CV_WINDOW_AUTOSIZE);
					cv::imshow(filePath + "BING", clone);
					cv::namedWindow(filePath, CV_WINDOW_AUTOSIZE);
					cv::imshow(filePath, croppedImage);*/
				}
				else {
					croppedImage2 = croppedImage.clone();
					croppedImage2 = croppedImage2(cv::Rect(finalLowerLeftX, finalLowerLeftY, finalUpperRightX - finalLowerLeftX, finalUpperRightY - finalLowerLeftY));
					cropped2Coords.clear();
					cropped2Coords.push_back(finalLowerLeftX + cropped1Coords.at(0));
					cropped2Coords.push_back(finalLowerLeftY + cropped1Coords.at(1));
					cropped2Coords.push_back(finalUpperRightX + cropped1Coords.at(0));
					cropped2Coords.push_back(finalUpperRightY + cropped1Coords.at(1));
					//cv::rectangle(croppedImage, cv::Point(finalLowerLeftX, finalLowerLeftY), cv::Point(finalUpperRightX, finalUpperRightY), cv::Scalar(0, 0, 255), 4);
					//Display
					//cv::namedWindow(filePath + "BING2", CV_WINDOW_AUTOSIZE);
					//cv::imshow(filePath + "BING2", clone);
					//cv::namedWindow(filePath + "2", CV_WINDOW_AUTOSIZE);
					//cv::imshow(filePath + "2", croppedImage2);
				}
				if (!cropped2Coords.empty()) {
					cv::Mat test = image.clone();
					cv::rectangle(test, cv::Point(cropped2Coords.at(0), cropped2Coords.at(1)), cv::Point(cropped2Coords.at(2), cropped2Coords.at(3)), cv::Scalar(0, 0, 255), 4);
					cv::rectangle(test, cv::Point(cropped1Coords.at(0), cropped1Coords.at(1)), cv::Point(cropped1Coords.at(2), cropped1Coords.at(3)), cv::Scalar(0, 0, 255), 4);
					
					cv::namedWindow(filePath + "test", CV_WINDOW_AUTOSIZE);
					cv::imshow(filePath + "test", test); 
				}
				objectnessBoundingBox.clear();
				objectnessValue.clear();
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
	//cv::namedWindow("calcHist", CV_WINDOW_AUTOSIZE);
	//cv::imshow("calcHist", histImage);
}

int ImageAttribute::getVariance(std::vector<int> values)
{
	double mean = getMean(values);
	double temp = 0;
	for (double a : values)
		temp += (a - mean)*(a - mean);
	int result = static_cast<int>(std::sqrt(temp / values.size()));
	//std::cout << result << std::endl;
	return result;
}

int ImageAttribute::getMean(std::vector<int> values)
{
	int result = static_cast<int>(std::accumulate(values.begin(), values.end(), 0.0) / values.size());
	//std::cout << result << std::endl;
	return result;
}

void ImageAttribute::calculateKeyPoints()
{
	if (!croppedImage.empty()) {
		cv::Mat greyScale = croppedImage.clone();
		cv::cvtColor(greyScale, greyScale, CV_BGR2GRAY);
		cv::Ptr<cv::xfeatures2d::SurfFeatureDetector> surf = cv::xfeatures2d::SurfFeatureDetector::create(400);
		surf->detect(greyScale, keypoints);
		cv::Ptr<cv::xfeatures2d::SurfDescriptorExtractor> extractor = cv::xfeatures2d::SurfFeatureDetector::create();
		extractor->compute(greyScale, keypoints, descriptor);
	}
}

