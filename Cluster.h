#pragma once
#include <opencv2\core.hpp>
#include <opencv2\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include "ImageAttributes.h"
#include "LocalCluster.h"
#include "MyGraph.h"
#include <iostream>

class Cluster
{
public:
	Cluster(std::vector<ImageAttribute> all);
	~Cluster();
	Cluster();
		
	void addLocalCluster(ImageAttribute image, cv::Point2f pivot, int height, int width);
	void setResult(cv::Mat resultImage) { result = resultImage; };
	void createLocalGraph();

	cv::Mat getResult() { return result; };
	void setPivot(cv::Point2f newpivot) { pivot=newpivot; };
	cv::Point2f getPivot() { return pivot; };
	
private:
	std::vector<LocalCluster> localClusters;
	cv::Mat result;

	cv::Point2f pivot;

	MyGraph localGraph;
	std::vector<ImageAttribute> allImages;
};

