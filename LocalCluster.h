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
		
		cv::Mat getImage() { return image.getImage(); };
		cv::Mat getSaliencyCroppedImage();
		
		int cellHeight;
		int cellWidth;

		void calculateBoundingBox();
		std::vector<cv::Point> boundingVertices;


private:
	ImageAttribute image;

	int boundingHeight;
	int boundingWidth;
	


	};



