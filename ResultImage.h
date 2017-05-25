#pragma once
#include "Utils.h"

class ResultImage
{
public:
	ResultImage(int widthPixel, int heightPixel);
	~ResultImage();

	int getHeight() { return height; };
	int getWidth() { return width; };
	cv::Mat getResult() { return canvas; };
	void setResult(cv::Mat newRes) {canvas = newRes;};
	
private:
	int width;
	int height;
	cv::Mat canvas;

};

