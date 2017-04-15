#include "stdafx.h"
#include "ImageAttributes.h"
#include <opencv2\imgproc\imgproc.hpp>
#include <iostream>
#include <opencv2\objdetect\objdetect.hpp>
#include <stdio.h>

ImageAttribute::ImageAttribute(cv::Mat img)
{
	image = img;
	resizedImage = image;
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
	image = tmp;
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

