#include "MyGraph.h"
#include <iostream>
#include <opencv2\features2d.hpp>
//#include <opencv2\opencv_modules.hpp>


void MyGraph::buildGraph(std::vector<ImageAttribute> all) {
	for (int i = 0; i < all.size(); i++) {
		ImageAttribute currentIA = all[i];

		std::vector<float> hogCorr;
		float maxHog = -1;
		std::vector<float> histCorr;
		int maxSiftMatch = -1;
		std::vector<int> siftCorr;
		for (int j = 0; j < all.size(); j++) {
				//small corr; big no corr -->normalize
				float corrHog = currentIA.compareHOGvalue(all.at(j).getHOGvalues());
				hogCorr.push_back(corrHog);
				if (corrHog > maxHog) {
					maxHog = corrHog;
				}
				// 1 -> corr; 0 -> no corr;
				float corrHist = currentIA.compareHist(all.at(j).getColorHis());
				histCorr.push_back(1 - corrHist);
				int corrSift = getSiftMatches(currentIA, all.at(j));
				siftCorr.push_back(corrSift);
				if (corrSift > maxSiftMatch) {
					maxSiftMatch = corrSift;
				}
			
		}
		//std::cout << "comparing " << all.at(i).getPath() << std::endl;
		for (int k = 0; k < histCorr.size(); k++) {
			//normalize HOG

			float hogValue = hogCorr.at(k) / maxHog;
			float siftValue = (float)siftCorr.at(k) / (float)maxSiftMatch;
			siftValue = 1.0 - siftValue;
			if (i != k) { //no edges with the same start and end vertex
				//createEdge(all.at(i), all.at(k), siftValue);
				createEdge(all.at(i), all.at(k), hogValue + histCorr.at(k) + siftValue); //if weight==3 no corr, weight == 0 is the same
				//std::cout << " with " << all.at(k).getPath() << std::endl;
				//std::cout << "value: " << hogValue + histCorr.at(k)  + siftValue<< std::endl;
			}
		}
		//std::cout << "*****************" << std::endl;
	}

}

void MyGraph::createEdge(ImageAttribute start, ImageAttribute end, float weight)
{
	edges.push_back(MyEdge(start, end, weight));
}

int MyGraph::doClustering(int amountVertices)
{
	while ((visitedVertices.empty() || visitedVertices.size() != amountVertices) && !edges.empty()) {
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
	writeIAclasses();

	return classes.size();
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

void MyGraph::IAclassesToString()
{
	std::cout << "()()()()()()()()()()()()()()()()()()()()()()()()()" << std::endl;
	typedef std::map<int, std::vector<SiftImg>>::iterator itType;
	for (itType it = classesImg.begin(); it != classesImg.end(); it++) {
		std::cout << "Class " << it->first << " +++++++++++++++++++++" << std::endl;
		for (int i = 0; i < it->second.size(); i++) {
			std::cout << it->second.at(i).img.getPath() << std::endl;
			std::cout << "Neighbor: " << it->second.at(i).neighborImg.getPath() << std::endl;
		}
	}
	std::cout << "()()()()()()()()()()()()()()()()()()()()()()()()()" << std::endl;
}

void MyGraph::compareSiftForNeighborhood()
{
	cv::FlannBasedMatcher flannMatcher;// = cv::BFMatcher();
	typedef std::map<int, std::vector<SiftImg>>::iterator itType;
	for (itType it = classesImg.begin(); it != classesImg.end(); it++) {
		for (int i = 0; i < it->second.size(); i++) {
			std::vector<cv::DMatch> matches;
			int bestMatches = -1;
			int bestMatchIndex = -1;
			for (int j = 0; j < it->second.size(); j++) {
				//std::cout << "size: " << it->second.size() << ", index: " << j << std::endl;
				if (i != j) {
					flannMatcher.match(it->second[i].img.getDescriptor(), it->second[i].img.getDescriptor(), matches);
					//-- Quick calculation of max and min distances between keypoints
					double maxDist = 0;
					double minDist = 100;
					for (int m = 0; m < it->second.at(i).img.getDescriptor().rows; m++) {
						double dist = matches.at(m).distance;
						if (dist < minDist) {
							minDist = dist;
						}
						if (dist > maxDist) {
							maxDist = dist;
						}
					}
					int goodMatchCounter = 0;
					for (int m = 0; m < it->second.at(i).img.getDescriptor().rows; m++) {
						if (matches[m].distance <= max(2*minDist, 0.02)) {
							goodMatchCounter++;
						}
					}
					//std::cout << "good match counter: " << goodMatchCounter << std::endl;
					if (goodMatchCounter > bestMatches) {
						bestMatches = goodMatchCounter;
						bestMatchIndex = j;
					}
					goodMatchCounter = -1;					
					matches.clear();
				}
			}
			//std::cout << bestMatchIndex << std::endl;
			it->second.at(i).neighborImg = it->second.at(bestMatchIndex).img;
			bestMatchIndex = -1;
		}
	}
	IAclassesToString();
}

int MyGraph::getSiftMatches(ImageAttribute image, ImageAttribute compareWith)
{
	cv::FlannBasedMatcher flannMatcher;// = cv::BFMatcher();
	std::vector<cv::DMatch> matches;
	flannMatcher.match(image.getDescriptor(), compareWith.getDescriptor(), matches);
	double maxDist = 0;
	double minDist = 100;
	for (int m = 0; m < matches.size(); m++) {
		double dist = matches.at(m).distance;
		if (dist < minDist) {
			minDist = dist;
		}
		if (dist > maxDist) {
			maxDist = dist;
		}
	}
	int goodMatchCounter = 0;
	float diffDist = std::abs(maxDist - minDist);
	for (int m = 0; m < matches.size(); m++) {
		if (matches[m].distance <= (minDist + (diffDist / 4))) {
			goodMatchCounter++;
		}
	}
	//std::cout << "minDist " << minDist << ", maxDist: " << maxDist << std::endl;
	//std::cout << "good Matc Counter: " << goodMatchCounter << ". maxMatches: " << matches.size() << std::endl;
	if (goodMatchCounter < matches.size() / 4) {
		goodMatchCounter = 0;
	}
	return goodMatchCounter;
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

void MyGraph::writeIAclasses()
{
	typedef std::map<int, std::vector<MyEdge>>::iterator itType;
	for (itType it = classes.begin(); it != classes.end(); it++) {
		std::vector<SiftImg> ia;
		for (int i = 0; i < it->second.size(); i++) {
			if (ia.empty()) {
				SiftImg siftImg;
				siftImg.img = it->second.at(i).getStartImage();
				SiftImg siftImg2;
				siftImg2.img = it->second.at(i).getEndImage();
				ia.push_back(siftImg);
				ia.push_back(siftImg2);
			}
			else {
				bool startFound = false;
				bool endFound = false;
				for (int k = 0; k < ia.size(); k++) {
					if (it->second.at(i).getStartImage().compareImage(ia.at(k).img)) {
						startFound = true;
					}
					if (it->second.at(i).getEndImage().compareImage(ia.at(k).img)) {
						endFound = true;
					}
					if (startFound && endFound) {
						break;
					}
				}
				if (!startFound) {
					SiftImg startImg;
					startImg.img = it->second.at(i).getStartImage();
					ia.push_back(startImg);
				}
				if (!endFound) {
					SiftImg endImg;
					endImg.img = it->second.at(i).getEndImage();
					ia.push_back(endImg);
				}
			}
		}
		classesImg[it->first] = std::vector<SiftImg>(ia);
		ia.clear();
	}

}
