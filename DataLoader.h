#pragma once
#include "Utils.h"

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

