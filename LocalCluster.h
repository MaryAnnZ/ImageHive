#pragma once
#include <opencv2\core.hpp>
#include <opencv2\highgui.hpp>
#include "ImageAttributes.h"

	class LocalCluster
	{
	public:
		~LocalCluster();
		LocalCluster(ImageAttribute img, cv::Point2f pivot, int height, int width);
		
		cv::Point2f getPivot() { return pivot; };


	private:
		int cellHeight;
		int cellWidth;
		cv::Point2f pivot;

		ImageAttribute image;

	};



