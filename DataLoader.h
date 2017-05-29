#pragma once
#include "Utils.h"

class DataLoader
{
public:
	/**
	*Lädt die Bilder in einem Ordner
	*/
	DataLoader();
	~DataLoader();
	/**
	*Gibt die Pfade zu dem geladenen Bilder zurück
	*/
	std::vector<cv::String> getFilePaths() { return filePaths; };
	/**
	*Lädt die Bilder
	*/
	std::vector<cv::Mat> DataLoader::loadDataset();

private:
	/**
	*Öffnet ein Windows File Browser
	*/
	std::string DataLoader::browseFolder();
	/**
	*die Pfade zu dem geladenen Bilder
	*/
	std::vector<cv::String> filePaths;
};

