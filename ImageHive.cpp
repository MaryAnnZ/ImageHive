// ImageHive.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"

#include <iostream>
#include <opencv2\core.hpp>
#include <opencv2\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <Windows.h>
#include <shlobj.h>
#include <cstdio>

using namespace cv;

//voronoi methods
void doVoronoi(std::vector<Mat>);
void draw_point(Mat&, Point2f, Scalar);
void draw_delaunay(Mat&, Subdiv2D&, Scalar);
void draw_voronoi(Mat&, Subdiv2D&);

std::string	BrowseFolder(void)
{
	TCHAR path[MAX_PATH];
	BROWSEINFO bi = { 0 };
	bi.lpszTitle = ("All Folders Automatically Recursed.");
	LPITEMIDLIST pidl = SHBrowseForFolder(&bi);

	if (pidl != 0)
	{
		// get the name of the folder and put it in path
		SHGetPathFromIDList(pidl, path);

		//Set the current directory to path
		SetCurrentDirectory(path);


		// free memory used
		IMalloc * imalloc = 0;
		if (SUCCEEDED(SHGetMalloc(&imalloc)))
		{
			imalloc->Free(pidl);
			imalloc->Release();
		}
	}
	return std::string(path);
}//BROWSE FOLDER


int main()
{
	std::vector<cv::String> filenames;
	cv::String folder(BrowseFolder());
	cv::glob(folder, filenames);
	cv::String ref1 = "png";
	cv::String ref2 = "jpg";

	if (filenames.size() >= 4) {

		std::vector<cv::Mat> images(filenames.size());
		int imageIndex = 0;

		for (int i = 0; i < filenames.size(); i++) {
			cv::String currentString = filenames.at(i);

			if (0 == currentString.compare(currentString.length() - ref1.length(), ref1.length(), ref1) || 0 == currentString.compare(currentString.length() - ref2.length(), ref1.length(), ref2)) {
				std::replace(currentString.begin(), currentString.end(), '\\', '/');
				std::cout << currentString << std::endl;

				Mat img = cv::imread(currentString);

				images[imageIndex] = img;
				imageIndex++;
			}
		}

		
		doVoronoi(images);
		
		cvWaitKey(0);
	}

    return 0;
}


void doVoronoi(std::vector<Mat> images) {
	
	int resultHeight = 400;
	int resultWidth = 600;

	std::vector<cv::Mat> resizedImages = images;
	std::vector<Point2f> positions(resizedImages.size());

	int imageIndex = 0;
	bool even = (int)images.size() % 2 == 0;

	int colCount = (int)images.size() / 2;
	int rowCount = even ? images.size() / colCount : (int)(images.size() / colCount) + 1;

	int cellHeight = resultHeight / rowCount;

	//place all images on the result image (global placement)
	std::vector<std::vector<Mat>> cluster(rowCount, std::vector<Mat>(colCount));

	int cellWidth = resultWidth / colCount;

	Mat result(resultHeight, resultWidth, CV_8UC3);
	int lastHeightResult = 1;
	int lastWidthResult = 1;

	for (int row = 0; row < rowCount;row++) {
		for (int col = 0;col < colCount && imageIndex + 1 <= (images.size());col++) {
			Size oldSize = images[imageIndex].size();

			if (oldSize.width > oldSize.height) {
				double widthRatio = (double)cellWidth / (double)oldSize.width;
				resize(images[imageIndex], resizedImages[imageIndex], 
					   Size(cellWidth-1, oldSize.height*widthRatio));
			}
			else {
				double heightRatio = (double)cellHeight / (double)oldSize.height;
				resize(images[imageIndex], resizedImages[imageIndex], 
					   Size(oldSize.width*heightRatio, cellHeight - 1));
			}
			
			cluster.at(row).at(col) = resizedImages[imageIndex];

			Size size = cluster.at(row).at(col).size();
			cluster.at(row).at(col).copyTo(result(Rect(lastWidthResult, lastHeightResult, size.width, size.height)));


			positions[imageIndex] = Point2f(lastWidthResult + (cellWidth / 2), lastHeightResult + (cellHeight / 2));

			lastWidthResult += cellWidth;
			imageIndex++;
		}

		lastHeightResult += cellHeight;
		lastWidthResult = 1;
	}

	// Define window names
	string win_delaunay = "Delaunay Triangulation";
	string win_voronoi = "Voronoi Diagram";
	
	// Define colors for drawing.
	Scalar delaunay_color(255, 255, 255), points_color(0, 0, 255);
	
	// Keep a copy around
	Mat img_orig = result.clone();

	// Rectangle result be used with Subdiv2D
	Size size = result.size();
	Rect rect(0, 0, size.width, size.height);

	// Create an instance of Subdiv2D
	Subdiv2D subdiv(rect);
	
	// Insert points into subdiv
	for (vector<Point2f>::iterator it = positions.begin(); it != positions.end(); it++)
	{
		subdiv.insert(*it);
		// Show animation
		Mat img_copy = img_orig.clone();
		// Draw delaunay triangles
		draw_delaunay(img_copy, subdiv, delaunay_color);
		imshow(win_delaunay, img_copy);
	}

	// Draw delaunay triangles
	draw_delaunay(result, subdiv, delaunay_color);

	// Draw points
	for (vector<Point2f>::iterator it = positions.begin(); it != positions.end(); it++)
	{
		draw_point(result, *it, points_color);
	}

	// Allocate space for Voronoi Diagram
	Mat img_voronoi = Mat::zeros(result.rows, result.cols, CV_8UC3);

	// Draw Voronoi diagram
	draw_voronoi(img_voronoi, subdiv);

	imshow(win_delaunay, result);
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
