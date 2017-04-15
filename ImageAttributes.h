#pragma once
#include <opencv2\core.hpp>
#include <opencv2\highgui.hpp>

class ImageAttribute
{
public:
	ImageAttribute(cv::Mat img);
	ImageAttribute();
	~ImageAttribute();

	void calcColorHistogram();
	void calcHOG();
	cv::Mat getImage() { return image; }
private:
	cv::Mat image;
	std::vector<cv::Mat> bgrHist;
	std::vector<float> valuesHOG;
	std::vector<cv::Point> locationsHOG;

	void outputHistogram();
	void writeHOG();
};

