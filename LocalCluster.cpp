#include "LocalCluster.h"

LocalCluster::LocalCluster(ImageAttribute img, cv::Point localPos, int height, int width)
{
	image = img;
	localPivot = localPos;

	globalPivot = localPos;

	cellHeight = height;
	cellWidth = width;
}
