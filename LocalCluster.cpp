#include "LocalCluster.h"
#include "Helper.h"

LocalCluster::LocalCluster(ImageAttribute img, cv::Point globalPivot_, int height, int width, cv::Point pos)
{
	image = img;
	globalPivot = globalPivot_;
	position = pos;
	cellHeight = height;
	cellWidth = width;
}

void LocalCluster::calculateBoundingBox() {

	boundingVertices.clear();

	cv::Point min = cv::Point(9999, 9999);
	cv::Point max = cv::Point(0, 0);

	for (int vert = 0; vert < globalPolygonVertices.size();vert++) {
		if (globalPolygonVertices[vert].x < min.x) {
			min.x = globalPolygonVertices[vert].x;
		}
		if (globalPolygonVertices[vert].y < min.y) {
			min.y = globalPolygonVertices[vert].y;
		}
		if (globalPolygonVertices[vert].y > max.y) {
			max.y = globalPolygonVertices[vert].y;
		}
		if (globalPolygonVertices[vert].x > max.x) {
			max.x = globalPolygonVertices[vert].x;
		}
	}

	boundingVertices.push_back(min);
	boundingVertices.push_back(cv::Point(max.x, min.y));
	boundingVertices.push_back(max);
	boundingVertices.push_back(cv::Point(min.x, max.y));

	boundingHeight = max.y - min.y;
	boundingWidth = max.x - min.x;
}



cv::Mat LocalCluster::getSaliencyCroppedImage(int mode) {

	Helper helper = Helper();

	//if mode == 0 then less saliency constraint, if mode == 1 more saliency constraint!

	int outerSaliencyHeight = image.getCropped().rows;
	int outerSaliencyWidth = image.getCropped().cols;


	cv::Size imageSize = image.getImage().size();

	cv::Mat rescaledImage;

	if (boundingHeight<=0 || boundingWidth <= 0 || boundingWidth>999 || boundingHeight>999) {
		boundingHeight = cellHeight;
		boundingWidth = cellWidth;
	}
	
	float ratio;
	if (boundingHeight >= boundingWidth) {
		if (mode == 1) {
			ratio = (float)boundingWidth / (float)outerSaliencyWidth;
		}
		else {
			ratio = (float)boundingHeight / (float)outerSaliencyHeight;
		}

	}
	else {
		if (mode == 1) {
			ratio = (float)boundingHeight / (float)outerSaliencyHeight;
		}
		else {
			ratio = (float)boundingWidth / (float)outerSaliencyWidth;
		}
	}

	if (mode == 0) {
		rescaledImage = helper.resize(image.getImage(), cv::Size(imageSize.width*(ratio), imageSize.height*(ratio)));
	}
	else {
		rescaledImage = helper.resize(image.getSaliencyMarkedOriginal(), cv::Size(imageSize.width*(ratio), imageSize.height*(ratio)));
	}
	

	lastScalingRatio = ratio;

	return rescaledImage;
}

