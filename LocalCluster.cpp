#include "LocalCluster.h"

LocalCluster::~LocalCluster()
{


}

LocalCluster::LocalCluster(ImageAttribute img, cv::Point2f position, int height, int width)
{
	image = img;
	pivot = position;

	cellHeight = height;
	cellWidth = width;
}
