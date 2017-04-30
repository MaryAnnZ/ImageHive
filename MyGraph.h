#pragma once
#include "ImageAttributes.h"
#include "MyEdge.h"
#include <map>

class MyGraph
{
public:
	MyGraph();
	~MyGraph();

	void createEdge(ImageAttribute start, ImageAttribute end, float weight);
	void doClustering(int amountVertices);
	void checkNeighborhood();
	std::map<int, std::vector<MyEdge>> getClasses() { return classes; }
	void classesToString();
	
private:
	std::vector<MyEdge> edges;
	std::vector<ImageAttribute> visitedVertices;
	std::vector<MyEdge> chosenEdges;

	void checkConnection(std::vector<MyEdge> connectedEdges, std::vector<MyEdge> neighboringEdges);

	int amountClasses;
	std::map<int, std::vector<MyEdge>> classes;
	

};

