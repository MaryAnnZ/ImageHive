#include "MyEdge.h"
#include <iostream>


MyEdge::MyEdge(ImageAttribute iaStart, ImageAttribute isEnd, float edgeWeight)
{
	imageStart = iaStart;
	imageEnd = isEnd;
	weight = edgeWeight;
	
}


MyEdge::~MyEdge()
{
}

