#include "Cluster.h"

Cluster::Cluster(std::vector<ImageAttribute> all)
{
	allImages = all;
}

void Cluster::setGlobalPos(int width, int height) {
	for each (LocalCluster clus in localClusters) {
		cv::Point tmp = clus.setGlobalPivot(width, height);
		allLocalPoints.push_back(tmp);
	}
}


void Cluster::addLocalCluster(ImageAttribute image, cv::Point locaPos, int height, int width) {
	LocalCluster currentLocal(image, locaPos, height, width);
	localClusters.push_back(currentLocal);
	
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

