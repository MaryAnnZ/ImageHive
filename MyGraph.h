#pragma once
#include "ImageAttributes.h"
#include "MyEdge.h"
#include <map>

class MyGraph
{
public:

	struct SiftImg {
		ImageAttribute img;
		ImageAttribute neighborImg;
	};

	MyGraph();
	~MyGraph();

	void createEdge(ImageAttribute start, ImageAttribute end, float weight);
	void doClustering(int amountVertices);
	void checkNeighborhood();
	std::map<int, std::vector<MyEdge>> getClasses() { return classes; }
	void classesToString();
	void IAclassesToString();

	void compareSift();
	
private:
	std::vector<MyEdge> edges;
	std::vector<ImageAttribute> visitedVertices;
	std::vector<MyEdge> chosenEdges;

	void checkConnection(std::vector<MyEdge> connectedEdges, std::vector<MyEdge> neighboringEdges);

	int amountClasses;
	std::map<int, std::vector<MyEdge>> classes;
	std::map<int, std::vector<SiftImg>> classesImg;

	void writeIAclasses();

};

