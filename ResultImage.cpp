#include "ResultImage.h"

ResultImage::ResultImage(int widthPixel, int heightPixel)
{
	width = widthPixel;
	height = heightPixel;
	canvas = cv::Mat(height, width, CV_8UC3, double(0));
}

ResultImage::~ResultImage()
{
}
