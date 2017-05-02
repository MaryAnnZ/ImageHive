#pragma once
#include "ImageAttributes.h"

class MyEdge
{
public:
	MyEdge(ImageAttribute iaStart, ImageAttribute isEnd, float edgeWeight);
	~MyEdge();

	ImageAttribute getStartImage() { return imageStart; }
	ImageAttribute getEndImage() { return imageEnd; }
	float getWeight() { return weight; }
	bool compareTo(MyEdge otherEdge);
	bool connectedWith(MyEdge otherEdge);
	void addNedigbor(MyEdge edge);
	std::vector<MyEdge> getNeighbors() { return neighbors; }

private:
	ImageAttribute imageStart;
	ImageAttribute imageEnd;
	float weight;

	std::vector<MyEdge> neighbors;
};

