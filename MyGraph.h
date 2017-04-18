#pragma once
#include "ImageAttributes.h"
#include "MyEdge.h"

class MyGraph
{
public:
	MyGraph();
	~MyGraph();

	void createEdge(ImageAttribute start, ImageAttribute end, float weight);
	void doClustering(int amountVertices);
private:
	std::vector<MyEdge> edges;
	std::vector<ImageAttribute> visitedVertices;


};

