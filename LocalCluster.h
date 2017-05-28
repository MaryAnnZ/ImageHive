#include "Utils.h"
#include "ImageAttributes.h"

class LocalCluster
{
public:
	
	LocalCluster() {};
	~LocalCluster() {};
		LocalCluster(ImageAttribute img, cv::Point globalPivot, int height, int width, cv::Point position);
		
		std::vector<cv::Point> globalPolygonVertices;
		std::vector<std::pair<cv::Point, cv::Point>> globalPolygonEdges;

		cv::Point globalPivot;

		cv::Point position;
		ImageAttribute image;
		
		int cellHeight;
		int cellWidth;


	};



