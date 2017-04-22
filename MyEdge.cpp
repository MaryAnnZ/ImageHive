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

bool MyEdge::compareTo(MyEdge otherEdge)
{
	if ((imageStart.compareImage(otherEdge.getStartImage()) && imageEnd.compareImage(otherEdge.getEndImage())) || (imageEnd.compareImage(otherEdge.getStartImage()) && imageStart.compareImage(otherEdge.getEndImage()))) {
		return true;
	}
	return false;
}

bool MyEdge::connectedWith(MyEdge otherEdge)
{
	if (imageStart.compareImage(otherEdge.getStartImage()) || imageStart.compareImage(otherEdge.getEndImage()) || imageEnd.compareImage(otherEdge.getStartImage()) || imageEnd.compareImage(otherEdge.getEndImage())) {
		return true;
	}
	return false;
}

void MyEdge::addNedigbor(MyEdge edge)
{
	neighbors.push_back(edge);
	std::cout << "Edge: " << weight << " is neighboring " << edge.getWeight() << std::endl;
}

