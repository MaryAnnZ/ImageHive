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

	std::vector<cv::Point2f> getAllLocalPivots() {return allLocalPoints;};
	cv::Mat getResult() { return result; };
	cv::Point2f getPivot() { return pivot; };
	
	//std::map<cv::Point2f, ImageAttribute> getImagePointMapping() { return imagePointMapping; };

private:
	void updateClusterPivot();

	std::vector<cv::Point2f> allLocalPoints;
	std::vector<LocalCluster> localClusters;
	cv::Mat result;

	cv::Point2f pivot;

	MyGraph localGraph;
	std::vector<ImageAttribute> allImages;
	//std::map<cv::Point2f, ImageAttribute> imagePointMapping; 
};

