// ImageHive.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <iostream>
#include <opencv2\core.hpp>
#include "opencv2\features2d\features2d.hpp"
#include "opencv2\nonfree\features2d.hpp"
#include <opencv\highgui.h>
#include <opencv2\imgproc\imgproc.hpp>
#include <Windows.h>
#include <shlobj.h>
#include <cstdio>
#include "ImageAttributes.h"
#include "DataLoader.h"
#include "Cluster.h"
#include "ResultImage.h"
#include "MyGraph.h"

using namespace cv;

//voronoi methods
std::vector<Cluster> createClusters(std::vector<ImageAttribute>, ResultImage);
void doVoronoi(std::vector<Cluster>, ResultImage);
void draw_point(Mat&, Point2f, Scalar);
void draw_delaunay(Mat&, Subdiv2D&, Scalar);
void draw_voronoi(Mat&, Subdiv2D&);

//col,row
std::map<int, std::vector<int>> gridLayout = { { 1,{ 1,1 } },{ 2,{ 2,1 } },{ 3,{ 2,2 } },
{ 4,{ 2,2 } },{ 5,{ 3,2 } },{ 6,{ 3,2 } },
{ 7,{ 3,3 } },{ 8,{ 3,3 } },{ 9,{ 3,3 } },
{ 10,{ 4,3 } },{ 11,{ 4,3 } },{ 12,{ 4,3 } } };


int main()
{
	DataLoader loader = DataLoader();
	std::vector<cv::Mat> images = loader.loadDataset();
	std::vector<cv::String> filePaths = loader.getFilePaths();

	if (images.empty()) {
		return 0;
	}

	int resultHeight = 400;
	int resultWidth = 600;

	ResultImage result = ResultImage(600, 400);

	//color and edge histogram
	std::vector<ImageAttribute> allImages(images.size());
	MyGraph graph = MyGraph();
	for (int i = 0; i < images.size(); i++) {
		ImageAttribute tmp = ImageAttribute::ImageAttribute(images.at(i),i);
		allImages[i] = tmp;
		allImages.at(i).calcColorHistogram();
		allImages.at(i).calcHOG();
		//graph.createVertex(allImages.at(i));
	}
		
	for (int i = 0; i < allImages.size(); i++) {
			ImageAttribute currentIA = allImages[i];
			
			std::vector<float> hogCorr;
			float maxHog = -1;
			std::vector<float> histCorr;
			for (int j = 0; j < allImages.size(); j++) {
				//small corr; big no corr -->normalize
				float corrHog = currentIA.compareHOGvalue(allImages.at(j).getHOGvalues());
				// 1 -> corr; 0 -> no corr;
				float corrHist = currentIA.compareHist(allImages.at(j).getColorHis());
				hogCorr.push_back(corrHog);
				if (corrHog > maxHog) {
					maxHog = corrHog;
				}
				histCorr.push_back(1 - corrHist);

			}
			std::cout << "comparing " << filePaths.at(i) << std::endl;
			for (int k = 0; k < histCorr.size(); k++) {
				//normalize HOG
				float hogValue = hogCorr.at(k) / maxHog;
				if (i != k) { //no edges with the same start and end vertex
					graph.createEdge(allImages.at(i), allImages.at(k), hogValue + histCorr.at(k)); //if weight==2 no corr, weight == 0 is the same
					std::cout << " with " << filePaths.at(k) << std::endl;
					std::cout << "value: " << hogValue + histCorr.at(k) << std::endl;
				}
			}
			std::cout << "*****************"  << std::endl;
	}

	std::vector<Cluster> allClusters = createClusters(allImages,result);

	doVoronoi(allClusters,result);
		
	cvWaitKey(0);
		

	return 0;
}

std::vector<Cluster> createClusters(std::vector<ImageAttribute> allMiddleImages, ResultImage result) {
	
	std::vector<Cluster> allClusters(allMiddleImages.size());

	int imageIndex = 0;
	bool even = (int)allMiddleImages.size() % 2 == 0;

	std::vector<int> colRowLayout = gridLayout[allMiddleImages.size()];
	int colCount = colRowLayout[0];
	int rowCount = colRowLayout[1];
	int lastColCount = colCount - ((colCount*rowCount) - allMiddleImages.size());

	int cellHeight = result.getHeight() / rowCount;

	//place all images on the result image (global placement)
	std::vector<std::vector<Mat>> canvasCluster(rowCount, std::vector<Mat>(colCount));
	cv::Mat tmpResult = cv::Mat(result.getHeight(), result.getWidth(), CV_8UC3);

	int cellWidth = result.getWidth() / colCount;
	int lastCellWidth = result.getWidth() / lastColCount;

	int lastHeightResult = 0;
	int lastWidthResult = 0;

	for (int row = 0; row < rowCount;row++) {
		for (int col = 0;col < colCount && imageIndex + 1 <= (allMiddleImages.size());col++) {
			Size oldSize = allMiddleImages[imageIndex].getOriginSize();
			cv::Mat resizedImage;
			int currentCellWidth = cellWidth;
			if (row == rowCount - 1) {
				//last row, split different!
				currentCellWidth = lastCellWidth;
			}

			if (oldSize.width >= oldSize.height) {
				float widthRatio = (float)currentCellWidth / (float)oldSize.width;
				resizedImage = allMiddleImages[imageIndex].resize(Size(currentCellWidth - 1, min(cellHeight, oldSize.height*widthRatio)));
			}
			else {
				float heightRatio = (float)cellHeight / (float)oldSize.height;
				resizedImage = allMiddleImages[imageIndex].resize(Size(min(currentCellWidth, oldSize.width*heightRatio), cellHeight - 1));
			}

			canvasCluster.at(row).at(col) = resizedImage;

			Size size = canvasCluster.at(row).at(col).size();

			canvasCluster.at(row).at(col).copyTo(tmpResult(Rect(lastWidthResult, lastHeightResult, size.width, size.height)));


			cv::Point2f pos = cv::Point2f(lastWidthResult + (currentCellWidth / 2), lastHeightResult + (cellHeight / 2));
			allClusters[imageIndex] = Cluster(allMiddleImages, allMiddleImages[imageIndex], pos, cellHeight, currentCellWidth);

			lastWidthResult += currentCellWidth;
			imageIndex++;
		}

		lastHeightResult += cellHeight;
		lastWidthResult = 1;
	}

	result.setResult(tmpResult);
	imshow("cluster_global", result.getResult());

	return allClusters;

}


void doVoronoi(std::vector<Cluster> allClusters, ResultImage result) {
	
	std::vector<Point2f> allPositions;

	for each (Cluster clus in allClusters) {
		allPositions.push_back(clus.pivot);
	}

	// Define window names
	string win_delaunay = "Delaunay Triangulation";
	string win_voronoi = "Voronoi Diagram";
	
	// Define colors for drawing.
	Scalar delaunay_color(255, 255, 255), points_color(0, 0, 255);
	
	// Keep a copy around
	Mat img_orig = result.getResult().clone();

	// Rectangle result be used with Subdiv2D
	Rect rect(0, 0, result.getWidth(), result.getHeight());

	// Create an instance of Subdiv2D
	Subdiv2D subdiv(rect);
	
	// Insert points into subdiv
	for (vector<Point2f>::iterator it = allPositions.begin(); it != allPositions.end(); it++)
	{
		subdiv.insert(*it);
		// Show animation
		Mat img_copy = img_orig.clone();
		// Draw delaunay triangles
		draw_delaunay(img_copy, subdiv, delaunay_color);
		imshow(win_delaunay, img_copy);
	}

	// Draw delaunay triangles
	draw_delaunay(result.getResult(), subdiv, delaunay_color);

	// Draw points
	for (vector<Point2f>::iterator it = allPositions.begin(); it != allPositions.end(); it++)
	{
		draw_point(result.getResult(), *it, points_color);
	}

	// Allocate space for Voronoi Diagram
	Mat img_voronoi = Mat::zeros(result.getResult().rows, result.getResult().cols, CV_8UC3);

	// Draw Voronoi diagram
	draw_voronoi(img_voronoi, subdiv);
	
	/*cv::SiftFeatureDetector detector;
	std::vector<cv::KeyPoint> keypoints;
	detector.detect(result.getResult(), keypoints);

	// Add results to image and save.
	cv::Mat output;
	cv::drawKeypoints(result.getResult(), keypoints, output);

	imshow(win_delaunay, output);*/
	imshow(win_voronoi, img_voronoi);
	waitKey(0);
}

// Draw a single point
void draw_point(Mat& img, Point2f fp, Scalar color)
{
	circle(img, fp, 2, color, CV_FILLED, CV_AA, 0);
}

// Draw delaunay triangles
void draw_delaunay(Mat& img, Subdiv2D& subdiv, Scalar delaunay_color)
{

	vector<Vec6f> triangleList;
	subdiv.getTriangleList(triangleList);
	vector<Point> pt(3);
	Size size = img.size();
	Rect rect(0, 0, size.width, size.height);

	for (size_t i = 0; i < triangleList.size(); i++)
	{
		Vec6f t = triangleList[i];
		pt[0] = Point(cvRound(t[0]), cvRound(t[1]));
		pt[1] = Point(cvRound(t[2]), cvRound(t[3]));
		pt[2] = Point(cvRound(t[4]), cvRound(t[5]));

		// Draw rectangles completely inside the image.
		if (rect.contains(pt[0]) && rect.contains(pt[1]) && rect.contains(pt[2]))
		{
			line(img, pt[0], pt[1], delaunay_color, 1, CV_AA, 0);
			line(img, pt[1], pt[2], delaunay_color, 1, CV_AA, 0);
			line(img, pt[2], pt[0], delaunay_color, 1, CV_AA, 0);
		}
	}
}

//Draw voronoi diagram
void draw_voronoi(Mat& img, Subdiv2D& subdiv)
{
	vector<vector<Point2f> > facets;
	vector<Point2f> centers;
	subdiv.getVoronoiFacetList(vector<int>(), facets, centers);

	vector<Point> ifacet;
	vector<vector<Point> > ifacets(1);

	for (size_t i = 0; i < facets.size(); i++)
	{
		ifacet.resize(facets[i].size());
		for (size_t j = 0; j < facets[i].size(); j++)
			ifacet[j] = facets[i][j];

		Scalar color;
		color[0] = rand() & 255;
		color[1] = rand() & 255;
		color[2] = rand() & 255;
		fillConvexPoly(img, ifacet, color, 8, 0);

		ifacets[0] = ifacet;
		polylines(img, ifacets, true, Scalar(), 1, CV_AA, 0);
		circle(img, centers[i], 3, Scalar(), CV_FILLED, CV_AA, 0);
	}
}
