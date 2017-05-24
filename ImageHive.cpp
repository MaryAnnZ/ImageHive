// ImageHive.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <iostream>
#include <opencv2\core.hpp>
#include "opencv2\features2d\features2d.hpp"
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
#include "Voronoi.h"

using namespace cv;
using namespace std;

//voronoi methods
void drawGlobalClusters(std::vector<Cluster>, ResultImage);
void doVoronoi(std::vector<Cluster>, ResultImage);
void draw_point(Mat&, Point2f, Scalar);
std::vector<Cluster> doLocalClusters(std::vector<ImageAttribute> allImages,
	std::map<int, std::vector<MyEdge>> globalClasses,
	std::map<int, std::vector<MyGraph::SiftImg>> localClasses,
	ResultImage result);
Cluster createLocalPlacement(std::vector<ImageAttribute> allImages, cv::Mat localResult);
void doLocalVoronoi(std::vector<Cluster>);


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

	int resultHeight = 600;
	int resultWidth = 900;

	ResultImage result = ResultImage(resultWidth, resultHeight);

	//color and edge histogram
	std::vector<ImageAttribute> allImages(images.size());
	
	for (int i = 0; i < images.size(); i++) {
		//allImages.push_back(ImageAttribute::ImageAttribute(images.at(i), i, filePaths.at(i)));	
		ImageAttribute tmp = ImageAttribute::ImageAttribute(images.at(i), i, filePaths.at(i));
		allImages[i] = tmp;
		allImages.at(i).calcColorHistogram();
		allImages.at(i).calcHOG();
		allImages.at(i).calculateObjectness();
		allImages.at(i).calculateKeyPoints();
	}

	MyGraph graph = MyGraph();
	graph.buildGraph(allImages);

	graph.doClustering(allImages.size());
	graph.classesToString();
	graph.compareSiftForNeighborhood();
	graph.IAclassesToString();

	std::vector<Cluster> allClusters = doLocalClusters(allImages,graph.getClasses(),graph.getLocalClasses(),result);

	drawGlobalClusters(allClusters, result);
	//doVoronoi(allClusters,result);
		
	cvWaitKey(0);
		

	return 0;
}

std::vector<Cluster> doLocalClusters(std::vector<ImageAttribute> allImages,
									std::map<int, std::vector<MyEdge>> globalClasses,
									std::map<int, std::vector<MyGraph::SiftImg>> localClasses, 
									ResultImage result) {

	std::map<int, int> isAlreadyClustered; //image ID to cluster ID mapping

	for (int i = 0;i < allImages.size();i++) {
		isAlreadyClustered.insert({ allImages[i].getId(),0});
	}

	std::map<int, std::vector<ImageAttribute>> globalCusterMapping;

	for (int i = 0;i < globalClasses.size();i++) {
		for (int num = 0;num < globalClasses[i].size();num++) {
			if (isAlreadyClustered[globalClasses[i][num].getStartImage().getId()]!=1) {
				isAlreadyClustered.at(globalClasses[i][num].getStartImage().getId()) = 1;
				globalCusterMapping[i].push_back(globalClasses[i][num].getStartImage());
				
			}

			if (isAlreadyClustered[globalClasses[i][num].getEndImage().getId()]!=1) {
				isAlreadyClustered.at(globalClasses[i][num].getEndImage().getId()) = 1;
				globalCusterMapping[i].push_back(globalClasses[i][num].getEndImage());

			}
		}
	}
	
	std::vector<Cluster> allLocalClusters;

	bool even = (int)globalCusterMapping.size() % 2 == 0;

	std::vector<int> colRowLayout = gridLayout[globalCusterMapping.size()];
	int colCount = colRowLayout[0];
	int rowCount = colRowLayout[1];
	int lastColCount = colCount - ((colCount*rowCount) - globalCusterMapping.size());

	int cellHeight = result.getHeight() / rowCount;
	
	int cellWidth = result.getWidth() / colCount;
	int lastCellWidth = result.getWidth() / lastColCount;


	for (int i = 0;i < globalCusterMapping.size();i++) {

		if (i > globalCusterMapping.size() - lastColCount) {
			allLocalClusters.push_back(createLocalPlacement(globalCusterMapping[i], cv::Mat(cellHeight, cellWidth, CV_8UC3, double(0))));
		}
		else {
			allLocalClusters.push_back(createLocalPlacement(globalCusterMapping[i], cv::Mat(cellHeight, lastCellWidth, CV_8UC3, double(0))));
		}

	}

	doLocalVoronoi(allLocalClusters);

	return allLocalClusters;
	
}

Cluster createLocalPlacement(std::vector<ImageAttribute> allImages, cv::Mat localResult) {
	
	Cluster cluster(allImages);
	cluster.createLocalGraph();

	int imageIndex = 0;
	bool even = (int)allImages.size() % 2 == 0;

	std::vector<int> colRowLayout = gridLayout[allImages.size()];
	int colCount = colRowLayout[0];
	int rowCount = colRowLayout[1];
	int lastColCount = colCount - ((colCount*rowCount) - allImages.size());

	int cellHeight = localResult.size().height / rowCount;

	//place all images on the result image (global placement)
	std::vector<std::vector<Mat>> canvasCluster(rowCount, std::vector<Mat>(colCount));
	cv::Mat tmpResult = cv::Mat(localResult.size().height, localResult.size().width, CV_8UC3);

	int cellWidth = localResult.size().width / colCount;
	int lastCellWidth = localResult.size().width / lastColCount;

	int lastHeightResult = 0;
	int lastWidthResult = 0;

	for (int row = 0; row < rowCount;row++) {
		for (int col = 0;col < colCount && imageIndex + 1 <= (allImages.size());col++) {
			Size oldSize = allImages[imageIndex].getCropped().size();
			cv::Mat resizedImage;
			int currentCellWidth = cellWidth;
			if (row == rowCount - 1) {
				//last row, split different!
				currentCellWidth = lastCellWidth;
			}

			if (oldSize.width >= oldSize.height) {
				float widthRatio = (float)currentCellWidth / (float)oldSize.width;
				resizedImage = allImages[imageIndex].resize(allImages[imageIndex].getCropped(),Size(currentCellWidth - 1, min(cellHeight, oldSize.height*widthRatio)));
			}
			else {
				float heightRatio = (float)cellHeight / (float)oldSize.height;
				resizedImage = allImages[imageIndex].resize(allImages[imageIndex].getCropped(),Size(min(currentCellWidth, oldSize.width*heightRatio), cellHeight - 1));
			}

			canvasCluster.at(row).at(col) = resizedImage;
			Size size = canvasCluster.at(row).at(col).size();

			canvasCluster.at(row).at(col).copyTo(tmpResult(Rect(lastWidthResult, lastHeightResult, size.width, size.height)));
			cv::Point2f pos = cv::Point2f(lastWidthResult + (currentCellWidth / 2), lastHeightResult + (cellHeight / 2));
			draw_point(tmpResult, pos, Scalar(0, 0, 255));

			cluster.addLocalCluster(allImages[imageIndex], pos, cellHeight, currentCellWidth);
				
			lastWidthResult += currentCellWidth;
			imageIndex++;
		}

		lastHeightResult += cellHeight;
		lastWidthResult = 1;
	}

	imshow("cluster_local"+ allImages[0].getId(), tmpResult);

	//set Resulting Image 
	cluster.setResult(tmpResult);

	return cluster;

}



 void drawGlobalClusters(std::vector<Cluster> allClusters, ResultImage globalResult) {
	
	int imageindex = 0;
	bool even = (int)allClusters.size() % 2 == 0;

	std::vector<int> colrowlayout = gridLayout[allClusters.size()];
	int colcount = colrowlayout[0];
	int rowcount = colrowlayout[1];
	int lastcolcount = colcount - ((colcount*rowcount) - allClusters.size());

	int cellheight = globalResult.getHeight() / rowcount;

	//place all images on the result image (global placement)
	std::vector<std::vector<cv::Mat>> canvascluster(rowcount, std::vector<cv::Mat>(colcount));
	cv::Mat tmpResult = cv::Mat(globalResult.getHeight(), globalResult.getWidth(), CV_8UC3);

	int cellwidth = globalResult.getWidth() / colcount;
	int lastcellwidth = globalResult.getWidth() / lastcolcount;

	int lastheightresult = 0;
	int lastwidthresult = 0;

	for (int row = 0; row < rowcount;row++) {
		for (int col = 0;col < colcount && imageindex + 1 <= (allClusters.size());col++) {
			Size oldsize = allClusters[imageindex].getResult().size();
			cv::Mat resizedimage;
			int currentcellwidth = cellwidth;
			if (row == rowcount - 1) {
				//last row, split different!
				currentcellwidth = lastcellwidth;
			}

			if (oldsize.width >= oldsize.height) {
				float widthratio = (float)currentcellwidth / (float)oldsize.width;
				cv::resize(allClusters[imageindex].getResult(), resizedimage, Size(currentcellwidth - 1, min(cellheight, oldsize.height*widthratio)));
			}
			else {
				float heightratio = (float)cellheight / (float)oldsize.height;
				cv::resize(allClusters[imageindex].getResult(), resizedimage, Size(min(currentcellwidth, oldsize.width*heightratio), cellheight - 1));
			}

			canvascluster.at(row).at(col) = resizedimage;

			Size size = canvascluster.at(row).at(col).size();

			canvascluster.at(row).at(col).copyTo(tmpResult(Rect(lastwidthresult, lastheightresult, size.width, size.height)));


			lastwidthresult += currentcellwidth;
			imageindex++;
		}

		lastheightresult += cellheight;
		lastwidthresult = 1;
	}

	globalResult.setResult(tmpResult);
	imshow("cluster_global", globalResult.getResult());


}

void doLocalVoronoi(std::vector<Cluster> allclusters) {
	for each (Cluster clus in allclusters) {

		cv::Mat image = clus.getResult();

		std::vector<cv::Point2f> allpositions = clus.getAllLocalPivots();
		//std::map<cv::Point2f, ImageAttribute> pointsImageMapping = clus.getImagePointMapping();
		

		// define window names
		string win_voronoi = "voronoi diagram";

		// allocate space for voronoi diagram
		Mat img_voronoi = Mat::zeros(image.rows, image.cols, CV_8UC3);

		Voronoi v;
		v.Make(&img_voronoi, allpositions);

		imshow(win_voronoi, img_voronoi);
		waitKey(0);

	}
 }

void doVoronoi(std::vector<Cluster> allclusters, ResultImage globalResult) {
	
	std::vector<cv::Point2f> allpositions;

	for each (Cluster clus in allclusters) {
		allpositions.push_back(clus.getPivot());
	}

	string win_voronoi = "voronoi diagram";
	// allocate space for voronoi diagram
	Mat img_voronoi = Mat::zeros(globalResult.getResult().rows, globalResult.getResult().cols, CV_8UC3);

	Voronoi v;
	v.Make(&img_voronoi, allpositions);
	
	imshow(win_voronoi, img_voronoi);
	waitKey(0);
}

// Draw a single point
void draw_point(Mat& img, cv::Point2f fp, Scalar color)
{
	circle(img, fp, 2, color, CV_FILLED, CV_AA, 0);
}


