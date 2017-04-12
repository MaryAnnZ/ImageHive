// ImageHive.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"

#include <iostream>
#include <opencv2\core.hpp>
#include <opencv2\highgui.hpp>
#include <Windows.h>
#include <cstdio>

int main()
{
	OPENFILENAME dialog;
	TCHAR pathBuffer[660];

	ZeroMemory(&dialog, sizeof(dialog));
	dialog.lStructSize = sizeof(dialog);
	dialog.hwndOwner = NULL;
	dialog.lpstrFile = pathBuffer;
	dialog.lpstrFile[0] = '\0';
	dialog.nMaxFile = sizeof(pathBuffer);
	dialog.lpstrFilter = "Image\0*.JPG;*.PNG\0";
	dialog.nFilterIndex = 1;
	dialog.lpstrFileTitle = NULL;
	dialog.nMaxFileTitle = 0;
	dialog.lpstrInitialDir = NULL;
	dialog.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	cv::Mat img;
	if (GetOpenFileName(&dialog) == TRUE) {
		if (dialog.lpstrFile == NULL) {
			return 0;
		}
		char filePath[660];


		int len = MultiByteToWideChar(CP_ACP, 0, dialog.lpstrFile, -1, NULL, 0);
		wchar_t* file = new wchar_t[len];
		MultiByteToWideChar(CP_ACP, 0, dialog.lpstrFile, -1, file, len);

		wcstombs(filePath, file, 660);

		img = cv::imread(filePath);
	}

	//cv::Mat img = cv::imread("myLittlePony.jpg");
	if (!img.data) {
		std::cout << "Where is the image?" << std::endl;
		getchar();
	}
	else {
		cvNamedWindow("img");
		cv::imshow("img", img);
	}
	cvWaitKey(0);

    return 0;
}

