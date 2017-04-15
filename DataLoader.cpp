#include "DataLoader.h"

DataLoader::DataLoader()
{
}


DataLoader::~DataLoader()
{
}

std::string DataLoader::browseFolder()
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
}

std::vector<cv::Mat> DataLoader::loadDataset()
{
	std::vector<cv::String> filenames;
	std::vector<cv::String> tmpFilePaths;

	cv::String folder(DataLoader::browseFolder());
	cv::glob(folder, filenames);
	cv::String ref1 = "png";
	cv::String ref2 = "jpg";

	std::vector<cv::Mat> images;

	if (filenames.size() >= 4) {

		for (int i = 0; i < filenames.size(); i++) {
			cv::String currentString = filenames.at(i);

			if (0 == currentString.compare(currentString.length() - ref1.length(), ref1.length(), ref1) || 0 == currentString.compare(currentString.length() - ref2.length(), ref1.length(), ref2)) {
				std::replace(currentString.begin(), currentString.end(), '\\', '/');
				std::cout << currentString << std::endl;

				cv::Mat img = cv::imread(currentString);
				tmpFilePaths.push_back(currentString);

				images.push_back(img);
			}
		}

		
	}

	filePaths = tmpFilePaths;
	return(images);

}
