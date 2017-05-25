#include "Voronoi.h"



std::vector<std::pair<cv::Point, cv::Point>> Voronoi::getAllVoronoiEdges(std::vector<cv::Point> all, int width, int height)
{

	std::vector<std::pair<cv::Point, cv::Point>> result;

	jcv_point* points = 0;
	points = (jcv_point*)malloc(sizeof(jcv_point) * (size_t)all.size());
	int pointoffset = 5; // move the points inwards, for aestetic reasons
	srand(0);

	for (int i = 0; i < all.size(); ++i)
	{
		points[i].x = (float)(pointoffset + rand() % (width - 2 * pointoffset));
		points[i].y = (float)(pointoffset + rand() % (height - 2 * pointoffset));
	}

	jcv_diagram voronoi;
	jcv_rect* rect = 0;
	jcv_diagram_generate(all.size(), (const jcv_point*)points, (const jcv_rect*)rect, &voronoi);

	jcv_point dimensions;
	dimensions.x = (jcv_real)width;
	dimensions.y = (jcv_real)height;

	const jcv_edge* edge = jcv_diagram_get_edges(&voronoi);
	while (edge)
	{
		jcv_point p0 = recalcPoint(&edge->pos[0], &voronoi.min, &voronoi.max, &dimensions);
		jcv_point p1 = recalcPoint(&edge->pos[1], &voronoi.min, &voronoi.max, &dimensions);

		result.push_back(std::pair<cv::Point, cv::Point>(cv::Point(p0.x, p0.y), cv::Point(p1.x, p1.y)));
		edge = edge->next;
	}
}


// Remaps the point from the input space to image space
jcv_point Voronoi::recalcPoint(const jcv_point* pt, const jcv_point* min, const jcv_point* max, const jcv_point* scale)
{
	jcv_point p;
	p.x = (pt->x - min->x) / (max->x - min->x) * scale->x;
	p.y = (pt->y - min->y) / (max->y - min->y) * scale->y;
	return p;
}