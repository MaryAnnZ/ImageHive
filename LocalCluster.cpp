#include "LocalCluster.h"

LocalCluster::LocalCluster(ImageAttribute img, cv::Point globalPivot_, int height, int width, cv::Point pos)
{
	image = img;
	globalPivot = globalPivot_;
	position = pos;
	cellHeight = height;
	cellWidth = width;
}

cv::Mat LocalCluster::getSaliencyCroppedImage() {

	int boundingHeight;
	int boundingWidth;

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

	boundingHeight = max.y - min.y;
	boundingWidth = max.x - min.x;

	if (min.y < 999) {
		cv::Size oldSize = image.getCropped().size();
		cv::Mat resizedImage;

		float ratio;
		if (boundingHeight >= boundingWidth) {
			ratio = (float)boundingHeight / (float)oldSize.height;
			
		}
		else {
			ratio = (float)boundingWidth / (float)oldSize.width;
		}

		resizedImage = image.resize(image.getCropped(), cv::Size(oldSize.width*(ratio), oldSize.height*(ratio)));

		/*if (oldSize.width >= oldSize.height) {
			float widthRatio = (float)boundingWidth / (float)oldSize.width;
			resizedImage = image.resize(image.getCropped(), cv::Size(boundingWidth, min(boundingHeight, oldSize.height*widthRatio)));
		}*/
		/*else {
			float heightRatio = (float)boundingHeight / (float)oldSize.height;
			resizedImage = image.resize(image.getCropped(), cv::Size(min(boundingWidth, oldSize.width*heightRatio), boundingHeight - 1));
		}
	*/

		return resizedImage;
	}
	else {
		image.getCropped();
	}
	
}
