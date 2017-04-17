#pragma once
#include "ImageAttributes.h"

class MyVertex
{
public:
	MyVertex(ImageAttribute ia);
	~MyVertex();

	void addEdge(MyVertex neighbor, float weight);
	ImageAttribute getImage() { return image; }

private:
	ImageAttribute image;
	std::vector<MyVertex> neighbors;
	std::vector<int> neighborsWeight;
};

