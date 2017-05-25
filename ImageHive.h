#pragma once
#include "Utils.h"
#include "jc_voronoi.h"
#include "DataLoader.h"
#include "Cluster.h"
#include "ResultImage.h"
#include "MyGraph.h"



using namespace cv;
using namespace std;

std::vector<Cluster> drawGlobalClusters(std::vector<Cluster>, ResultImage);
void doVoronoi(std::vector<Cluster>, ResultImage);
void draw_point(Mat&, Point2f, Scalar);
std::vector<Cluster> doLocalClusters(std::vector<ImageAttribute> allImages,
	std::map<int, std::vector<MyEdge>> globalClasses,
	std::map<int, std::vector<MyGraph::SiftImg>> localClasses,
	ResultImage result);
Cluster createLocalPlacement(std::vector<ImageAttribute> allImages, cv::Mat localResult);
//void doLocalVoronoi(std::vector<Cluster>);
jcv_point recalcPoint(const jcv_point* pt, const jcv_point* min, const jcv_point* max, int width, int height);
void calculatedAllVoronoiEdges(std::vector<cv::Point> all, int width, int height,std::vector<std::pair<cv::Point, cv::Point>>* edges);


Utils utils;
