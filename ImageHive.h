#pragma once
#include "Utils.h"
#include "jc_voronoi.h"
#include "DataLoader.h"
#include "Cluster.h"
#include "ResultImage.h"
#include "MyGraph.h"
#include "IntersectionHelper.h"

using namespace cv;
using namespace std;

//main methods
std::vector<Cluster*> createClustersAndGlobalVoronoi(std::vector<ImageAttribute> allImages,
	std::map<int, std::vector<MyEdge>> globalClasses,
	std::map<int, std::vector<MyGraph::SiftImg>> localClasses,
	ResultImage result);

Cluster* createCluster(std::vector<ImageAttribute> allImages, int height, int width, cv::Point position, cv::Point pivot);
std::vector<Cluster*> doLocalVoronoi(std::vector<Cluster*> allclusters, ResultImage globalResult);
void doGLobalVoronoiWithLocalClusters(std::vector<Cluster*>  allClusters, ResultImage result);

//Voronoi LIB
void calculateGlobalVoronoiEdges(std::vector<cv::Point> all, int width, int height, std::vector<Cluster*> clusterMapping);
void calculateLocalVoronoiEdges(Cluster* clus, std::vector<cv::Point> all, std::map<int, LocalCluster*> clusterMapping, cv::Point offsetPosition);

void calcGLobalWithLocalVoronoi(std::vector<cv::Point> all, std::vector<LocalCluster*> map, cv::Mat global);

void relax_points(const jcv_diagram* diagram, jcv_point* points);
jcv_point recalcPoint(const jcv_point* pt, const jcv_point* min, const jcv_point* max, Cluster* clus);
jcv_point recalcPoint(const jcv_point* pt, const jcv_point* min, const jcv_point* max, int width, int height);

//Filling
void floodFill(Mat src, Mat out, Mat colorSrc, std::vector<cv::Point> polygonVertices, int pointx, int pointy, int srcX, int srcY, int srcWidth, int srcHeight);

//Helper
void drawBoundingBox(cv::Mat cells, Cluster* clus, Scalar col);
void drawBoundingBox(cv::Mat cells, LocalCluster* Lclus, Scalar col);
void draw_point(Mat&, Point2f, Scalar);