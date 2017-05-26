#define JC_VORONOI_IMPLEMENTATION
#include "ImageHive.h"

int main()
{
	DataLoader loader = DataLoader();
	std::vector<cv::Mat> images = loader.loadDataset();
	std::vector<cv::String> filePaths = loader.getFilePaths();

	if (images.empty()) {
		return 0;
	}

	int resultHeight = 600;
	int resultWidth = 900;

	ResultImage result = ResultImage(resultWidth, resultHeight);

	//color and edge histogram
	std::vector<ImageAttribute> allImages(images.size());
	
	for (int i = 0; i < images.size(); i++) {
		//allImages.push_back(ImageAttribute::ImageAttribute(images.at(i), i, filePaths.at(i)));	
		ImageAttribute tmp = ImageAttribute::ImageAttribute(images.at(i), i, filePaths.at(i));
		allImages[i] = tmp;
		allImages.at(i).calcColorHistogram();
		allImages.at(i).calcHOG();
		allImages.at(i).calculateObjectness();
		allImages.at(i).calculateKeyPoints();
	}

	MyGraph graph = MyGraph();
	graph.buildGraph(allImages);

	graph.doClustering(allImages.size());
	graph.classesToString();
	graph.compareSiftForNeighborhood();
	graph.IAclassesToString();

	std::vector<Cluster> allClusters = doLocalClusters(allImages,graph.getClasses(),graph.getLocalClasses(),result);

	allClusters = drawGlobalClusters(allClusters, result);

	doVoronoi(allClusters,result);
		
	cvWaitKey(0);
		

	return 0;
}

std::vector<Cluster> doLocalClusters(std::vector<ImageAttribute> allImages,
									std::map<int, std::vector<MyEdge>> globalClasses,
									std::map<int, std::vector<MyGraph::SiftImg>> localClasses, 
									ResultImage result) {

	std::map<int, int> isAlreadyClustered; //image ID to cluster ID mapping

	for (int i = 0;i < allImages.size();i++) {
		isAlreadyClustered.insert({ allImages[i].getId(),0});
	}

	std::map<int, std::vector<ImageAttribute>> globalCusterMapping;

	for (int i = 0;i < globalClasses.size();i++) {
		for (int num = 0;num < globalClasses[i].size();num++) {
			if (isAlreadyClustered[globalClasses[i][num].getStartImage().getId()]!=1) {
				isAlreadyClustered.at(globalClasses[i][num].getStartImage().getId()) = 1;
				globalCusterMapping[i].push_back(globalClasses[i][num].getStartImage());
				
			}

			if (isAlreadyClustered[globalClasses[i][num].getEndImage().getId()]!=1) {
				isAlreadyClustered.at(globalClasses[i][num].getEndImage().getId()) = 1;
				globalCusterMapping[i].push_back(globalClasses[i][num].getEndImage());

			}
		}
	}
	
	std::vector<Cluster> allLocalClusters;

	bool even = (int)globalCusterMapping.size() % 2 == 0;

	std::vector<int> colRowLayout = utils.gridLayout[globalCusterMapping.size()];
	int colCount = colRowLayout[0];
	int rowCount = colRowLayout[1];
	int lastColCount = colCount - ((colCount*rowCount) - globalCusterMapping.size());

	int cellHeight = result.getHeight() / rowCount;
	
	int cellWidth = result.getWidth() / colCount;
	int lastCellWidth = result.getWidth() / lastColCount;


	for (int i = 0;i < globalCusterMapping.size();i++) {

		if (i > globalCusterMapping.size() - lastColCount) {
			allLocalClusters.push_back(createLocalPlacement(globalCusterMapping[i], cv::Mat(cellHeight, cellWidth, CV_8UC3, double(0))));
		}
		else {
			allLocalClusters.push_back(createLocalPlacement(globalCusterMapping[i], cv::Mat(cellHeight, lastCellWidth, CV_8UC3, double(0))));
		}

	}

	//doLocalVoronoi(allLocalClusters);

	return allLocalClusters;
	
}

Cluster createLocalPlacement(std::vector<ImageAttribute> allImages, cv::Mat localResult) {
	
	Cluster cluster(allImages);
	cluster.createLocalGraph();

	int imageIndex = 0;
	bool even = (int)allImages.size() % 2 == 0;

	std::vector<int> colRowLayout = utils.gridLayout[allImages.size()];
	int colCount = colRowLayout[0];
	int rowCount = colRowLayout[1];
	int lastColCount = colCount - ((colCount*rowCount) - allImages.size());

	int cellHeight = localResult.size().height / rowCount;

	//place all images on the result image (global placement)
	std::vector<std::vector<Mat>> canvasCluster(rowCount, std::vector<Mat>(colCount));
	cv::Mat tmpResult = cv::Mat(localResult.size().height, localResult.size().width, CV_8UC3);

	int cellWidth = localResult.size().width / colCount;
	int lastCellWidth = localResult.size().width / lastColCount;

	int lastHeightResult = 0;
	int lastWidthResult = 0;

	for (int row = 0; row < rowCount;row++) {
		for (int col = 0;col < colCount && imageIndex + 1 <= (allImages.size());col++) {
			Size oldSize = allImages[imageIndex].getCropped().size();
			cv::Mat resizedImage;
			int currentCellWidth = cellWidth;
			if (row == rowCount - 1) {
				//last row, split different!
				currentCellWidth = lastCellWidth;
			}

			if (oldSize.width >= oldSize.height) {
				float widthRatio = (float)currentCellWidth / (float)oldSize.width;
				resizedImage = allImages[imageIndex].resize(allImages[imageIndex].getCropped(),Size(currentCellWidth - 1, min(cellHeight, oldSize.height*widthRatio)));
			}
			else {
				float heightRatio = (float)cellHeight / (float)oldSize.height;
				resizedImage = allImages[imageIndex].resize(allImages[imageIndex].getCropped(),Size(min(currentCellWidth, oldSize.width*heightRatio), cellHeight - 1));
			}

			canvasCluster.at(row).at(col) = resizedImage;
			Size size = canvasCluster.at(row).at(col).size();

			canvasCluster.at(row).at(col).copyTo(tmpResult(Rect(lastWidthResult, lastHeightResult, size.width, size.height)));
			cv::Point pos = cv::Point(lastWidthResult + (currentCellWidth / 2), lastHeightResult + (cellHeight / 2));

			cluster.addLocalCluster(allImages[imageIndex], pos, cellHeight, currentCellWidth);
				
			lastWidthResult += currentCellWidth;
			imageIndex++;
		}

		lastHeightResult += cellHeight;
		lastWidthResult = 1;
	}

	imshow("cluster_local"+ allImages[0].getId(), tmpResult);

	//set Resulting Image 
	cluster.setResult(tmpResult);

	return cluster;

}


std::vector<Cluster> drawGlobalClusters(std::vector<Cluster> allClusters, ResultImage globalResult) {
	
	int imageindex = 0;
	bool even = (int)allClusters.size() % 2 == 0;

	std::vector<int> colrowlayout = utils.gridLayout[allClusters.size()];
	int colcount = colrowlayout[0];
	int rowcount = colrowlayout[1];
	int lastcolcount = colcount - ((colcount*rowcount) - allClusters.size());

	int cellheight = globalResult.getHeight() / rowcount;

	//place all images on the result image (global placement)
	std::vector<std::vector<cv::Mat>> canvascluster(rowcount, std::vector<cv::Mat>(colcount));
	cv::Mat tmpResult = cv::Mat(globalResult.getHeight(), globalResult.getWidth(), CV_8UC3);

	int cellwidth = globalResult.getWidth() / colcount;
	int lastcellwidth = globalResult.getWidth() / lastcolcount;

	int lastheightresult = 0;
	int lastwidthresult = 0;

	for (int row = 0; row < rowcount;row++) {
		for (int col = 0;col < colcount && imageindex + 1 <= (allClusters.size());col++) {
			Size oldsize = allClusters[imageindex].getResult().size();
			cv::Mat resizedimage;
			int currentcellwidth = cellwidth;
			if (row == rowcount - 1) {
				//last row, split different!
				currentcellwidth = lastcellwidth;
			}

			if (oldsize.width >= oldsize.height) {
				float widthratio = (float)currentcellwidth / (float)oldsize.width;
				cv::resize(allClusters[imageindex].getResult(), resizedimage, Size(currentcellwidth - 1, min(cellheight, oldsize.height*widthratio)));
			}
			else {
				float heightratio = (float)cellheight / (float)oldsize.height;
				cv::resize(allClusters[imageindex].getResult(), resizedimage, Size(min(currentcellwidth, oldsize.width*heightratio), cellheight - 1));
			}

			canvascluster.at(row).at(col) = resizedimage;

			Size size = canvascluster.at(row).at(col).size();

			canvascluster.at(row).at(col).copyTo(tmpResult(Rect(lastwidthresult, lastheightresult, size.width, size.height)));
			
			allClusters[imageindex].setGlobalPos(lastwidthresult, lastheightresult);

			lastwidthresult += currentcellwidth;
			imageindex++;
		}

		lastheightresult += cellheight;
		lastwidthresult = 1;
	}

	globalResult.setResult(tmpResult);
	imshow("cluster_global", globalResult.getResult());

	return allClusters;
}

//void doLocalVoronoi(std::vector<Cluster> allclusters) {
//	for each (Cluster clus in allclusters) {
//
//		cv::Mat image = clus.getResult();
//
//		std::vector<cv::Point> allpositions = clus.getAllLocalPivots();
//		//std::map<cv::Point, ImageAttribute> pointsImageMapping = clus.getImagePointMapping();
//		
//		// define window names
//		string win_voronoi = "voronoi diagram";
//
//		// allocate space for voronoi diagram
//		Mat img_voronoi = Mat::zeros(image.rows, image.cols, CV_8UC3);
//
//		//Voronoi v;
//		//v.Make(&img_voronoi, allpositions);
//
//		imshow(win_voronoi, img_voronoi);
//		waitKey(0);
//
//	}
// }

void doVoronoi(std::vector<Cluster> allclusters, ResultImage globalResult) {
	
	std::vector<cv::Point> allpositions;
	
	/*for each (Cluster clus in allclusters) {
		for each (cv::Point Lclus in clus.getAllLocalPivots()) {
			if (Lclus.x < globalResult.getResult().cols &&Lclus.y < globalResult.getResult().rows) {
				allpositions.push_back(Point(Lclus.x, Lclus.y));
			}
		}
	}*/
	
	string win_voronoi = "voronoi diagram";
	// allocate space for voronoi diagram
	Mat img_voronoi = Mat::zeros(globalResult.getResult().rows, globalResult.getResult().cols, CV_8UC3);

	// Populate with random points
	for (int i = 0; i < 19; i++)
	{
		cv::Point p = cv::Point(1 + rand() % 900, 1 + rand() % 600);
		allpositions.push_back(p);

		cv::circle(img_voronoi, cv::Point(p.x, p.y), 1, Scalar(255, 255, 255), 3);
	}
	
	std::vector<std::pair<cv::Point, cv::Point>> voronoiEdges;

	calculatedAllVoronoiEdges(allpositions, img_voronoi.size().width, img_voronoi.size().height, &voronoiEdges);

	for each (std::pair<cv::Point, cv::Point> pair in voronoiEdges) {
		cv::line(img_voronoi, pair.first, pair.second, Scalar(1 + rand() % 255, 1 + rand() % 255, 1 + rand() % 255), 2, 8, 0);
	}
	
	imshow(win_voronoi, img_voronoi);

	allpositions.clear();
	waitKey(0);
}

//https://github.com/JCash/voronoi/tree/87c0ab219a531721a1dd7e57d5134444437d95c0
void calculatedAllVoronoiEdges(std::vector<cv::Point> all, int width, int height, std::vector<std::pair<cv::Point, cv::Point>>* edges) {
	
	jcv_point* points = 0;
	points = (jcv_point*)malloc(sizeof(jcv_point) * (size_t)all.size());
	int pointoffset = 10; // move the points inwards, for aestetic reasons
	srand(0);

	for (int i = 0; i < all.size(); ++i)
	{
		points[i].x = (float)(pointoffset + rand() % (width - 2 * pointoffset));
		points[i].y = (float)(pointoffset + rand() % (height - 2 * pointoffset));
	}

	jcv_diagram voronoi;
	jcv_rect* rect = 0;
	jcv_diagram_generate(all.size(), (const jcv_point*)points, (const jcv_rect*)rect, &voronoi);

	const jcv_edge* edge = jcv_diagram_get_edges(&voronoi);
	while (edge)
	{
		jcv_point p0 = recalcPoint(&edge->pos[0], &voronoi.min, &voronoi.max, width, height);
		jcv_point p1 = recalcPoint(&edge->pos[1], &voronoi.min, &voronoi.max, width, height);

		edges->push_back(std::pair<cv::Point, cv::Point>(Point(p0.x, p0.y), Point(p1.x, p1.y)));
		edge = edge->next;
	}
}

// Remaps the point from the input space to image space
jcv_point recalcPoint(const jcv_point* pt, const jcv_point* min, const jcv_point* max, int width, int height)
{
	jcv_point p;
	p.x = (pt->x - min->x) / (max->x - min->x) * width;
	p.y = (pt->y - min->y) / (max->y - min->y) * height;
	return p;
}



// Draw a single point
void draw_point(Mat& img, cv::Point fp, Scalar color)
{
	circle(img, fp, 2, color, CV_FILLED, CV_AA, 0);
}


