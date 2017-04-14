#pragma once

inline int main()
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

			Mat img = cv::imread(currentString);

			images.push_back(img);
		}
	}

	doVoronoi(images);

	Mat cluster[images.size / 2 + 1][2];

	int imageIndex = 0;
	bool even = images.size % 2 = 0;

	for (int height = 0; height < images.size / 2;height++) {
		for (int length = 0; length < 2;length++) {
			if (even || (!even&&imageIndex<(images.size - 1))) {
				cluster[height][length] = images[imageIndex];
			}
			imageIndex++;
		}
	}

	int resultHeight;
	int resultWidth;



	Size sz1 = images[0].size();
	Size sz2 = images[1].size();
	Mat im3(sz1.height, sz1.width + sz2.width, CV_8UC3);
	images[0].copyTo(im3(Rect(0, 0, sz1.width, sz1.height)));
	images[1].copyTo(im3(Rect(sz1.width, 0, sz2.width, sz2.height)));
	imshow("im3", im3);

	cvWaitKey(0);

	return 0;
}

int main();
