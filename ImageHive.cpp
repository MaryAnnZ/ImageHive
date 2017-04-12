// ImageHive.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"

#include <iostream>
#include <opencv2\core.hpp>
#include <opencv2\highgui.hpp>
#include <Windows.h>
#include <shlobj.h>
#include <cstdio>


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
	std::vector<cv::Mat> images;
	for (int i = 0; i < filenames.size(); i++) {
		cv::String currentString = filenames.at(i);
		if (0 == currentString.compare(currentString.length() - ref1.length(), ref1.length(), ref1) || 0 == currentString.compare(currentString.length() - ref2.length(), ref1.length(), ref2)) {
			std::replace(currentString.begin(), currentString.end(), '\\', '/');
			std::cout << currentString << std::endl;
		
			cv::Mat img = cv::imread(currentString);

			images.push_back(img);
		}
	}

	//only test output
	if (!images.at(1).data) {
		std::cout << "Where is the image?" << std::endl;
		getchar();
	}
	else {
		cvNamedWindow("img");
		cv::imshow("img", images.at(1));
	}
	cvWaitKey(0);

    return 0;
}

