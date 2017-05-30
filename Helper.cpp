#include "Helper.h"

cv::Mat Helper::resize(cv::Mat image, cv::Size newsize) {

	cv::Mat newImage;
	cv::resize(image, newImage, newsize);
	return newImage;

}
