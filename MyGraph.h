#pragma once
#include "ImageAttributes.h"
#include "MyVertex.h"

class MyGraph
{
public:
	MyGraph();
	~MyGraph();

	void createVertex(ImageAttribute vertex);
	void createEdge(ImageAttribute start, ImageAttribute end, float weight);

private:
	std::vector<MyVertex> vertices;


};

