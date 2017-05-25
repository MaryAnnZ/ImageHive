#pragma once
#include "Utils.h"
#include "ImageAttributes.h"

class LocalCluster
{
public:
	~LocalCluster() {};
		LocalCluster(ImageAttribute img, cv::Point localPiv, int height, int width);
		
		cv::Point getLocalPivot() { return localPivot; };

		cv::Point getGlobalPivot() { return globalPivot; };
		cv::Point setGlobalPivot(int width, int height);

	private:
		int cellHeight;
		int cellWidth;

		cv::Point localPivot;
		cv::Point globalPivot;

		ImageAttribute image;

	};



