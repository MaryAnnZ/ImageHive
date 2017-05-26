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

	MyGraph() {};
	~MyGraph() {};

	void buildGraph(std::vector<ImageAttribute> all);

	void createEdge(ImageAttribute start, ImageAttribute end, float weight);
	int doClustering(int amountVertices);
	void checkNeighborhood();
	std::map<int, std::vector<MyEdge>> getClasses() { return classes; }
	std::map<int, std::vector<SiftImg>> getLocalClasses() { return classesImg; }
	void classesToString();
	void IAclassesToString();

	void compareSiftForNeighborhood();

	
private:
	std::vector<MyEdge> edges;
	std::vector<ImageAttribute> visitedVertices;
	std::vector<MyEdge> chosenEdges;

	void checkConnection(std::vector<MyEdge> connectedEdges, std::vector<MyEdge> neighboringEdges);

	int amountClasses = 0;
	std::map<int, std::vector<MyEdge>> classes;
	std::map<int, std::vector<SiftImg>> classesImg;

	void writeIAclasses();
	int getSiftMatches(ImageAttribute image, ImageAttribute compareWith);

};

