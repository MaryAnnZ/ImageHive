#include "MyGraph.h"
#include <iostream>


MyGraph::MyGraph()
{
	amountClasses = 0;
}


MyGraph::~MyGraph()
{
}

void MyGraph::createEdge(ImageAttribute start, ImageAttribute end, float weight)
{
	edges.push_back(MyEdge(start, end, weight));
}

void MyGraph::doClustering(int amountVertices)
{
	while (visitedVertices.empty() || visitedVertices.size() != amountVertices) {
		float minWeight = edges.at(0).getWeight();
		int chosenEdgeIndex = 0;
		for (int i = 0; i < edges.size(); i++) {
			if (edges.at(i).getWeight() < minWeight) {
				minWeight = edges.at(i).getWeight();
				chosenEdgeIndex = i;
			}
		}
		bool containsStart = false;
		bool containsEnd = false;
		for (int i = 0; i < visitedVertices.size(); i++) {
			if (visitedVertices.at(i).compareImage(edges.at(chosenEdgeIndex).getStartImage())) {
				containsStart = true;
			}
			if (visitedVertices.at(i).compareImage(edges.at(chosenEdgeIndex).getEndImage())) {
				containsEnd = true;
			}
			if (containsEnd && containsStart) {
				break;
			}
		}
		if (!(containsStart && containsEnd)) {
			chosenEdges.push_back(edges.at(chosenEdgeIndex));
		}
		if (!containsStart) {
			visitedVertices.push_back(edges.at(chosenEdgeIndex).getStartImage());
		}
		if (!containsEnd) {
			visitedVertices.push_back(edges.at(chosenEdgeIndex).getEndImage());
		}
		edges.erase(edges.begin() + chosenEdgeIndex);

	}
	//for (int i = 0; i < chosenEdges.size(); i++) {
	//	std::cout << "chosen edges size: " << chosenEdges.at(i).getWeight() << std::endl;
	//}
	//std::cout << edges.size() << std::endl;
	//std::cout << visitedVertices.size() << std::endl;
	checkNeighborhood();
	std::vector<MyEdge> connectedEdges;
	std::vector<MyEdge> neighboringEdges;
	checkConnection(connectedEdges, neighboringEdges);
	classesToString();
}

void MyGraph::checkNeighborhood()
{
	for (int i = 0; i < chosenEdges.size(); i++) {
		for (int j = 0; j < chosenEdges.size(); j++) {
			if (i != j) {
				if (chosenEdges.at(i).connectedWith(chosenEdges.at(j))) {
					chosenEdges.at(i).addNedigbor(chosenEdges.at(j));
				}
			}
		}
	}
}

void MyGraph::classesToString()
{
	typedef std::map<int, std::vector<MyEdge>>::iterator itType;
	for (itType it = classes.begin(); it != classes.end(); it++) {
		std::cout << "Class " << it->first << " +++++++++++++++++++++" << std::endl;
		for (int i = 0; i < it->second.size(); i++) {
			std::cout << it->second.at(i).getStartImage().getPath() << std::endl;
			std::cout << it->second.at(i).getEndImage().getPath() << std::endl;
		}
	}
}

void MyGraph::checkConnection(std::vector<MyEdge> connectedEdges, std::vector<MyEdge> neighboringEdges)
{
	if (!connectedEdges.empty() && neighboringEdges.empty()) {
		
		classes[amountClasses] = std::vector<MyEdge>(connectedEdges);
		connectedEdges.clear();
		amountClasses++;
	}
	if (chosenEdges.empty()) {
		return;
	}
	if (connectedEdges.empty() && neighboringEdges.empty()) {
		connectedEdges.push_back(chosenEdges.at(0));
		neighboringEdges = std::vector<MyEdge>(chosenEdges.at(0).getNeighbors());
		chosenEdges.erase(chosenEdges.begin());
		checkConnection(connectedEdges, neighboringEdges);
	}
	else if (!neighboringEdges.empty()) {
		bool alreadyVisited = true;
		int edgeIndex = -1;
		for (int i = 0; i < chosenEdges.size(); i++) {
			if (chosenEdges.at(i).compareTo(neighboringEdges.at(0))) {
				alreadyVisited = false;
				edgeIndex = i;
				break;
			}
		}
		if (!alreadyVisited) {
			connectedEdges.push_back(neighboringEdges.at(0));
			neighboringEdges.insert(neighboringEdges.end(), neighboringEdges.at(0).getNeighbors().begin(), neighboringEdges.at(0).getNeighbors().end());
			chosenEdges.erase(chosenEdges.begin() + edgeIndex);
		}
		neighboringEdges.erase(neighboringEdges.begin());
		checkConnection(connectedEdges, neighboringEdges);
	}
}
