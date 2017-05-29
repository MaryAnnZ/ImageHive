#include "LocalCluster.h"

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



cv::Mat LocalCluster::getSaliencyCroppedImage() {

	cv::Size innerSaliency = image.getCroppedImage2().size();
	cv::Size imageSize = image.getImage().size();

	cv::Mat rescaledImage;

	if (boundingHeight<=0 || boundingWidth <= 0 || boundingWidth>999 || boundingHeight>999) {
		boundingHeight = cellHeight;
		boundingWidth = cellWidth;
	}
	else {
		float ratio;
		if (boundingHeight >= boundingWidth) {
			ratio = (float)boundingHeight / (float)innerSaliency.height;

		}
		else {
			ratio = (float)boundingWidth / (float)innerSaliency.width;
		}

		rescaledImage = image.resize(image.getImage(), cv::Size(imageSize.width*(ratio), imageSize.height*(ratio)));
	}

	return rescaledImage;
}

