#pragma once
#include "Utils.h"
#include "ImageAttributes.h"

class LocalCluster
{
public:
	
	LocalCluster() {};
	~LocalCluster() {};
		LocalCluster(ImageAttribute img, cv::Point localPiv, int height, int width);
		
		cv::Point getLocalPivot() { return localPivot; };

		cv::Point getGlobalPivot() { return globalPivot; };
		cv::Point setGlobalPivot(int width, int height);

		void setGlobalPolygonVertices(std::vector<cv::Point> newPolygon) { globalPolygonVertices = newPolygon; };
		std::vector<cv::Point> getGlobalPolygonVertices() {return globalPolygonVertices; };

		void setGlobalPolygonEdges(std::vector<std::pair<cv::Point, cv::Point>> newPolygon) { globalPolygonEdges = newPolygon; };
		std::vector<std::pair<cv::Point, cv::Point>> getGlobalPolygonEdges() { return globalPolygonEdges; };


	private:
		int cellHeight;
		int cellWidth;

		std::vector<cv::Point> globalPolygonVertices;
		std::vector<std::pair<cv::Point, cv::Point>> globalPolygonEdges;

		cv::Point localPivot;
		cv::Point globalPivot;

		ImageAttribute image;

	};



