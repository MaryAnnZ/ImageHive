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

private:
	ImageAttribute imageStart;
	ImageAttribute imageEnd;
	float weight;
};

