#include "Cluster.h"

Cluster::Cluster(std::vector<ImageAttribute> all)
{
	allImages = all;
}


Cluster::~Cluster()
{


}

Cluster::Cluster()
{


}

void Cluster::addLocalCluster(ImageAttribute image, cv::Point2f pivot, int height, int width) {

	LocalCluster currentLocal(image, pivot, height, width);

	localClusters.push_back(currentLocal);
	allLocalPoints.push_back(pivot);

	//imagePointMapping[pivot]= image;

	updateClusterPivot();
}

void Cluster::updateClusterPivot() {

	int x=0;
	int y=0; 

	for each(LocalCluster clus in localClusters) {
		x += clus.getPivot().x;
		y += clus.getPivot().y;
	}

	x /= localClusters.size();
	y /= localClusters.size();

	pivot = cv::Point2f(x, y);
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

