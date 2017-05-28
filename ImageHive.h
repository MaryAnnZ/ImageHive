#pragma once
#include "Utils.h"
#include "jc_voronoi.h"
#include "DataLoader.h"
#include "Cluster.h"
#include "ResultImage.h"
#include "MyGraph.h"

using namespace cv;
using namespace std;

std::vector<Cluster*> createClusters(std::vector<ImageAttribute> allImages,
	std::map<int, std::vector<MyEdge>> globalClasses,
	std::map<int, std::vector<MyGraph::SiftImg>> localClasses,
	ResultImage result);

std::vector<Cluster*> drawGlobalClusters(std::vector<Cluster*>, ResultImage);
void doVoronoi(std::vector<Cluster*>, ResultImage);
void draw_point(Mat&, Point2f, Scalar);

//std::vector<Cluster*> doLocalClusters(std::vector<ImageAttribute> allImages,
//	std::map<int, std::vector<MyEdge>> globalClasses,
//	std::map<int, std::vector<MyGraph::SiftImg>> localClasses,
//	ResultImage result);

Cluster* createCluster(std::vector<ImageAttribute> allImages, int height, int width, cv::Point position, cv::Point pivot);
void calcuateLocalVoronoi(std::vector<Cluster*> allclusters, ResultImage globalResult);
cv::Mat drawHelperLines(Mat img);

//Voronoi
void calculatedGlobalVoronoiEdges(std::vector<cv::Point> all, int width, int height, std::map<int, Cluster*> clusterMapping, std::vector<std::pair<cv::Point, cv::Point>> *edges);
void calculatedLocalVoronoiEdges(Cluster* clus,std::vector<cv::Point> all, std::map<int, LocalCluster*> clusterMapping, std::vector<std::pair<cv::Point, cv::Point>>* edges, cv::Point offsetPosition);
void relax_points(const jcv_diagram* diagram, jcv_point* points);
jcv_point recalcPoint(const jcv_point* pt, const jcv_point* min, const jcv_point* max, Cluster* clus);
jcv_point recalcPoint(const jcv_point* pt, const jcv_point* min, const jcv_point* max, int width, int height);

//Filling
void floodFill(Mat src, Mat out, Mat colorSrc, int pointx, int pointy, int srcX, int srcY, int srcWidth, int srcHeight);