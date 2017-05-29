#pragma once
#include "Utils.h"

class IntersectionHelper
{
public:
	IntersectionHelper() {};
	~IntersectionHelper() {};


	bool isInside(std::vector<cv::Point> polygonVertices, cv::Point p);
	bool pointInPolygon(int polyCorners, std::vector<float> polyX, std::vector<float> polyY, float x, float y);

private:
	bool doIntersect(cv::Point p1, cv::Point q1, cv::Point p2, cv::Point q2);
	bool onSegment(cv::Point p, cv::Point q, cv::Point r);
	int orientation(cv::Point p, cv::Point q, cv::Point r);
};

