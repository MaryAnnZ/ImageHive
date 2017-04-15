#pragma once
#include <opencv2\core.hpp>
#include <opencv2\highgui.hpp>

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

