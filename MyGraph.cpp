#include "MyGraph.h"



MyGraph::MyGraph()
{
}


MyGraph::~MyGraph()
{
}

void MyGraph::createVertex(ImageAttribute vertex)
{
	vertices.push_back(MyVertex(vertex));
}

void MyGraph::createEdge(ImageAttribute start, ImageAttribute end, float weight)
{
	int startIndex = -1;
	int endIndex = -1;
	for (int i = 0; i < vertices.size(); i++) {
		if (start.compareImage(vertices.at(i).getImage())) {
			startIndex = i;
		}
		else if (end.compareImage(vertices.at(i).getImage())) {
			endIndex = i;
		}
		if (startIndex != -1 && endIndex != -1) {
			break;
		}
	}
	vertices.at(startIndex).addEdge(vertices.at(endIndex), weight);
}
