#include "KNN.h"



KNN::KNN()
{
	imageGraph = cv::detail::Graph();
	//verticesMap = std::map<ImageAttribute, int>();
}


KNN::~KNN()
{
}

void KNN::createVertices(std::vector<ImageAttribute> vertices)
{
	for (int i = 0; i < vertices.size(); i++) {
		//shaderPrograms.insert(std::pair<const MeshLoadInfo::ShaderLoadInfo*, ShaderProgram*>(shader, result));
		verticesMap.insert(std::pair<int, ImageAttribute>(i, vertices.at(i)));
	}
	imageGraph.create(vertices.size());
}

void KNN::createEdge(int index1, int index2, float weight)
{
	imageGraph.addEdge(index1, index2, weight);
}
