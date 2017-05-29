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
		allImages.at(i).calculateObjectness();
		allImages.at(i).calculateKeyPoints();
	}

	MyGraph graph = MyGraph();
	graph.buildGraph(allImages);

	graph.doClustering(allImages.size());
	graph.classesToString();
	graph.compareSiftForNeighborhood();
	graph.IAclassesToString();

	std::vector<Cluster*> allClusters = createClustersAndGlobalVoronoi(allImages, graph.getClasses(), graph.getLocalClasses(), result);
	
	allClusters = doLocalVoronoi(allClusters, result);
		
	doGLobalVoronoiWithLocalClusters(allClusters, result);

	cvWaitKey(0);
	return 0;
}

/**
* TODO
*/
std::vector<Cluster*> createClustersAndGlobalVoronoi(std::vector<ImageAttribute> allImages,
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

	int maxClusterSize = 0;
	int mostClusters = 0;
	for (int i = 0;i < globalCusterMapping.size();i++) {
		if (globalCusterMapping[i].size() > maxClusterSize) {
			mostClusters = i;
			maxClusterSize = globalCusterMapping[i].size();
		}
	}

	if (mostClusters != (globalCusterMapping.size() - 1)) {
		std::vector<ImageAttribute> tmp = globalCusterMapping[globalCusterMapping.size() - 1];
		globalCusterMapping[globalCusterMapping.size() - 1] = globalCusterMapping[mostClusters];
		globalCusterMapping[mostClusters] = tmp;
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
	std::vector<Cluster*> pointMap;

	int i = 0;
	for (int clus = 0;clus < allGlobalClusters.size();clus++) {
			cv::Point pivot = cv::Point(allGlobalClusters[clus]->pivot);
			allpositions.push_back(Point(pivot.x, pivot.y));
			pointMap.push_back(allGlobalClusters[clus]);
			i++;
	}

	string GlobalVoronoi = "GlobalVoronoi";
	// allocate space for voronoi diagram
	Mat cells = Mat::zeros(result.getHeight(), result.getWidth(), CV_8UC3);
	
	calculateGlobalVoronoiEdges(allpositions, cells.size().width, cells.size().height, pointMap);
	
	for (int clus = 0;clus < allGlobalClusters.size();clus++) {
		Scalar col = Scalar(rand() / 255, rand() / 255, rand() / 255);
			for (int i = 0;i < allGlobalClusters[clus]->polygonEdges.size();i++) {
				cv::line(cells,
					allGlobalClusters[clus]->polygonEdges.at(i).first,
					allGlobalClusters[clus]->polygonEdges.at(i).second,
					Scalar(255, 255, 255), 1, 8, 0);
			}

			cv::circle(cells, allGlobalClusters[clus]->pivot, 2, Scalar(255, 255, 255));

			for (int Lclus = 0;Lclus < allGlobalClusters[clus]->allLocalClusters.size();Lclus++) {
				cv::circle(cells, cv::Point(allGlobalClusters[clus]->allLocalClusters[Lclus]->globalPivot), 4, col,3);
				cv::circle(cells, cv::Point(allGlobalClusters[clus]->allLocalClusters[Lclus]->position), 4, col, 8);
			}

			drawBoundingBox(cells, allGlobalClusters[clus], col);
	}

	cv::imshow("voronoi cells", cells);
	
	cv::waitKey(0);


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
			int currentCellWidth = cellWidth;
			if (row == rowCount - 1) {
				//last row, split different!
				currentCellWidth = lastCellWidth;
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


void calculateGlobalVoronoiEdges(std::vector<cv::Point> all, int width, int height, std::vector<Cluster*> clusterMapping) {

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
	for (int i = 0; i < 700; ++i)
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
			
			clusterMapping.at(i)->polygonVertices.push_back(Point(p0.x, p0.y));
			clusterMapping.at(i)->polygonEdges.push_back(std::pair<cv::Point, cv::Point>(Point(p0.x, p0.y), Point(p1.x, p1.y)));

			e = e->next;
		}

		clusterMapping.at(i)->pivot = cv::Point(points[i].x, points[i].y);
		clusterMapping.at(i)->calculatedBoundingBox();
	}

	jcv_diagram_free(&voronoi);
	std::free(points);
	std::free(rect);

}


std::vector<Cluster*> doLocalVoronoi(std::vector<Cluster*> allclusters, ResultImage globalResult) {

	string win_voronoi = "localVoronoi";

	Mat cells = Mat(globalResult.getHeight(), globalResult.getWidth(), CV_8UC3, Scalar(0, 0, 255));
	
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

		calculateLocalVoronoiEdges(allclusters[clus], allpositions, pointMap, allclusters[clus]->position);


		for (int clus = 0;clus < allclusters.size();clus++) {
			Scalar col = Scalar(255, 255, 255);
			for (int Lclus = 0;Lclus < allclusters[clus]->allLocalClusters.size();Lclus++) {
				for (int i = 0;i < allclusters[clus]->allLocalClusters[Lclus]->globalPolygonEdges.size();i++) {
					cv::line(cells,
						allclusters[clus]->allLocalClusters[Lclus]->globalPolygonEdges.at(i).first,
						allclusters[clus]->allLocalClusters[Lclus]->globalPolygonEdges.at(i).second,
						col, 3, 8, 0);

				}
/*
				for (int i = 0;i < allclusters[clus]->allLocalClusters[Lclus]->globalPolygonVertices.size();i++) {
					cv::circle(cells, allclusters[clus]->allLocalClusters[Lclus]->globalPolygonVertices[i], 3, Scalar(0, 255, 0));
				}*/
			}
		}

		Mat ouput = Mat(globalResult.getHeight(), globalResult.getWidth(), CV_8UC3, Scalar(0, 0, 0));

		for (int clus = 0;clus < allclusters.size();clus++) {
			for (int Lclus = 0;Lclus < allclusters[clus]->allLocalClusters.size();Lclus++) {
				cv::Mat tmp = allclusters[clus]->allLocalClusters[Lclus]->getSaliencyCroppedImage();

					floodFill(cells, ouput,tmp,
						allclusters[clus]->allLocalClusters[Lclus]);
					
			}
		}

		cv::imshow("Local Voronoi cells filled", ouput);

		voronoiEdges.clear();
		pointMap.clear();
		allpositions.clear();
	}

	cv::imshow("Local Voronoi cells", cells);
	cv::waitKey(0);

	return allclusters;
}


//https://github.com/JCash/voronoi/tree/87c0ab219a531721a1dd7e57d5134444437d95c0
void calculateLocalVoronoiEdges(Cluster* clus, std::vector<cv::Point> all, std::map<int, LocalCluster*> clusterMapping, cv::Point offsetPosition) {

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
	for (int i = 0; i < 700; ++i)
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
			
			clusterMapping.at(i)->globalPolygonVertices.push_back(Point(p0.x + offsetPosition.x, p0.y + offsetPosition.y));
			clusterMapping.at(i)->globalPolygonEdges.push_back(std::pair<cv::Point, cv::Point>(Point(p0.x + offsetPosition.x, p0.y + offsetPosition.y), Point(p1.x + offsetPosition.x, p1.y + offsetPosition.y)));

			e = e->next;
		}

		
		clusterMapping.at(i)->globalPivot = cv::Point(points[i].x + offsetPosition.x, points[i].y + offsetPosition.y);
		clusterMapping.at(i)->calculateBoundingBox();
	}
	
	cv::imshow("voronoi_Local"+ height+width, local);

	jcv_diagram_free(&voronoi);
	std::free(points);
	std::free(rect);

}


/**
* Testing voronoi with local points
**/
void doGLobalVoronoiWithLocalClusters(std::vector<Cluster*>  allClusters, ResultImage result) {

	string win_voronoi = "localVoronoi";

	// allocate space for voronoi diagram
	Mat cells = Mat::zeros(result.getHeight(), result.getWidth(), CV_8UC3);
	cells.setTo(cv::Scalar(0, 0, 0));

	std::vector<cv::Point> allCalculatedpositions;

	std::vector<cv::Point> allpositions;
	std::vector<LocalCluster*> pointMap;

	int i = 0;
	for (int clus = 0;clus < allClusters.size();clus++) {
		for (int Lclus = 0;Lclus < allClusters[clus]->allLocalClusters.size();Lclus++) {
			allpositions.push_back(cv::Point(allClusters[clus]->allLocalClusters[Lclus]->globalPivot));
			pointMap.push_back(allClusters[clus]->allLocalClusters[Lclus]);
			i++;
		}
	}
	
	calcGLobalWithLocalVoronoi(allpositions, pointMap, cells);
	
	imshow("TEST cells", cells);

	waitKey(0);
	
	pointMap.clear();
	allpositions.clear();
}

/**
* Do GLobal Voronoi with ALL calculated Local positions
* https://github.com/JCash/voronoi/tree/87c0ab219a531721a1dd7e57d5134444437d95c0
**/
void calcGLobalWithLocalVoronoi(std::vector<cv::Point> all, std::vector<LocalCluster*> map, cv::Mat global) {

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
	max.x = (float)(global.size().width - 1);
	max.y = (float)(global.size().height - 1);

	jcv_rect* rect = (jcv_rect*)malloc(sizeof(jcv_rect));
	rect[0].min = min;
	rect[0].max = max;

	jcv_diagram voronoi;

	memset(&voronoi, 0, sizeof(jcv_diagram));
	jcv_diagram_generate(all.size(), (const jcv_point*)points, rect, &voronoi);

	// If you want to draw triangles, or relax the diagram,
	// you can iterate over the sites and get all edges easily
	const jcv_site* sites = jcv_diagram_get_sites(&voronoi);
	for (int i = 0; i < voronoi.numsites; ++i)
	{
		const jcv_site* site = &sites[i];

		const jcv_graphedge* e = site->edges;
		while (e)
		{
			jcv_point p0 = recalcPoint(&e->pos[0], &voronoi.min, &voronoi.max, global.size().width, global.size().height);
			jcv_point p1 = recalcPoint(&e->pos[1], &voronoi.min, &voronoi.max, global.size().width, global.size().height);

			//Draw new Points/Lines
			cv::line(global, Point(p0.x, p0.y), Point(p1.x, p1.y), Scalar(255, 255, 255), 3, 7, 0);


			e = e->next;
		}

	}

	Mat boundarySrc = Mat::zeros(global.size().height, global.size().width, CV_8UC3);
	boundarySrc.setTo(Scalar(0, 0, 255));


	Mat output = Mat::zeros(global.size().height, global.size().width, CV_8UC3);
	output.setTo(Scalar(1, 2, 0));

	for (int i = 0; i < 1000; ++i)
	{
		memset(&voronoi, 0, sizeof(jcv_diagram));
		jcv_diagram_generate(all.size(), (const jcv_point*)points, rect, &voronoi);

		relax_points(&voronoi, points);

		for (int i = 0; i < all.size(); ++i)
		{
			cv::circle(global, Point(points[i].x, points[i].y), 2, Scalar(255, 0, 0), 3, 8);
		}
		
	}

	// If you want to draw triangles, or relax the diagram,
	// you can iterate over the sites and get all edges easily
	const jcv_site* sites2 = jcv_diagram_get_sites(&voronoi);
	for (int i = 0; i < voronoi.numsites; ++i)
	{
		const jcv_site* site = &sites2[i];

		std::vector<cv::Point> polygonVertices;

		const jcv_graphedge* e = site->edges;
		while (e)
		{
			jcv_point p0 = recalcPoint(&e->pos[0], &voronoi.min, &voronoi.max, global.size().width, global.size().height);
			jcv_point p1 = recalcPoint(&e->pos[1], &voronoi.min, &voronoi.max, global.size().width, global.size().height);

			//Draw new Points/Lines
			cv::line(global, Point(p0.x, p0.y), Point(p1.x, p1.y), Scalar(255, 0, 0), 3, 7, 0);
			cv::line(boundarySrc, Point(p0.x, p0.y), Point(p1.x, p1.y), Scalar(255, 255, 255), 3, 8, 0);

			polygonVertices.push_back(Point(p0.x, p0.y));

			e = e->next;
		}


		cv::Mat tmp = map[i]->getImage();

		floodFill(boundarySrc, output, tmp, map[i]);

		cv::circle(boundarySrc, cv::Point(points[i].x, points[i].y),3 , Scalar(0, 0, 0), 3, 8, 0);

		polygonVertices.clear();
	}


	imshow("output Global cells", output);
	imshow("boundry", boundarySrc);

	waitKey(0);

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


void floodFill(Mat src, Mat out, Mat colorSrc,LocalCluster* clus) {

	cv::Mat innerSal = clus->image.getCroppedImage2();
	std::vector<int> innerSaliencyVertices = clus->image.getCropped2Coords();

	int pivotX = (innerSal.size().width / 2) + innerSaliencyVertices[0];
	int pivotY = (innerSal.size().height / 2) + innerSaliencyVertices[1];

	std::stack<std::pair<cv::Point, cv::Point>> stack;
	stack.push(std::pair<cv::Point, cv::Point>(Point(clus->globalPivot.x, clus->globalPivot.x), Point(pivotX, pivotY)));
	
	int counter = 0;
	int newPointX = 0;
	int newPointy = 0;
	int newsrcX = 0;
	int newsrcY = 0;

	while (!stack.empty()) {
		newPointX = stack.top().first.x;
		newPointy = stack.top().first.y;
		newsrcX = stack.top().second.x;
		newsrcY = stack.top().second.y;

		stack.pop();
		counter++;
		if (newPointX > 1 && newPointy > 1 &&
			newPointy < (RESULT_HEIGHT - 1) && newPointX < (RESULT_WIDTH - 1) &&
			newsrcX > 1 && newsrcY > 1 &&
			newsrcY < (src.size().height - 1) && newsrcX < (src.size().width - 1))
			 {

			if (out.at<Vec3b>(newPointy, newPointX) == Vec3b(0, 0, 0) &&
				src.at<Vec3b>(newPointy, newPointX) == Vec3b(0, 0, 255)) {

				if (newsrcX == pivotX && pivotY == newsrcY) {
					out.at<Vec3b>(newPointy, newPointX) = Vec3b(255, 0, 0);
				}
				else {
					out.at<Vec3b>(newPointy, newPointX) = colorSrc.at<Vec3b>(newsrcY, newsrcX);
				}
				
				stack.push(std::pair<cv::Point, cv::Point>(Point(newPointX + 1, newPointy), Point(newsrcX + 1, newsrcY)));
				stack.push(std::pair<cv::Point, cv::Point>(Point(newPointX - 1, newPointy), Point(newsrcX - 1, newsrcY)));
				stack.push(std::pair<cv::Point, cv::Point>(Point(newPointX, newPointy - 1), Point(newsrcX, newsrcY - 1)));
				stack.push(std::pair<cv::Point, cv::Point>(Point(newPointX, newPointy + 1), Point(newsrcX, newsrcY + 1)));
				}
			}
		}

}

void drawBoundingBox(cv::Mat cells, Cluster* clus, Scalar col) {

	//draw cells
	cv::line(cells,
		clus->boundingVertices[0],
		clus->boundingVertices[1],
		col, 1, 8, 0);

	cv::line(cells,
		clus->boundingVertices[1],
		clus->boundingVertices[2],
		col, 1, 8, 0);

	cv::line(cells,
		clus->boundingVertices[2],
		clus->boundingVertices[3],
		col, 1, 8, 0);

	cv::line(cells,
		clus->boundingVertices[3],
		clus->boundingVertices[0],
		col, 1, 8, 0);
}

void drawBoundingBox(cv::Mat cells, LocalCluster* Lclus, Scalar col) {
	//draw cells
	cv::line(cells,
		Lclus->boundingVertices[0],
		Lclus->boundingVertices[1],
		col, 1, 8, 0);

	cv::line(cells,
		Lclus->boundingVertices[1],
		Lclus->boundingVertices[2],
		col, 1, 8, 0);

	cv::line(cells,
		Lclus->boundingVertices[2],
		Lclus->boundingVertices[3],
		col, 1, 8, 0);

	cv::line(cells,
		Lclus->boundingVertices[3],
		Lclus->boundingVertices[0],
		col, 1, 8, 0);
}

// Draw a single point
void draw_point(Mat& img, cv::Point fp, Scalar color)
{
	circle(img, fp, 2, color, CV_FILLED, CV_AA, 0);
}



