#pragma once
#include "Utils.h"

class ImageAttribute
{
public:
	/**
	*Ein ImageAttribute stellt ein Bild dar und speichert seine relevante Attribute
	*
	*/
	::ImageAttribute(cv::Mat img, int idVal, std::string path);
	ImageAttribute();
	~ImageAttribute();

	void calcColorHistogram();
	void calcHOG();
	float compareHOGvalue(std::vector<float> otherValues);
	cv::Mat getImage() { return image; }
	std::vector<float> getHOGvalues() {return valuesHOG;}
	cv::Mat getResizedImage(){return resizedImage;};

	cv::Mat resize(cv::Size newSize);
	cv::Mat resize(cv::Mat, cv::Size newsize);

	cv::Size getOriginSize() { return image.size(); };

	std::vector<cv::Mat> getColorHis() { return bgrHist; }
	float compareHist(std::vector<cv::Mat> otherHistograms);

	int getId() { return id; }
	bool compareImage(ImageAttribute image);

	std::string getPath() { return filePath; }

	void calculateObjectness();

	void calculateKeyPoints();
	std::vector<cv::KeyPoint> getKeyPoints() { return keypoints; }
	cv::Mat getDescriptor() { return descriptor; }

	bool operator<(ImageAttribute other) const { return image.size < other.image.size; }

	cv::Mat getCropped() {return croppedImage;};
	cv::Mat getCroppedImage2() { return croppedImage2; }

	

private:
	cv::Mat image;
	int id;
	cv::Mat resizedImage;
	cv::Mat croppedImage;
	cv::Mat croppedImage2;

	std::vector<cv::Point> croppedImageRectangleVertices;

	std::vector<cv::Mat> bgrHist;
	std::vector<float> valuesHOG;
	std::vector<cv::Point> locationsHOG;

	void outputHistogram();
	std::string filePath;

	std::vector<cv::Vec4i> objectnessBoundingBox;
	std::vector<float> objectnessValue;

	int getVariance(std::vector<int> values);
	int getMean(std::vector<int> values);

	std::vector<cv::KeyPoint> keypoints;
	cv::Mat descriptor;

};

