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
	std::vector<cv::Mat> loadDataset();

private:
	std::string browseFolder();
	std::vector<cv::String> filePaths;
};

