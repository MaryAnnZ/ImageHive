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

	std::vector<Cluster*> allClusters = doLocalClusters(allImages,graph.getClasses(),graph.getLocalClasses(),result);

	drawGlobalClusters(allClusters, result);
	doVoronoi(allClusters,result);
		
	cvWaitKey(0);
		

	return 0;
}

std::vector<Cluster*> doLocalClusters(std::vector<ImageAttribute> allImages,
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
	
	std::vector<Cluster*> allLocalClusters;

	bool even = (int)globalCusterMapping.size() % 2 == 0;

	std::vector<int> colRowLayout = gridLayout[globalCusterMapping.size()];
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

Cluster* createLocalPlacement(std::vector<ImageAttribute> allImages, cv::Mat localResult) {
	
	Cluster* cluster = new Cluster(allImages);
	cluster->createLocalGraph();

	int imageIndex = 0;
	bool even = (int)allImages.size() % 2 == 0;

	std::vector<int> colRowLayout = gridLayout[allImages.size()];
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

			cluster->addLocalCluster(allImages[imageIndex], pos, cellHeight, currentCellWidth);
				
			cv::circle(tmpResult, pos, 4, Scalar(0, 255, 0));

			lastWidthResult += currentCellWidth;
			imageIndex++;
		}

		lastHeightResult += cellHeight;
		lastWidthResult = 1;
	}

	imshow("cluster_local"+ allImages[0].getId(), tmpResult);

	//set Resulting Image 
	cluster->setResult(tmpResult);

	return cluster;

}


std::vector<Cluster*> drawGlobalClusters(std::vector<Cluster*> allClusters, ResultImage globalResult) {
	
	int imageindex = 0;
	bool even = (int)allClusters.size() % 2 == 0;

	std::vector<int> colrowlayout = gridLayout[allClusters.size()];
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

	for (int clus = 0;clus < allClusters.size();clus++) {
		for (int Lclus = 0;Lclus < allClusters[clus]->allLocalClusters.size();Lclus++) {
			cv::circle(tmpResult, allClusters[clus]->allLocalClusters[Lclus]->globalPivot, 4, Scalar(0, 255, 0));
		}
	}

	for (int row = 0; row < rowcount;row++) {
		for (int col = 0;col < colcount && imageindex + 1 <= (allClusters.size());col++) {
			Size oldsize = allClusters[imageindex]->getResult().size();
			cv::Mat resizedimage;
			int currentcellwidth = cellwidth;
			if (row == rowcount - 1) {
				//last row, split different!
				currentcellwidth = lastcellwidth;
			}

			if (oldsize.width >= oldsize.height) {
				float widthratio = (float)currentcellwidth / (float)oldsize.width;
				cv::resize(allClusters[imageindex]->getResult(), resizedimage, Size(currentcellwidth - 1, min(cellheight, oldsize.height*widthratio)));
			}
			else {
				float heightratio = (float)cellheight / (float)oldsize.height;
				cv::resize(allClusters[imageindex]->getResult(), resizedimage, Size(min(currentcellwidth, oldsize.width*heightratio), cellheight - 1));
			}

			canvascluster.at(row).at(col) = resizedimage;

			Size size = canvascluster.at(row).at(col).size();

			canvascluster.at(row).at(col).copyTo(tmpResult(Rect(lastwidthresult, lastheightresult, size.width, size.height)));
			
			allClusters[imageindex]->setGlobalPos(lastwidthresult, lastheightresult);


			lastwidthresult += currentcellwidth;
			imageindex++;
		}

		lastheightresult += cellheight;
		lastwidthresult = 1;
	}

	for (int clus = 0;clus < allClusters.size();clus++) {
		for (int Lclus = 0;Lclus < allClusters[clus]->allLocalClusters.size();Lclus++) {
			cv::circle(tmpResult, allClusters[clus]->allLocalClusters[Lclus]->globalPivot, 7, Scalar(255, 0, 0));
		}
	}

	globalResult.setResult(tmpResult);
	imshow("cluster_global", tmpResult);

	return allClusters;
}



void doVoronoi(std::vector<Cluster*> allclusters, ResultImage globalResult) {
	
	std::vector<cv::Point> allpositions;

	std::map<int, LocalCluster*> pointMap;
	
	int i = 0;
	for (int clus = 0;clus < allclusters.size();clus++) {
		for (int Lclus = 0;Lclus < allclusters[clus]->allLocalClusters.size() ;Lclus++) {
			cv::Point globalPivot = cv::Point(allclusters[clus]->allLocalClusters[Lclus]->globalPivot);
			if (globalPivot.x < globalResult.getResult().cols &&globalPivot.y < globalResult.getResult().rows) {
				allpositions.push_back(Point(globalPivot.x, globalPivot.y));
				pointMap[i] = allclusters[clus]->allLocalClusters[Lclus];
				i++;
			}
			else {
				std::cout << globalPivot.x << " x error" << std::endl;
				std::cout << globalPivot.y << " y error" << std::endl;
				std::cout << "*******************************" << std::endl;
			}
		}
	}

	std::cout << pointMap.size() << " pointMap size" << std::endl;

	string win_voronoi = "voronoi diagram";
	// allocate space for voronoi diagram
	Mat cells = Mat::zeros(globalResult.getHeight(), globalResult.getWidth(), CV_8UC3);
		
	std::vector<std::pair<cv::Point, cv::Point>> voronoiEdges;

	calculatedAllVoronoiEdges(allpositions, cells.size().width, cells.size().height, pointMap, &voronoiEdges);


	for (int clus = 0;clus < allclusters.size();clus++) {
		for (int Lclus = 0;Lclus < allclusters[clus]->allLocalClusters.size();Lclus++) {
			for (int i = 0;i < allclusters[clus]->allLocalClusters[Lclus]->globalPolygonEdges.size();i++) {
				cv::line(cells, 
					allclusters[clus]->allLocalClusters[Lclus]->globalPolygonEdges.at(i).first, 
					allclusters[clus]->allLocalClusters[Lclus]->globalPolygonEdges.at(i).second, 
					Scalar(255, 255, 255), 1, 8, 0);

				
			}

			cv::circle(cells, allclusters[clus]->allLocalClusters[Lclus]->globalPivot, 2, Scalar(255, 255, 255));
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

			cv::circle(ouput, allclusters[clus]->allLocalClusters[Lclus]->globalPivot, 2, Scalar(255, 255, 255));
			
		}
		std::cout << clus << " cluster" << std::endl;
		std::cout << localCounter << " local" << std::endl;
		std::cout << "*******************************" << std::endl;
	}
	

	imshow("voronoi filled", ouput);
	imshow("voronoi cells", cells);

	allpositions.clear();
	waitKey(0);
}

void floodFill(Mat src, Mat out, Mat colorSrc, int pointx, int pointy, int srcX, int srcY, int srcWidth, int srcHeight) {

	std::stack<std::pair<cv::Point, cv::Point>> stack;
	stack.push(std::pair<cv::Point, cv::Point>(Point(pointx,pointy),Point(srcX, srcY)));
	
	int counter = 0;
	int newPointX = stack.top().first.x;
	int newPointy = stack.top().first.y;
	int newsrcX = stack.top().second.x;
	int newsrcY = stack.top().second.y;

	while (!stack.empty()&& counter <30000) {
		newPointX = stack.top().first.x;
		newPointy = stack.top().first.y;
		newsrcX = stack.top().second.x;
		newsrcY = stack.top().second.y;

		stack.pop();
		counter++;
		if (src.at<Vec3b>(newPointX, newPointy) == Vec3b(0, 0, 0) &&
			out.at<Vec3b>(newPointX, newPointy) == Vec3b(0, 0, 0) &&
			newPointX > 1 && newPointy > 1 &&
			newPointy < RESULT_HEIGHT - 1 &&newPointX < RESULT_WIDTH-1 &&
			newsrcX>1 && newsrcY>1 &&
			newsrcY < srcHeight - 1 && newsrcX < srcWidth - 1) {

			Vec3b newIntensity = colorSrc.at<Vec3b>(newsrcX, newsrcY);
			out.at<Vec3b>(newPointX, newPointy) = newIntensity;

			if (src.at<Vec3b>(newPointX + 1, newPointy) == Vec3b(0, 0, 0) &&
				out.at<Vec3b>(newPointX + 1, newPointy) == Vec3b(0, 0, 0)) {
				stack.push(std::pair<cv::Point, cv::Point>(Point(newPointX + 1, newPointy), Point(newsrcX + 1, newsrcY)));
			}
			if (src.at<Vec3b>(newPointX - 1, newPointy) == Vec3b(0, 0, 0) &&
				out.at<Vec3b>(newPointX - 1, newPointy) == Vec3b(0, 0, 0)) {
				stack.push(std::pair<cv::Point, cv::Point>(Point(newPointX - 1, newPointy), Point(newsrcX - 1, newsrcY)));
			}
			if (src.at<Vec3b>(newPointX, newPointy - 1) == Vec3b(0, 0, 0) &&
				out.at<Vec3b>(newPointX, newPointy - 1) == Vec3b(0, 0, 0)) {
				stack.push(std::pair<cv::Point, cv::Point>(Point(newPointX, newPointy - 1), Point(newsrcX, newsrcY - 1)));
			}
			if (src.at<Vec3b>(newPointX, newPointy + 1) == Vec3b(0, 0, 0) &&
				out.at<Vec3b>(newPointX, newPointy +1) == Vec3b(0, 0, 0)) {
				stack.push(std::pair<cv::Point, cv::Point>(Point(newPointX, newPointy + 1), Point(newsrcX, newsrcY + 1)));
			}
		}

		
	}

	/*Vec3b area = src.at<Vec3b>(pointx, pointy);
	Vec3b outArea = out.at<Vec3b>(pointx, pointy);
	src.at<Vec3b>(pointx, pointy) = Vec3b(255, 0, 0);
	if (area == Vec3b(0,0,0) && outArea == Vec3b(0, 0, 0) && pointx>6 && pointy>6 && pointy<RESULT_HEIGHT&& pointx<RESULT_WIDTH
		&& srcX>6 && srcY>6 && srcY<srcHeight&& srcX<srcWidth)
	{
		Vec3b newIntensity = colorSrc.at<Vec3b>(srcX, srcY);
		out.at<Vec3b>(pointx, pointy) = newIntensity;
		
		floodFill(src, out, colorSrc, pointx + 5, pointy, srcX+5, srcY, srcWidth, srcHeight);
		floodFill(src, out, colorSrc, pointx - 5, pointy, srcX-5, srcY, srcWidth, srcHeight);
		floodFill(src, out, colorSrc, pointx, pointy + 5, srcX, srcY+5, srcWidth, srcHeight);
		floodFill(src, out, colorSrc, pointx, pointy - 5, srcX, srcY-5, srcWidth, srcHeight);
	}*/
}


//https://github.com/JCash/voronoi/tree/87c0ab219a531721a1dd7e57d5134444437d95c0
void calculatedAllVoronoiEdges(std::vector<cv::Point> all, int width, int height, std::map<int, LocalCluster*> clusterMapping, std::vector<std::pair<cv::Point,cv::Point>> *edges) {
	
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
	for (int i = 0; i < 300; ++i)
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
			clusterMapping.at(i)->globalPolygonVertices.push_back(Point(p0.x, p0.y));
			clusterMapping.at(i)->globalPolygonEdges.push_back(std::pair<cv::Point, cv::Point>(Point(p0.x, p0.y), Point(p1.x, p1.y)));

			e = e->next;
		}

		clusterMapping.at(i)->globalPivot = cv::Point(points[i].x, points[i].y);
	}
	
	jcv_diagram_free(&voronoi);
	free(points);
	free(rect);

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


