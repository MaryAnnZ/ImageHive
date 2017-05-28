#include "Utils.h"
#include "LocalCluster.h"
#include "MyGraph.h"
#include <iostream>


class Cluster
{
public:

	Cluster(std::vector<ImageAttribute> all, int height, int width, cv::Point position, cv::Point pivot);
	~Cluster() {};
	Cluster() {};
		
	void addLocalCluster(ImageAttribute image, cv::Point globalPivot, int height, int width,cv::Point position);
	
	void createLocalGraph();
	
	std::vector<LocalCluster*> allLocalClusters;
	
	std::vector<cv::Point> polygonVertices;
	std::vector<std::pair<cv::Point, cv::Point>> polygonEdges;
	void calculatedBoundingBox();

	bool isInside(cv::Point p);

	cv::Point pivot;
	cv::Point position;

	int height;
	int width;

	int boundingHeight;
	int boundingWidth;

	std::vector<ImageAttribute> allImages;

	std::vector<cv::Point> boundingVertices;

private:
	bool doIntersect(cv::Point p1, cv::Point q1, cv::Point p2, cv::Point q2);
	bool onSegment(cv::Point p, cv::Point q, cv::Point r);
	int orientation(cv::Point p, cv::Point q, cv::Point r);

	MyGraph localGraph;

};





