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

}

void Cluster::createLocalGraph() {

	std::cout << "*****************" << std::endl;
	std::cout << "LOCAL" << std::endl;
	std::cout << "*****************" << std::endl;
	localGraph = MyGraph();

	localGraph.buildGraph(allImages);
	localGraph.doClustering(allImages.size());
	localGraph.classesToString();
	localGraph.compareSift();
	localGraph.IAclassesToString();
}

