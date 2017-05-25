#pragma once
#include "stdafx.h"
#include <iostream>
#include <opencv2\core.hpp>
#include "opencv2\features2d\features2d.hpp"
#include <opencv2\features2d.hpp>
#include <opencv\highgui.h>
#include <opencv2\imgproc\imgproc.hpp>
#include "opencv2\videoio.hpp"
#include "opencv2\imgcodecs.hpp"
#include <Windows.h>
#include <shlobj.h>
#include <cstdio>
#include <iostream>
#include <map>

class Utils
{
public:

	//col,row
	std::map<int, std::vector<int>> gridLayout = { { 1,{ 1,1 } },{ 2,{ 2,1 } },{ 3,{ 2,2 } },
	{ 4,{ 2,2 } },{ 5,{ 3,2 } },{ 6,{ 3,2 } },
	{ 7,{ 3,3 } },{ 8,{ 3,3 } },{ 9,{ 3,3 } },
	{ 10,{ 4,3 } },{ 11,{ 4,3 } },{ 12,{ 4,3 } } };

	Utils() {};
	~Utils() {};

};


