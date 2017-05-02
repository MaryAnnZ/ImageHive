#pragma once
#include <opencv2\core.hpp>
#include <opencv2\highgui.hpp>

class ImageAttribute
{
public:
	ImageAttribute::ImageAttribute(cv::Mat img, int idVal, std::string path);
	ImageAttribute();
	~ImageAttribute();

	void calcColorHistogram();
	void calcHOG();
	float compareHOGvalue(std::vector<float> otherValues);
	cv::Mat getImage() { return image; }
	std::vector<float> getHOGvalues() {return valuesHOG;}
	cv::Mat getResizedImage(){return resizedImage;};

	cv::Mat resize(cv::Size newSize);
	cv::Size getOriginSize() { return image.size(); };

	std::vector<cv::Mat> getColorHis() { return bgrHist; }
	float compareHist(std::vector<cv::Mat> otherHistograms);

	int getId() { return id; }
	bool compareImage(ImageAttribute image);

	std::string getPath() { return filePath; }

private:
	cv::Mat image;
	int id;
	cv::Mat resizedImage;

	std::vector<cv::Mat> bgrHist;
	std::vector<float> valuesHOG;
	std::vector<cv::Point> locationsHOG;

	void outputHistogram();
	std::string filePath;

};

