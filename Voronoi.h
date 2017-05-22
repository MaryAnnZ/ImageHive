#pragma once
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include <cmath> 
#include "ImageAttributes.h"

class Voronoi {
public:
	Voronoi() {};
	~Voronoi() {};

	void Voronoi::Make(cv::Mat* bmp, std::vector<cv::Point2f> points, bool test);
	void Make(cv::Mat* bmp, std::vector<cv::Point2f> points);

private:
	void CreateSites();
	void CreateSites(std::vector<cv::Point2f> points_);
	void SetSitesPoints();
	
	void CreateColors();
	int DistanceSqrd(cv::Point2f point, int x, int y);

	std::vector<cv::Point2f> points_;
	std::vector<cv::Vec3b> colors_;
	cv::Mat* bmp_;
};

