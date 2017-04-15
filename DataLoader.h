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

	static std::vector<cv::Mat> loadDataset();

private:
	static std::string browseFolder();

};

