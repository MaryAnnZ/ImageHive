#include "MyVertex.h"



MyVertex::MyVertex(ImageAttribute ia)
{
	image = ia;
}


MyVertex::~MyVertex()
{
}

void MyVertex::addEdge(MyVertex neighbor, float weight)
{
	neighbors.push_back(neighbor);
	neighborsWeight.push_back(weight);
}
