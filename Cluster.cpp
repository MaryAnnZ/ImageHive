#include "Cluster.h"

Cluster::Cluster()
{}

Cluster::Cluster(std::vector<ImageAttribute> all, ImageAttribute middle, cv::Point2f position, int height, int width)
{
	allImages = all;
	middleImage = middle;
	pivot = position;

	cellHeight = height;
	cellWidth = width;
}


Cluster::~Cluster()
{


}
