#include "LocalCluster.h"

LocalCluster::LocalCluster(ImageAttribute img, cv::Point globalPivot_, int height, int width, cv::Point pos)
{
	image = img;
	globalPivot = globalPivot_;
	position = pos;
	cellHeight = height;
	cellWidth = width;
}
