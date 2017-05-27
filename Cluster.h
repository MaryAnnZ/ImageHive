#include "Utils.h"
#include "LocalCluster.h"
#include "MyGraph.h"
#include <iostream>


class Cluster
{
public:

	Cluster(std::vector<ImageAttribute> all);
	~Cluster() {};
	Cluster() {};
		
	void addLocalCluster(ImageAttribute image, cv::Point pivot, int height, int width);
	
	void setGlobalPos(int width, int height);
	void createLocalGraph();

	std::vector<cv::Point> getAllLocalPivots() { return allLocalPoints; };
	void setResult(cv::Mat resultImage) { result = resultImage; };
	cv::Mat getResult() { return result; };

	cv::Point getPivot() { return pivot; };
	cv::Point setPivot(cv::Point piv) { pivot = piv; };

	std::vector<LocalCluster> getAllLocalClusters() {return localClusters;};

private:

	std::vector<cv::Point> allLocalPoints;
	std::vector<LocalCluster> localClusters;
	cv::Mat result;

	cv::Point pivot;

	MyGraph localGraph;
	std::vector<ImageAttribute> allImages;
	//std::map<cv::Point, ImageAttribute> imagePointMapping; 
};





