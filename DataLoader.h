#pragma once
#include "Utils.h"

class DataLoader
{
public:
	/**
	*Laedt die Bilder in einem Ordner
	*/
	DataLoader();
	~DataLoader();
	/**
	*Gibt die Pfade zu dem geladenen Bilder zurueck
	@return Pfad zum geladenen Bild
	*/
	std::vector<cv::String> getFilePaths() { return filePaths; };
	/**
	*Laedt die Bilder
	@return Referenz zu allen geladenen Bildern
	*/
	std::vector<cv::Mat> DataLoader::loadDataset();

private:
	/**
	*oeffnet ein Windows File Browser
	*/
	std::string DataLoader::browseFolder();

	std::vector<cv::String> filePaths;					///<die Pfade zu dem geladenen Bilder
};

