#include "Cluster.h"


Cluster::Cluster(std::vector<ImageAttribute> all, int height_, int width_, cv::Point position_, cv::Point pivot_)
{
	allImages = all;

	pivot = pivot_;
	position = position_;
	width = width_;
	height = height_;

}


void Cluster::addLocalCluster(ImageAttribute image, cv::Point globalPivot, int height, int width, cv::Point position) {
	allLocalClusters.push_back((new LocalCluster(image, globalPivot, height, width, position)));
}

void Cluster::calculatedBoundingBox() {

	boundingVertices.clear();

	cv::Point min = cv::Point(9999, 9999);
	cv::Point max = cv::Point(0, 0);

	for (int vert = 0; vert < polygonVertices.size();vert++) {
		if (polygonVertices[vert].x < min.x) {
			min.x = polygonVertices[vert].x;
		}
		if (polygonVertices[vert].y < min.y) {
			min.y = polygonVertices[vert].y;
		}
		if (polygonVertices[vert].y > max.y) {
			max.y = polygonVertices[vert].y;
		}
		if (polygonVertices[vert].x > max.x) {
			max.x = polygonVertices[vert].x;
		}
	}

	boundingVertices.push_back(min);
	boundingVertices.push_back(cv::Point(max.x, min.y));
	boundingVertices.push_back(max);
	boundingVertices.push_back(cv::Point(min.x, max.y));

	boundingHeight = max.y - min.y;
	boundingWidth = max.x - min.x;
}

void Cluster::createLocalGraph() {

	/*std::cout << "*****************" << std::endl;
	std::cout << "LOCAL" << std::endl;
	std::cout << "*****************" << std::endl;
	localGraph = MyGraph();

	localGraph.buildGraph(allImages);
	localGraph.doClustering(allImages.size());
	localGraph.classesToString();
	localGraph.compareSiftForNeighborhood();
	localGraph.IAclassesToString();*/
}

