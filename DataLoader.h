#pragma once
#include "stdafx.h"
#include <opencv2\core.hpp>
#include <opencv2\highgui.hpp>
#include <iostream>
#include <Windows.h>
#include <shlobj.h>
#include <cstdio>

class DataLoader
{
public:
	DataLoader();
	~DataLoader();

	std::vector<cv::String> getFilePaths() { return filePaths; };
	std::vector<cv::Mat> DataLoader::loadDataset();

private:
	std::string DataLoader::browseFolder();
	std::vector<cv::String> filePaths;
};

