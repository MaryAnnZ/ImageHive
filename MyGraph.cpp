#include "MyGraph.h"
#include <iostream>


MyGraph::MyGraph()
{
}


MyGraph::~MyGraph()
{
}

void MyGraph::createEdge(ImageAttribute start, ImageAttribute end, float weight)
{
	edges.push_back(MyEdge(start, end, weight));
	std::cout << "creating edge; start: " << start.getId() << " end: " << end.getId() << " weight: " << weight << std::endl;
}

void MyGraph::doClustering(int amountVertices)
{
	while (visitedVertices.empty() || visitedVertices.size() != amountVertices)
	{
		float minWeight = edges.at(0).getWeight();
		MyEdge tmp = edges.at(0);
		for (int i = 1; i < edges.size(); i++) {
			if (edges.at(i).getWeight() < minWeight) {
				tmp = edges.at(i);
				minWeight = tmp.getWeight();
			}
		}
	}
}
