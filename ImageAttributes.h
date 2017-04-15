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
	cv::Mat getResizedImage(){return resizedImage;};

	cv::Mat resize(cv::Size newSize);
	cv::Size getOriginSize() { return image.size(); };

private:
	cv::Mat image;
	cv::Mat resizedImage;

	std::vector<cv::Mat> bgrHist;
	std::vector<float> valuesHOG;
	std::vector<cv::Point> locationsHOG;

	void outputHistogram();
	void writeHOG();

};

