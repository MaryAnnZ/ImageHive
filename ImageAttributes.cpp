#include "stdafx.h"
#include "ImageAttributes.h"
#include <opencv2\imgproc\imgproc.hpp>
#include <iostream>
#include <opencv2\objdetect\objdetect.hpp>
#include <stdio.h>

ImageAttribute::ImageAttribute(cv::Mat img)
{
	image = img;
}

ImageAttribute::ImageAttribute()
{
}

ImageAttribute::~ImageAttribute()
{
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
	//I dont know if we need to resize the image or not...
	//cv::Mat resized;
	//cv::resize(image, resized, cv::Size(64, 48));
	cv::Mat grayScale;
	cv::cvtColor(image, grayScale, CV_BGR2GRAY);

	cv::HOGDescriptor des(cv::Size(32, 32), cv::Size(16, 16), cv::Size(8, 8), cv::Size(4, 4), 9);
	des.compute(grayScale, valuesHOG, cv::Size(0, 0), cv::Size(0, 0), locationsHOG);
	//writeHOG();
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

void ImageAttribute::writeHOG()
{
	//char fileName[100] = "HOG.xml";
	//cv::FileStorage hogXML(fileName, cv::FileStorage::WRITE);

	//int row = valuesHOG.size();
	//int col = valuesHOG.size();
	//std::cout << "col: " << col << ", row: " << row << std::endl;
	//cv::Mat mat(row, col, CV_32F);
	//for (int i = 0; i < row; i++) {
	//	memcpy(&(mat.data[col * i * sizeof(float)]), valuesHOG.at(i).data(), col * sizeof(float));
	//}
	//std::cout << "write" << std::endl;
	//cv::write(hogXML, "Descriptor", mat);
	//std::cout << "written" << std::endl;
	//hogXML.release();
	//std::cout << "END" << std::endl;
}
