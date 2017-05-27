#include "Cluster.h"

Cluster::Cluster(std::vector<ImageAttribute> all)
{
	allImages = all;
}

void Cluster::setGlobalPos(int width, int height) {

	for (int i = 0;i < allLocalClusters.size();i++) {
		allLocalClusters[i]->globalPivot.x += width;
		allLocalClusters[i]->globalPivot.y += height;
	}

}

cv::Point Cluster::getGlobalClusterLoc() {

	int xPos = 0;
	int yPos = 0;

	for (int i = 0;i < allLocalClusters.size();i++) {
		xPos += allLocalClusters[i]->globalPivot.x;
		yPos += allLocalClusters[i]->globalPivot.y;
	}

	return cv::Point(xPos / allLocalClusters.size(), yPos / allLocalClusters.size());
}


void Cluster::addLocalCluster(ImageAttribute image, cv::Point locaPos, int height, int width) {
	allLocalClusters.push_back((new LocalCluster(image, locaPos, height, width)));
}



void Cluster::createLocalGraph() {

	std::cout << "*****************" << std::endl;
	std::cout << "LOCAL" << std::endl;
	std::cout << "*****************" << std::endl;
	localGraph = MyGraph();

	localGraph.buildGraph(allImages);
	localGraph.doClustering(allImages.size());
	localGraph.classesToString();
	localGraph.compareSiftForNeighborhood();
	localGraph.IAclassesToString();
}

