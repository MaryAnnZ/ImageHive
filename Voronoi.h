#include <cmath>
#include "jc_voronoi.h"
#include "Utils.h"

class Voronoi {
public:

	std::vector<std::pair<cv::Point, cv::Point>> Voronoi::getAllVoronoiEdges(std::vector<cv::Point> all, int width, int height);

	Voronoi() {};
	~Voronoi() {};


private:
	
	jcv_point recalcPoint(const jcv_point* pt, const jcv_point* min, const jcv_point* max, const jcv_point* scale);

};

