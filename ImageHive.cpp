#define JC_VORONOI_IMPLEMENTATION
#include "ImageHive.h"

//col,row
std::map<int, std::vector<int>> gridLayout = { { 1,{ 1,1 } },{ 2,{ 2,1 } },{ 3,{ 2,2 } },
{ 4,{ 2,2 } },{ 5,{ 3,2 } },{ 6,{ 3,2 } },
{ 7,{ 3,3 } },{ 8,{ 3,3 } },{ 9,{ 3,3 } },
{ 10,{ 4,3 } },{ 11,{ 4,3 } },{ 12,{ 4,3 } } };

int main()
{
	DataLoader loader = DataLoader();
	std::vector<cv::Mat> images = loader.loadDataset();
	std::vector<cv::String> filePaths = loader.getFilePaths();

	if (images.empty()) {
		return 0;
	}
	
	ResultImage result = ResultImage(RESULT_WIDTH, RESULT_HEIGHT);

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

	std::vector<Cluster*> allClusters = createClusters(allImages, graph.getClasses(), graph.getLocalClasses(), result);

	calcuateLocalVoronoi(allClusters, result);
		
	cvWaitKey(0);
	return 0;
}

/**
* TODO
*/
std::vector<Cluster*> createClusters(std::vector<ImageAttribute> allImages,
	std::map<int, std::vector<MyEdge>> globalClasses,
	std::map<int, std::vector<MyGraph::SiftImg>> localClasses,
	ResultImage result) {

	//get results of similarity analysis, create cluster accordingly

	std::map<int, int> isAlreadyClustered; //image ID to cluster ID mapping

	for (int i = 0;i < allImages.size();i++) {
		isAlreadyClustered.insert({ allImages[i].getId(),0 });
	}

	std::map<int, std::vector<ImageAttribute>> globalCusterMapping;

	for (int i = 0;i < globalClasses.size();i++) {
		for (int num = 0;num < globalClasses[i].size();num++) {
			if (isAlreadyClustered[globalClasses[i][num].getStartImage().getId()] != 1) {
				isAlreadyClustered.at(globalClasses[i][num].getStartImage().getId()) = 1;
				globalCusterMapping[i].push_back(globalClasses[i][num].getStartImage());

			}

			if (isAlreadyClustered[globalClasses[i][num].getEndImage().getId()] != 1) {
				isAlreadyClustered.at(globalClasses[i][num].getEndImage().getId()) = 1;
				globalCusterMapping[i].push_back(globalClasses[i][num].getEndImage());

			}
		}
	}

	//create clusters with calculated height, width and position to get initial pivot for global voronoi
	std::vector<Cluster*> allGlobalClusters;

	bool even = (int)globalCusterMapping.size() % 2 == 0;

	std::vector<int> colRowLayout = gridLayout[globalCusterMapping.size()];
	int colCount = colRowLayout[0];
	int rowCount = colRowLayout[1];
	int lastColCount = colCount - ((colCount*rowCount) - globalCusterMapping.size());

	int cellHeight = result.getHeight() / rowCount;
	int cellWidth = result.getWidth() / colCount;
	int lastCellWidth = result.getWidth() / lastColCount;
	

	int lastHeightResult = 0;
	int lastWidthResult = 0;

	int imageIndex = 0;

	cv::Point position = cv::Point(0, 0);
	cv::Point pivot = Point(0, 0);

	for (int row = 0; row < rowCount;row++) {
		for (int col = 0;col < colCount && imageIndex + 1 <= (globalCusterMapping.size());col++) {

			int currentCellWidth = cellWidth;
			if (row == rowCount - 1) {
				//last row, split different!
				currentCellWidth = lastCellWidth;
			}

			position = cv::Point(lastWidthResult, lastHeightResult);
			pivot = cv::Point(lastWidthResult + (currentCellWidth / 2), lastHeightResult + (cellHeight / 2));

			allGlobalClusters.push_back(createCluster(globalCusterMapping[imageIndex], cellHeight, currentCellWidth, position, pivot));

			lastWidthResult += currentCellWidth;
			imageIndex++;
		}

		lastHeightResult += cellHeight;
		lastWidthResult = 1;
	}

	
	//Do global voronoi, calcuated edges and safe them in cluster mapping
	std::vector<cv::Point> allpositions;
	std::map<int, Cluster*> pointMap;

	int i = 0;
	for (int clus = 0;clus < allGlobalClusters.size();clus++) {
			cv::Point pivot = cv::Point(allGlobalClusters[clus]->pivot);
			allpositions.push_back(Point(pivot.x, pivot.y));
			pointMap[i] = allGlobalClusters[clus];
			i++;
	}

	string GlobalVoronoi = "GlobalVoronoi";
	// allocate space for voronoi diagram
	Mat cells = Mat::zeros(result.getHeight(), result.getWidth(), CV_8UC3);

	std::vector<std::pair<cv::Point, cv::Point>> voronoiEdges;

	calculatedGlobalVoronoiEdges(allpositions, cells.size().width, cells.size().height, pointMap, &voronoiEdges);
	
	for (int clus = 0;clus < allGlobalClusters.size();clus++) {
		Scalar col = Scalar(rand() / 255, rand() / 255, rand() / 255);
			for (int i = 0;i < allGlobalClusters[clus]->polygonEdges.size();i++) {
				cv::line(cells,
					allGlobalClusters[clus]->polygonEdges.at(i).first,
					allGlobalClusters[clus]->polygonEdges.at(i).second,
					Scalar(255, 255, 255), 1, 8, 0);
			}

			allGlobalClusters[clus]->calculatedBoundingBox();

			cv::circle(cells, allGlobalClusters[clus]->pivot, 2, Scalar(255, 255, 255));
			cv::circle(cells, allGlobalClusters[clus]->position, 4, Scalar(255, 0, 0),8);

			for (int Lclus = 0;Lclus < allGlobalClusters[clus]->allLocalClusters.size();Lclus++) {
				cv::circle(cells, cv::Point(allGlobalClusters[clus]->allLocalClusters[Lclus]->globalPivot), 4, col,3);
				cv::circle(cells, cv::Point(allGlobalClusters[clus]->allLocalClusters[Lclus]->position), 4, col, 8);
			}

			//draw cells
			cv::line(cells,
				allGlobalClusters[clus]->boundingVertices[0],
				allGlobalClusters[clus]->boundingVertices[1],
				col, 1, 8, 0);

			cv::line(cells,
				allGlobalClusters[clus]->boundingVertices[1],
				allGlobalClusters[clus]->boundingVertices[2],
				col, 1, 8, 0);

			cv::line(cells,
				allGlobalClusters[clus]->boundingVertices[2],
				allGlobalClusters[clus]->boundingVertices[3],
				col, 1, 8, 0);

			cv::line(cells,
				allGlobalClusters[clus]->boundingVertices[3],
				allGlobalClusters[clus]->boundingVertices[0],
				col, 1, 8, 0);
	}

	imshow("voronoi cells", cells);
	
	waitKey(0);


	return allGlobalClusters;
}

/**
* TODO
*/
Cluster* createCluster(std::vector<ImageAttribute> allImages, int height, int width, cv::Point position, cv::Point pivot) {

	Cluster* cluster = new Cluster(allImages,height, width, position, pivot);
	cluster->createLocalGraph();

	int imageIndex = 0;
	bool even = (int)allImages.size() % 2 == 0;

	std::vector<int> colRowLayout = gridLayout[allImages.size()];
	int colCount = colRowLayout[0];
	int rowCount = colRowLayout[1];
	int lastColCount = colCount - ((colCount*rowCount) - allImages.size());

	int cellHeight = height / rowCount;
	int cellWidth = width / colCount;
	int lastCellWidth = width / lastColCount;

	int lastHeightResult = 0;
	int lastWidthResult = 0;

	cv::Point pos;
	cv::Point piv;

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
				resizedImage = allImages[imageIndex].resize(allImages[imageIndex].getCropped(), Size(currentCellWidth - 1, min(cellHeight, oldSize.height*widthRatio)));
			}
			else {
				float heightRatio = (float)cellHeight / (float)oldSize.height;
				resizedImage = allImages[imageIndex].resize(allImages[imageIndex].getCropped(), Size(min(currentCellWidth, oldSize.width*heightRatio), cellHeight - 1));
			}

			pos = cv::Point(position.x+lastWidthResult, position.y+lastHeightResult);
			piv = cv::Point(position.x + lastWidthResult + (currentCellWidth / 2), position.y + lastHeightResult + (cellHeight / 2));
			
			cluster->addLocalCluster(allImages[imageIndex], piv, cellHeight, currentCellWidth, pos);
			
			lastWidthResult += currentCellWidth;
			imageIndex++;
		}

		lastHeightResult += cellHeight;
		lastWidthResult = 1;
	}

	return cluster;

}


void calculatedGlobalVoronoiEdges(std::vector<cv::Point> all, int width, int height, std::map<int, Cluster*> clusterMapping, std::vector<std::pair<cv::Point, cv::Point>> *edges) {

	jcv_point* points = 0;
	points = (jcv_point*)malloc(sizeof(jcv_point) * (size_t)all.size());

	for (int i = 0; i < all.size(); ++i)
	{
		points[i].x = (float)(all.at(i).x);
		points[i].y = (float)(all.at(i).y);
	}

	jcv_point min;
	min.x = (float)(0);
	min.y = (float)(0);

	jcv_point max;
	max.x = (float)(width - 1);
	max.y = (float)(height - 1);

	jcv_rect* rect = (jcv_rect*)malloc(sizeof(jcv_rect));
	rect[0].min = min;
	rect[0].max = max;

	jcv_diagram voronoi;
	for (int i = 0; i < 100; ++i)
	{
		memset(&voronoi, 0, sizeof(jcv_diagram));
		jcv_diagram_generate(all.size(), (const jcv_point*)points, rect, &voronoi);

		relax_points(&voronoi, points);
	}

	// If you want to draw triangles, or relax the diagram,
	// you can iterate over the sites and get all edges easily
	const jcv_site* sites = jcv_diagram_get_sites(&voronoi);

	for (int i = 0; i < voronoi.numsites; ++i)
	{
		const jcv_site* site = &sites[i];
		const jcv_graphedge* e = site->edges;

		while (e)
		{
			jcv_point p0 = recalcPoint(&e->pos[0], &voronoi.min, &voronoi.max, width, height);
			jcv_point p1 = recalcPoint(&e->pos[1], &voronoi.min, &voronoi.max, width, height);



			edges->push_back(std::pair<cv::Point, cv::Point>(Point(p0.x, p0.y), Point(p1.x, p1.y)));
			clusterMapping.at(i)->polygonVertices.push_back(Point(p0.x, p0.y));
			clusterMapping.at(i)->polygonEdges.push_back(std::pair<cv::Point, cv::Point>(Point(p0.x, p0.y), Point(p1.x, p1.y)));

			e = e->next;
		}

		clusterMapping.at(i)->pivot = cv::Point(points[i].x, points[i].y);
	}

	jcv_diagram_free(&voronoi);
	free(points);
	free(rect);

}


void calcuateLocalVoronoi(std::vector<Cluster*> allclusters, ResultImage globalResult) {

	string win_voronoi = "localVoronoi";

	// allocate space for voronoi diagram
	Mat cells = Mat::zeros(globalResult.getHeight(), globalResult.getWidth(), CV_8UC3);
	cells.setTo(cv::Scalar(0, 0, 255));

	std::vector<cv::Point> allCalculatedpositions;

	for (int clus = 0;clus < allclusters.size();clus++) {
		std::vector<cv::Point> allpositions;

		//get all Local positions
		std::map<int, LocalCluster*> pointMap;

		int i = 0;

		for (int Lclus = 0;Lclus < allclusters[clus]->allLocalClusters.size();Lclus++) {
			cv::Point globalPivot = cv::Point(allclusters[clus]->allLocalClusters[Lclus]->globalPivot);
			allpositions.push_back(Point(globalPivot.x - allclusters[clus]->position.x, globalPivot.y - allclusters[clus]->position.y));
			pointMap[i] = allclusters[clus]->allLocalClusters[Lclus];
			i++;
		}



		std::vector<std::pair<cv::Point, cv::Point>> voronoiEdges;

		calculatedLocalVoronoiEdges(allclusters[clus], allpositions, pointMap, &voronoiEdges, allclusters[clus]->position);


		for (int clus = 0;clus < allclusters.size();clus++) {
			Scalar col = Scalar(255, 255, 255);
			for (int Lclus = 0;Lclus < allclusters[clus]->allLocalClusters.size();Lclus++) {
				for (int i = 0;i < allclusters[clus]->allLocalClusters[Lclus]->globalPolygonEdges.size();i++) {
					cv::line(cells,
						allclusters[clus]->allLocalClusters[Lclus]->globalPolygonEdges.at(i).first,
						allclusters[clus]->allLocalClusters[Lclus]->globalPolygonEdges.at(i).second,
						col, 1, 8, 0);

					////draw cells
					//cv::line(cells,
					//	allclusters[clus]->allLocalClusters[Lclus]->position,
					//	cv::Point(allclusters[clus]->allLocalClusters[Lclus]->position.x + allclusters[clus]->allLocalClusters[Lclus]->cellWidth,
					//		allclusters[clus]->allLocalClusters[Lclus]->position.y)
					//	, Scalar(255, 255, 255), 1, 8, 0);

					//cv::line(cells,
					//	cv::Point(allclusters[clus]->allLocalClusters[Lclus]->position.x + allclusters[clus]->allLocalClusters[Lclus]->cellWidth,
					//		allclusters[clus]->allLocalClusters[Lclus]->position.y),
					//	cv::Point(allclusters[clus]->allLocalClusters[Lclus]->position.x + allclusters[clus]->allLocalClusters[Lclus]->cellWidth,
					//		allclusters[clus]->allLocalClusters[Lclus]->position.y + allclusters[clus]->allLocalClusters[Lclus]->cellHeight),
					//	Scalar(255, 255, 255), 1, 8, 0);

					//cv::line(cells,
					//	cv::Point(allclusters[clus]->allLocalClusters[Lclus]->position.x + allclusters[clus]->allLocalClusters[Lclus]->cellWidth,
					//		allclusters[clus]->allLocalClusters[Lclus]->position.y + allclusters[clus]->allLocalClusters[Lclus]->cellHeight),
					//	cv::Point(allclusters[clus]->allLocalClusters[Lclus]->position.x,
					//		allclusters[clus]->allLocalClusters[Lclus]->position.y + allclusters[clus]->allLocalClusters[Lclus]->cellHeight),
					//	Scalar(255, 255, 255), 1, 8, 0);

					//cv::line(cells,
					//	cv::Point(allclusters[clus]->allLocalClusters[Lclus]->position.x,
					//		allclusters[clus]->allLocalClusters[Lclus]->position.y + allclusters[clus]->allLocalClusters[Lclus]->cellHeight),
					//	cv::Point(allclusters[clus]->allLocalClusters[Lclus]->position),
					//	Scalar(255, 255, 255), 1, 8, 0);
				}

				//cv::circle(cells, allclusters[clus]->allLocalClusters[Lclus]->globalPivot, 3, Scalar(0, 0, 255));
			}
		}

		cv::Mat ouput = Mat::zeros(globalResult.getHeight(), globalResult.getWidth(), CV_8UC3);

		for (int clus = 0;clus < allclusters.size();clus++) {
			int localCounter = 0;
			for (int Lclus = 0;Lclus < allclusters[clus]->allLocalClusters.size();Lclus++) {
				localCounter++;
				floodFill(cells, ouput,
					allclusters[clus]->allLocalClusters[Lclus]->image.getImage(),
					allclusters[clus]->allLocalClusters[Lclus]->globalPivot.x,
					allclusters[clus]->allLocalClusters[Lclus]->globalPivot.y,
					allclusters[clus]->allLocalClusters[Lclus]->image.getImage().size().width / 2,
					allclusters[clus]->allLocalClusters[Lclus]->image.getImage().size().height / 2,
					allclusters[clus]->allLocalClusters[Lclus]->image.getImage().size().width,
					allclusters[clus]->allLocalClusters[Lclus]->image.getImage().size().height);

				allCalculatedpositions.push_back(allclusters[clus]->allLocalClusters[Lclus]->globalPivot);
				cv::circle(ouput, allclusters[clus]->allLocalClusters[Lclus]->globalPivot, 4, Scalar(255, 0, 0), 3);

			}
			//std::cout << clus << " cluster" << std::endl;
			//std::cout << localCounter << " local" << std::endl;
			//std::cout << "*******************************" << std::endl;
		}

		imshow("voronoi filled", ouput);

		voronoiEdges.clear();
		pointMap.clear();
		allpositions.clear();
	}
	imshow("voronoi Local cells", cells);

	waitKey(0);
}


//https://github.com/JCash/voronoi/tree/87c0ab219a531721a1dd7e57d5134444437d95c0
void calculatedLocalVoronoiEdges(Cluster* clus, std::vector<cv::Point> all, std::map<int, LocalCluster*> clusterMapping, std::vector<std::pair<cv::Point, cv::Point>> *edges, cv::Point offsetPosition) {

	int height = clus->height;
	int width = clus->width;

	jcv_point* points = 0;
	points = (jcv_point*)malloc(sizeof(jcv_point) * (size_t)all.size());

	for (int i = 0; i < all.size(); ++i)
	{
		points[i].x = (float)(all.at(i).x);
		points[i].y = (float)(all.at(i).y);
	}

	jcv_point min;
	min.x = (float)(0);
	min.y = (float)(0);
	
	jcv_point max;
	max.x = (float)(width-1);
	max.y = (float)(height-1);

	jcv_rect* rect =(jcv_rect*) malloc(sizeof(jcv_rect));
	rect[0].min = min;
	rect[0].max = max;

	jcv_diagram voronoi;
	for (int i = 0; i < 100; ++i)
	{
		memset(&voronoi, 0, sizeof(jcv_diagram));
		jcv_diagram_generate(all.size(), (const jcv_point*)points, rect, &voronoi);

		relax_points(&voronoi, points);
	}
	
	// allocate space for voronoi diagram
	Mat local = Mat::zeros(height, width, CV_8UC3);

	// If you want to draw triangles, or relax the diagram,
	// you can iterate over the sites and get all edges easily
	const jcv_site* sites = jcv_diagram_get_sites(&voronoi);
	for (int i = 0; i < voronoi.numsites; ++i)
	{
		const jcv_site* site = &sites[i];
		
		const jcv_graphedge* e = site->edges;
		while (e)
		{
			jcv_point p0 = recalcPoint(&e->pos[0], &voronoi.min, &voronoi.max, clus);
			jcv_point p1 = recalcPoint(&e->pos[1], &voronoi.min, &voronoi.max, clus);

			cv::line(local,
				Point(p0.x,p0.y),
				Point(p1.x, p1.y),
				Scalar(255,0,0), 3, 7, 0);

			edges->push_back(std::pair<cv::Point, cv::Point>(Point(p0.x+offsetPosition.x, p0.y + offsetPosition.y), Point(p1.x+offsetPosition.x, p1.y+offsetPosition.y)));
			clusterMapping.at(i)->globalPolygonVertices.push_back(Point(p0.x + offsetPosition.x, p0.y + offsetPosition.y));
			clusterMapping.at(i)->globalPolygonEdges.push_back(std::pair<cv::Point, cv::Point>(Point(p0.x + offsetPosition.x, p0.y + offsetPosition.y), Point(p1.x + offsetPosition.x, p1.y + offsetPosition.y)));

			e = e->next;
		}

		clusterMapping.at(i)->globalPivot = cv::Point(points[i].x + offsetPosition.x, points[i].y + offsetPosition.y);
	}
	
	imshow("voronoi_Local"+ height+width, local);

	jcv_diagram_free(&voronoi);
	free(points);
	free(rect);

}

// Remaps the point from the input space to image space
jcv_point recalcPoint(const jcv_point* pt, const jcv_point* min, const jcv_point* max, Cluster* clus)
{
	int width = clus->width;
	int height = clus->height;

	jcv_point p;
	p.x = (pt->x - min->x) / (max->x - min->x) * width;
	p.y = (pt->y - min->y) / (max->y - min->y) * height;
	return p;
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

Mat drawHelperLines(Mat img) {

	/*Mat newImg = Mat(img);
	int colCount = newImg.cols/100;
	int rowCount = newImg.rows / 100;

	for (int x = newImg.cols-1;x >= 0;x -= 100) {
		line(newImg, cv::Point(max(0, x), 0),cv::Point(max(0, x), newImg.rows-1), Scalar(255, 0, 0));
	}

	for (int y = newImg.rows - 1;y >= 0;y -= 100) {
		line(newImg, cv::Point(0, max(0, y)), cv::Point(newImg.cols - 1, max(0, y)), Scalar(0, 255, 0));
	}*/

	return img;
}


void floodFill(Mat src, Mat out, Mat colorSrc, int pointx, int pointy, int srcX, int srcY, int srcWidth, int srcHeight) {

	std::stack<std::pair<cv::Point, cv::Point>> stack;
	stack.push(std::pair<cv::Point, cv::Point>(Point(pointx, pointy), Point(srcX, srcY)));
	
	int counter = 0;
	int newPointX = 0;
	int newPointy = 0;
	int newsrcX = 0;
	int newsrcY = 0;

	while (!stack.empty() && counter < 900000) {
		newPointX = stack.top().first.x;
		newPointy = stack.top().first.y;
		newsrcX = stack.top().second.x;
		newsrcY = stack.top().second.y;

		stack.pop();
		counter++;
		if (newPointX > 1 && newPointy > 1 &&
			newPointy < (RESULT_HEIGHT - 1) && newPointX < (RESULT_WIDTH - 1) &&
			newsrcX > 1 && newsrcY > 1 &&
			newsrcY < (srcHeight - 1) && newsrcX < (srcWidth - 1))
			 {
			if (src.at<Vec3b>(newPointy, newPointX) == Vec3b(0, 0, 255) &&
				out.at<Vec3b>(newPointy, newPointX) == Vec3b(0, 0, 0)) {

				Vec3b newIntensity = colorSrc.at<Vec3b>(newsrcY, newsrcX);
				out.at<Vec3b>(newPointy, newPointX) = newIntensity;
				src.at<Vec3b>(newPointy, newPointX) == Vec3b(0, 255, 0);

				stack.push(std::pair<cv::Point, cv::Point>(Point(newPointX + 1, newPointy), Point(newsrcX + 1, newsrcY)));
				stack.push(std::pair<cv::Point, cv::Point>(Point(newPointX - 1, newPointy), Point(newsrcX - 1, newsrcY)));
				stack.push(std::pair<cv::Point, cv::Point>(Point(newPointX, newPointy - 1), Point(newsrcX, newsrcY - 1)));
				stack.push(std::pair<cv::Point, cv::Point>(Point(newPointX, newPointy + 1), Point(newsrcX, newsrcY + 1)));
				}
			}
		}


}





void relax_points(const jcv_diagram* diagram, jcv_point* points)
{
	const jcv_site* sites = jcv_diagram_get_sites(diagram);
	for (int i = 0; i < diagram->numsites; ++i)
	{
		const jcv_site* site = &sites[i];
		jcv_point sum = site->p;
		int count = 1;

		const jcv_graphedge* edge = site->edges;

		while (edge)
		{
			sum.x += edge->pos[0].x;
			sum.y += edge->pos[0].y;
			++count;
			edge = edge->next;
		}

		points[site->index].x = sum.x / count;
		points[site->index].y = sum.y / count;
	}
}


