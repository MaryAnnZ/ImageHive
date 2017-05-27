#include "Utils.h"
#include "ImageAttributes.h"

class LocalCluster
{
public:
	
	LocalCluster() {};
	~LocalCluster() {};
		LocalCluster(ImageAttribute img, cv::Point localPiv, int height, int width);
		
		std::vector<cv::Point> globalPolygonVertices;
		std::vector<std::pair<cv::Point, cv::Point>> globalPolygonEdges;

		cv::Point localPivot;
		cv::Point globalPivot;


		ImageAttribute image;
		
	private:
		int cellHeight;
		int cellWidth;
	};



