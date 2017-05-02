#pragma once
#include <opencv2\core.hpp>
#include <opencv2\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include "ImageAttributes.h"

class Cluster
{
public:
	Cluster();
	Cluster(std::vector<ImageAttribute> allImages, ImageAttribute middleImage, cv::Point2f pivot, int height, int width);
	~Cluster();
	
	std::vector<ImageAttribute> allImages;
	ImageAttribute middleImage;

	cv::Point2f pivot;
	
private:
	int cellHeight;
	int cellWidth;

};

