#include "LocalCluster.h"

LocalCluster::LocalCluster(ImageAttribute img, cv::Point localPos, int height, int width)
{
	image = img;
	localPivot = localPos;

	cellHeight = height;
	cellWidth = width;
}

cv::Point LocalCluster::setGlobalPivot(int width, int height) {

	globalPivot = cv::Point(localPivot.x + width, localPivot.y + height);

	return cv::Point(globalPivot.x, globalPivot.y);
}