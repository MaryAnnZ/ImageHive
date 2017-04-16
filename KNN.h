#pragma once
#include <opencv2\stitching\detail\util.hpp>
#include "ImageAttributes.h"
#include <map>
class KNN
{
public:
	KNN();
	~KNN();

	void createVertices(std::vector<ImageAttribute> vertices);
	void createEdge(int index1, int index2, float weight);

private:
	cv::detail::Graph imageGraph;
	std::map<int, ImageAttribute> verticesMap;
};

