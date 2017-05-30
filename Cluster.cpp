#include "Cluster.h"


Cluster::Cluster(std::vector<ImageAttribute> all, int height_, int width_, cv::Point position_, cv::Point pivot_)
{
	allImages = all;
	pivot = pivot_;
	position = position_;
	width = width_;
	height = height_;

}


void Cluster::addLocalCluster(ImageAttribute image, cv::Point globalPivot, int height, int width, cv::Point position) {
	allLocalClusters.push_back((new LocalCluster(image, globalPivot, height, width, position)));
}
