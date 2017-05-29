#pragma once
#include "Utils.h"

class DataLoader
{
public:
	/**
	*L�dt die Bilder in einem Ordner
	*/
	DataLoader();
	~DataLoader();
	/**
	*Gibt die Pfade zu dem geladenen Bilder zur�ck
	*/
	std::vector<cv::String> getFilePaths() { return filePaths; };
	/**
	*L�dt die Bilder
	*/
	std::vector<cv::Mat> DataLoader::loadDataset();

private:
	/**
	*�ffnet ein Windows File Browser
	*/
	std::string DataLoader::browseFolder();
	/**
	*die Pfade zu dem geladenen Bilder
	*/
	std::vector<cv::String> filePaths;
};

