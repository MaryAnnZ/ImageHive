#pragma once
#include "Utils.h"
#include "jc_voronoi.h"
#include "DataLoader.h"
#include "Cluster.h"
#include "MyGraph.h"

using namespace cv;
using namespace std;


/**
* Calculates different parameters for all input images as sift comparison, histogram, edge detection and
* Objectness (Saliency Mapping), saves all calculated images in global dictionary "allimages"
@param rawImages references to all matrices which represents the raw images read from file
@param filePaths filepaths of all input files
*/
void doSimilarityCalculation(std::vector<cv::Mat> rawImages, std::vector<cv::String> filePaths);

/**
* Should be calles after parameter calculation of images. Creates a global graph and where edges have certain values
* After graph creation, connected components are evaluted for global clustering
*/
void doGlobalClustering();

/**
* After Graph generation and general clustering, this method creats cluster objects and saves them in a global cluster map
* it also does basic global placement on the result image
*/
void createClustersAndGlobalVoronoi();

/**
* Called during global clustering, creates a cluster with gien values
@param allImages all image which belong to this cluster
@param height height of cluster
@param width width of cluster
@param position lower left position on global result
@param pivot pivot point relative to result
*/
Cluster* createCluster(std::vector<ImageAttribute> allImages, int height, int width, cv::Point position, cv::Point pivot);

/**
* Is called after all clusters are set, then local voronoi starts including separating the space evenly with 
centroidal vornoi tessellation
*/
void doLocalVoronoi();


/**
* https://github.com/JCash/voronoi/tree/87c0ab219a531721a1dd7e57d5134444437d95c0
* The actual vornoi tessellation with help of github library
* manhattan distance is used for distance calculation
* is doing the global tessellation
@param all all pivot points of global cells
@param height height of cluster
@param width width of cluster
@param clusterMapping references to clusters
*/
void calculateGlobalVoronoiEdges(std::vector<cv::Point> all, int width, int height, std::vector<Cluster*> clusterMapping);

/**
* https://github.com/JCash/voronoi/tree/87c0ab219a531721a1dd7e57d5134444437d95c0
* The actual vornoi tessellation with help of github library
* manhattan distance is used for distance calculation
* is doing local tessellation wicth constrains of global tessellation
@param all all pivot points of local cell
@param offsetPosition relative to global result
@param clusterMapping references to all localclusters
@param clus references to belonging cluster
*/
void calculateLocalVoronoiEdges(Cluster* clus, std::vector<cv::Point> all, std::map<int, LocalCluster*> clusterMapping, cv::Point offsetPosition);

/**
* recalculates centers of vornoi cells, depending on size of diagram
@param diagram reference to voronoi diagram
@param points reference to all voronoi points
*/
void relax_points(const jcv_diagram* diagram, jcv_point* points);

/**
* clips the edges to image space
@param pt current point (edge vertex)
@param min min point of diagram
@param max max point of diagram
@param clus current cluster
*/
jcv_point recalcPoint(const jcv_point* pt, const jcv_point* min, const jcv_point* max, Cluster* clus);

/**
* clips the edges to image space
@param pt current point (edge vertex)
@param min min point of diagram
@param max max point of diagram
@param width widht of cluster
@param height height of cluster
*/
jcv_point recalcPoint(const jcv_point* pt, const jcv_point* min, const jcv_point* max, int width, int height);


/**
* Fills the matrix "out" according to boundries of parameter "boundries" with respective color of image "colorSrc"
* 
@param boundries boundry source where boundry is white pixel
@param out result image (should be filled after function)
@param colorSrc respective color of image
@param clus current cluster 
@param scaleRatio the scaling factor of image, which is scaled to fit into voronoi cell
*/
void cellFloodFill(Mat boundries, Mat out, Mat colorSrc, LocalCluster* clus, float scaleRatio);

/**
* Fills the matrix "out" according to boundries of parameter "boundries" with respective color of image "colorSrc"
*
@param boundries boundry source where boundry is white pixel
@param out result image (should be filled after function)
@param colorSrc respective color of image
@param clus current cluster
@param scaleRatio the scaling factor of image, which is scaled to fit into voronoi cell
@param offsetPosition position difference local and global voronoi cell
*/
void localFloodFill(Mat boundries, Mat out, Mat colorSrc, LocalCluster* clus, float scaleRatio, cv::Point offsetPosition);

/**
* Fills the matrix "out" according to boundries of parameter "boundries" with given colorIndex starting at give seedpoint
*
@param boundries boundry source where boundry is white pixel
@param out result image (should be filled after function)
@param colorIndex color index value
@param seed seed point of floodfill (global point)
*/
void floodFillColor(Mat boundries, Mat out,int colorIndex, cv::Point seed);

/**
* returns the clicked cluster according to given x and y position in result image
*/
Cluster* getTouchedCluster(int x, int y);

/**
* prints out a header for the program
*/
static void help();

/**
* called if user clicks on the resultimage
*/
static void onMouse(int event, int x, int y, int, void*);

/**
* free all global parameters
*/
void cleanUp();

