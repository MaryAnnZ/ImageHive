#pragma once
#include "Utils.h"

using namespace cv;

class Helper
{
public:
	Helper() {};
	~Helper() {};

	/**
	* Skaliert das Bild zu uebergebenen Groesse
	@param image Bild welches skaliert werden soll
	@param newsize neue Groesse
	@return skaliertes Bild
	*/
	cv::Mat Helper::resize(cv::Mat image, cv::Size newsize);

private:

};

