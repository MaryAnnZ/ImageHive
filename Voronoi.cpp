#include "Voronoi.h"

void Voronoi::Make(cv::Mat* bmp, std::vector<cv::Point2f> points,bool test) {
	bmp_ = bmp;
	//CreateColors();
	
	points_ = points;

	SetSitesPoints();
	CreateSites(points);
}

void Voronoi::Make(cv::Mat* bmp, std::vector<cv::Point2f> points) {
	bmp_ = bmp;
	points_ = points;
	CreateColors();
	CreateSites();
	SetSitesPoints();
}

void Voronoi::CreateSites(std::vector<cv::Point2f> points_) {
	for (const auto& point : points_) {
		int x = point.x, y = point.y;
		for (int i = -1; i < 2; i++) {
			for (int j = -1; j < 2; j++) {
				cv::floodFill(*bmp_, point, 255, (cv::Rect*)0, cv::Scalar(), 200);
			}
		}
	}
}

void Voronoi::CreateSites() {
	int w = bmp_->size().width, h = bmp_->size().height, d;
	for (int hh = 0; hh < h; hh++) {
		for (int ww = 0; ww < w; ww++) {
			int ind = -1, dist = INT_MAX;
			for (size_t it = 0; it < points_.size(); it++) {
				cv::Point2f p = points_[it];
				d = DistanceSqrd(p, ww, hh);
				if (d < dist) {
					dist = d;
					ind = it;
				}
			}

			if (ind > -1)
			{
				// set pixel
				bmp_->at<cv::Vec3b>(cv::Point(ww, hh)) = colors_[ind];
				//SetPixel(bmp_->hdc(), ww, hh, colors_[ind]);
			}
		}
	}
}

void Voronoi::SetSitesPoints() {
	for (const auto& point : points_) {
		int x = point.x, y = point.y;
		for (int i = -1; i < 2; i++)
			for (int j = -1; j < 2; j++)
				bmp_->at<cv::Vec3b>(cv::Point(x + i, y + j)) = 0;
	}
}

void Voronoi::CreateColors() {
	for (size_t i = 0; i < points_.size(); i++) {
		cv::Vec3b color;
		color[0] = rand() & 255;
		color[1] = rand() & 255;
		color[2] = rand() & 255;

		colors_.push_back(color);
	}
}

int Voronoi::DistanceSqrd(cv::Point2f point, int x, int y) {
	int xd = x - point.x;
	int yd = y - point.y;

	return abs(xd) + abs(yd);
	//Math.abs(x1 - x2) + Math.abs(y1 - y2);
	//return (xd * xd) + (yd * yd);
}

