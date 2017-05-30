#define JC_VORONOI_IMPLEMENTATION
#include "ImageHive.h"

int RESULT_HEIGHT = 600;
int RESULT_WIDTH = 900;

//col,row
std::map<int, std::vector<int>> gridLayout = { { 1,{ 1,1 } },{ 2,{ 2,1 } },{ 3,{ 2,2 } },
{ 4,{ 2,2 } },{ 5,{ 3,2 } },{ 6,{ 3,2 } },
{ 7,{ 3,3 } },{ 8,{ 3,3 } },{ 9,{ 3,3 } },
{ 10,{ 4,3 } },{ 11,{ 4,3 } },{ 12,{ 4,3 } } };

DataLoader loader;

MyGraph globalGraph;

std::vector<ImageAttribute> allImages;			///<Speichert alle Bilder

std::vector<Cluster*> allClusters;				///<Speichert alle Cluster

Mat globalClusters, voronoicells, filledCells;  ///<Speichert alle Ausgabebilder
Mat clusterIndex;

std::map<int,Cluster*> colorMappingGlobal;		///<Index damit man weiß welches CLuster angeklickt worden ist

//mode 0 = enlarge
//mode 1 = show cvt
int mode = 0;

int main(int argc, char** argv)
{
	//Lade die Bilder vom Benutzer
	loader = DataLoader();

	std::vector<cv::Mat> rawImages = loader.loadDataset();
	std::vector<cv::String> filePaths = loader.getFilePaths();

	if (rawImages.empty()) {
		return 0;
	}

	//Hole massse des Ausgabebildes vom Benuzer
	int width = 0;
	cout << "Please enter window width (between 300 and 1200): ";
	cin >> width;

	if (width >= 300 && width <= 1200) {
		int height = 0;
		cout << "Please enter window height (between 300 and 1200): ";
		cin >> height;
		if (height >= 300 && height <= 1200) {

			RESULT_WIDTH = width;
			RESULT_HEIGHT = height;
		}
		else {
			return 0;
		}
	}
	else {
		return 0;
	}

	cout << "************************************************************************************************** \n";
	cout << "VIS2: ImageHive: (Interactive) ContentAware Image Summarization \n";
	cout << "************************************************************************************************** \n";

	//init all images array
	allImages = std::vector<ImageAttribute>(rawImages.size());
	cout << "************************************************************************************************** \n";
	cout << "Analyzing Images are analyzed.. \n";
	doSimilarityCalculation(rawImages,filePaths);

	cout << "************************************************************************************************** \n";
	cout << "Calculating Globalclusters.. \n";
	doGlobalClustering();

	cout << "************************************************************************************************** \n";
	cout << "Evaluating Vornoi and Global Placement.. \n";
	createClustersAndGlobalVoronoi();

	cout << "************************************************************************************************** \n";
	cout << "Voronoi cells are filled.. \n";
	doLocalVoronoi();
	
	help();
	namedWindow("LocalVoronoiFilled", 0);
	setMouseCallback("LocalVoronoiFilled", onMouse, 0);

	resizeWindow("LocalVoronoiFilled", RESULT_WIDTH, RESULT_HEIGHT);

		for (;;)
		{
			imshow("LocalVoronoiFilled", filledCells);

			char c = (char)waitKey(0);
			if (c == 27)
			{
				cout << "Exiting ...\n";
				break;
			}

			switch (c)
			{
			case 'd':
				cout << "Debug Color\n";
				imshow("debug", clusterIndex);
				break;
			case 'c':
				if (mode != 1) {
					cout << "CVT mode is on\n";
					mode = 1;
				}
				break;
			case 'e':
				if (mode != 0) {
					cout << "Enlarge/Saliency mode is on\n";
					mode = 0;
				}
				break;
			case 'v':
					cout << "local voronoi cells shown on\n";
					imshow("Local Voronoi Tessellation", voronoicells);
					mode = 0;
				
				break;
			}
			

		}

		return 0;
	
}

//UI

Cluster* getTouchedCluster(int x, int y) {

	Vec3b color = clusterIndex.at<Vec3b>(y, x);
	int key = color.val[0] - 10;

	return colorMappingGlobal.at(key);
}

static void onMouse(int event, int x, int y, int, void*)
{
	//oeffne lokale cluster informationen je nachdem welches cluster angeklickt worden ist
	if (event != EVENT_LBUTTONDOWN)
		return;

	Cluster* currentClus = getTouchedCluster(x, y);

	if (currentClus != nullptr) {
		if (mode == 0) {
			imshow("LocalCluster", currentClus->localResult);
		}
		else if (mode == 1) {
			imshow("Local CVT Process", currentClus->localcvt);
		}
	}

}


void doSimilarityCalculation(std::vector<cv::Mat> rawImages, std::vector<cv::String> filePaths) {

	//color and edge histogram
	for (int i = 0; i < rawImages.size(); i++) {
		//allImages.push_back(ImageAttribute::ImageAttribute(images.at(i), i, filePaths.at(i)));	
		ImageAttribute tmp = ImageAttribute::ImageAttribute(rawImages.at(i), i, filePaths.at(i));
		allImages[i] = tmp;
		allImages.at(i).calcColorHistogram();
		allImages.at(i).calcHOG();
		allImages.at(i).calculateObjectness();
		allImages.at(i).calculateObjectness();
		allImages.at(i).calculateKeyPoints();
	}
}


void doGlobalClustering() {

	globalGraph = MyGraph();
	globalGraph.buildGraph(allImages);

	globalGraph.doClustering(allImages.size());
	//globalGraph.classesToString();
	globalGraph.compareSiftForNeighborhood();
	//globalGraph.IAclassesToString();

}

void createClustersAndGlobalVoronoi() {

	std::map<int, std::vector<MyEdge>> globalClasses = globalGraph.getClasses();

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


	bool even = (int)globalCusterMapping.size() % 2 == 0;

	std::vector<int> colRowLayout = gridLayout[globalCusterMapping.size()];
	int colCount = colRowLayout[0];
	int rowCount = colRowLayout[1];
	int lastColCount = colCount - ((colCount*rowCount) - globalCusterMapping.size());

	int cellHeight = RESULT_HEIGHT / rowCount;
	int cellWidth = RESULT_WIDTH / colCount;
	int lastCellWidth = RESULT_WIDTH / lastColCount;
	

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

			allClusters.push_back(createCluster(globalCusterMapping[imageIndex], cellHeight, currentCellWidth, position, pivot));

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
	for (int clus = 0;clus < allClusters.size();clus++) {
			cv::Point pivot = cv::Point(allClusters[clus]->pivot);
			allpositions.push_back(Point(pivot.x, pivot.y));
			pointMap.push_back(allClusters[clus]);
			i++;
	}

	string GlobalVoronoi = "GlobalVoronoi";

	// allocate space for voronoi diagram
	globalClusters = Mat::zeros(RESULT_HEIGHT, RESULT_WIDTH, CV_8UC3);
	
	calculateGlobalVoronoiEdges(allpositions, globalClusters.size().width, globalClusters.size().height, pointMap);
	
	for (int clus = 0;clus < allClusters.size();clus++) {
		Scalar col = Scalar(rand() / 255, rand() / 255, rand() / 255);
			for (int i = 0;i < allClusters[clus]->polygonEdges.size();i++) {
				cv::line(globalClusters,
					allClusters[clus]->polygonEdges.at(i).first,
					allClusters[clus]->polygonEdges.at(i).second,
					Scalar(255, 255, 255), 1, 8, 0);
			}

			cv::circle(globalClusters, allClusters[clus]->pivot, 2, Scalar(255, 255, 255));

			for (int Lclus = 0;Lclus < allClusters[clus]->allLocalClusters.size();Lclus++) {
				cv::circle(globalClusters, cv::Point(allClusters[clus]->allLocalClusters[Lclus]->globalPivot), 4, col,3);
				cv::circle(globalClusters, cv::Point(allClusters[clus]->allLocalClusters[Lclus]->position), 4, col, 8);
			}

	}

	globalClasses.clear();

}


Cluster* createCluster(std::vector<ImageAttribute> allImages, int height, int width, cv::Point position, cv::Point pivot) {

	Cluster* cluster = new Cluster(allImages,height, width, position, pivot);

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
	for (int i = 0; i < 30; ++i)
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
	}

	jcv_diagram_free(&voronoi);
	std::free(points);
	std::free(rect);

}

void doLocalVoronoi() {

	voronoicells = Mat(RESULT_HEIGHT, RESULT_WIDTH, CV_8UC3, Scalar(0, 0, 255));
		
	for (int clus = 0;clus < allClusters.size();clus++) {
		std::vector<cv::Point> allLocalPositions;

		//get all Local positions
		std::map<int, LocalCluster*> allLocalClusters;

		int i = 0;

		for (int Lclus = 0;Lclus < allClusters[clus]->allLocalClusters.size();Lclus++) {
			cv::Point globalPivot = cv::Point(allClusters[clus]->allLocalClusters[Lclus]->globalPivot);
			allLocalPositions.push_back(Point(globalPivot.x - allClusters[clus]->position.x, globalPivot.y - allClusters[clus]->position.y));
			allLocalClusters[i] = allClusters[clus]->allLocalClusters[Lclus];
			i++;
		}


		calculateLocalVoronoiEdges(allClusters[clus], allLocalPositions, allLocalClusters, allClusters[clus]->position);


	}

	Mat colorClusterBoundries = Mat(RESULT_HEIGHT, RESULT_WIDTH, CV_8UC3, Scalar(255, 255, 255));

	//draw voronoi edges of alllocalClusters
	for (int clus = 0;clus < allClusters.size();clus++) {
		Scalar col = Scalar(255, 255, 255);
		for (int Lclus = 0;Lclus < allClusters[clus]->allLocalClusters.size();Lclus++) {
			for (int i = 0;i < allClusters[clus]->allLocalClusters[Lclus]->globalPolygonEdges.size();i++) {
				cv::line(voronoicells,
					allClusters[clus]->allLocalClusters[Lclus]->globalPolygonEdges.at(i).first,
					allClusters[clus]->allLocalClusters[Lclus]->globalPolygonEdges.at(i).second,
					col, 1, 8, 0);

			}
		}

		cv::rectangle(colorClusterBoundries, cv::Rect(allClusters[clus]->position.x, allClusters[clus]->position.y, allClusters[clus]->width, allClusters[clus]->height), Scalar(0, 0, 0),3,8);
	}

		filledCells = Mat(RESULT_HEIGHT, RESULT_WIDTH, CV_8UC3, Scalar(0, 0, 0));
		clusterIndex = Mat(RESULT_HEIGHT, RESULT_WIDTH, CV_8UC3, Scalar(0, 0, 0));


		for (int clus = 0;clus < allClusters.size();clus++) {
			for (int Lclus = 0;Lclus < allClusters[clus]->allLocalClusters.size();Lclus++) {
				
				cv::Mat tmp = allClusters[clus]->allLocalClusters[Lclus]->getSaliencyCroppedImage(0);
				float ratio=allClusters[clus]->allLocalClusters[Lclus]->getLastScalingRatio();

				cellFloodFill(voronoicells, filledCells,tmp,
						allClusters[clus]->allLocalClusters[Lclus], ratio);
			
			}

			colorMappingGlobal.insert(std::pair<int, Cluster*>(clus, allClusters[clus]));

			int x = allClusters[clus]->position.x + (allClusters[clus]->width / 2);
			int y = allClusters[clus]->position.y + (allClusters[clus]->height / 2);

			floodFillColor(colorClusterBoundries, clusterIndex, clus, cv::Point(x, y));

		}
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
	
	Mat cvtResult = Mat(height, width, CV_8UC3, Scalar(0, 0, 0));

	jcv_diagram voronoi;

	memset(&voronoi, 0, sizeof(jcv_diagram));
	jcv_diagram_generate(all.size(), (const jcv_point*)points, rect, &voronoi);

	const jcv_site* sites = jcv_diagram_get_sites(&voronoi);
		for (int i = 0; i < voronoi.numsites; ++i)
		{
			const jcv_site* site = &sites[i];
	
			const jcv_graphedge* e = site->edges;
			while (e)
			{
				jcv_point p0 = recalcPoint(&e->pos[0], &voronoi.min, &voronoi.max, cvtResult.size().width, cvtResult.size().height);
				jcv_point p1 = recalcPoint(&e->pos[1], &voronoi.min, &voronoi.max, cvtResult.size().width, cvtResult.size().height);
	
				//Draw new Points/Lines
				cv::line(cvtResult, Point(p0.x, p0.y), Point(p1.x, p1.y), Scalar(255, 255, 255), 3, 8, 0);
	
	
				e = e->next;
			}
	
	}

	for (int i = 0; i < 30; ++i)
	{
		memset(&voronoi, 0, sizeof(jcv_diagram));
		jcv_diagram_generate(all.size(), (const jcv_point*)points, rect, &voronoi);

		relax_points(&voronoi, points);

		for (int i = 0; i < all.size(); ++i)
		{
			cv::circle(cvtResult, Point(points[i].x, points[i].y), 2, Scalar(255, 0, 0), 3, 8);
		}
	}

	Mat localCellsBoundries = Mat(height, width, CV_8UC3, Scalar(0, 0, 255));
	
	// If you want to draw triangles, or relax the diagram,
	// you can iterate over the sites and get all edges easily
	const jcv_site*sites2  = jcv_diagram_get_sites(&voronoi);
	for (int i = 0; i < voronoi.numsites; ++i)
	{
		const jcv_site* site = &sites2[i];
		
		const jcv_graphedge* e = site->edges;
		while (e)
		{
			jcv_point p0 = recalcPoint(&e->pos[0], &voronoi.min, &voronoi.max, clus);
			jcv_point p1 = recalcPoint(&e->pos[1], &voronoi.min, &voronoi.max, clus);

			cv::line(localCellsBoundries,
				Point(p0.x,p0.y),
				Point(p1.x, p1.y),
				Scalar(0,0,0), 1, 8, 0);
			
			cv::line(cvtResult, Point(p0.x, p0.y), Point(p1.x, p1.y), Scalar(255, 0, 0), 3, 8, 0);

			clusterMapping.at(i)->globalPolygonVertices.push_back(Point(p0.x + offsetPosition.x, p0.y + offsetPosition.y));
			clusterMapping.at(i)->globalPolygonEdges.push_back(std::pair<cv::Point, cv::Point>(Point(p0.x + offsetPosition.x, p0.y + offsetPosition.y), Point(p1.x + offsetPosition.x, p1.y + offsetPosition.y)));

			e = e->next;
		}

		
		clusterMapping.at(i)->globalPivot = cv::Point(points[i].x + offsetPosition.x, points[i].y + offsetPosition.y);
		clusterMapping.at(i)->calculateBoundingBox();
	
	}

	Mat localResult = Mat(height, width, CV_8UC3, Scalar(0, 0, 0));

	for (int Lclus = 0;Lclus < clusterMapping.size();Lclus++) {
			cv::Mat tmp = clusterMapping.at(Lclus)->getSaliencyCroppedImage(1);
			float ratio = clusterMapping.at(Lclus)->getLastScalingRatio();

			localFloodFill(localCellsBoundries, localResult, tmp,
			clusterMapping.at(Lclus), ratio, offsetPosition);

	}

	clus->localResult = localResult;
	clus->localcvt = cvtResult;
	
	jcv_diagram_free(&voronoi);
	std::free(points);
	std::free(rect);


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

void floodFillColor(Mat boundries, Mat out, int colorIndex, cv::Point seed) {

	int g = (rand()+3) / 255 -2;
	int b = (rand()+3) / 255 -2;

	std::stack<std::pair<int, int>> stack;
	stack.push(std::pair<int, int>(seed.x,seed.y));

	int counter = 0;
	int newPointX = 0;
	int newPointY = 0;

	while (!stack.empty() && counter<2000000) {
		newPointX = stack.top().first;
		newPointY = stack.top().second;

		stack.pop();
		counter++;
		if (newPointX > 1 && newPointY > 1 &&
			newPointY < (RESULT_HEIGHT - 1) && newPointX < (RESULT_WIDTH - 1))
		{
			if (out.at<Vec3b>(newPointY, newPointX) == Vec3b(0, 0, 0) &&
				boundries.at<Vec3b>(newPointY, newPointX) == Vec3b(255,255, 255)) {

				out.at<Vec3b>(newPointY, newPointX) = Vec3b(colorIndex + 10, g, b);
				
				stack.push(std::pair<int,int>(newPointX + 1, newPointY));
				stack.push(std::pair<int, int>(newPointX - 1, newPointY));
				stack.push(std::pair<int, int>(newPointX, newPointY - 1));
				stack.push(std::pair<int, int>(newPointX, newPointY + 1));
			}
		}
	}

	while (!stack.empty()) {
		stack.pop();
	}
}

void localFloodFill(Mat boundries, Mat out, Mat colorSrc, LocalCluster* Lclus, float scaleRatio, cv::Point offsetPosition) {

	cv::Mat innerSal = Lclus->image.getCroppedImage2();
	std::vector<int> innerSaliencyVertices = Lclus->image.getCropped2Coords();

	int pivotX = ((innerSal.size().width*scaleRatio) / 2) + (innerSaliencyVertices[0] * scaleRatio);
	int pivotY = ((innerSal.size().height*scaleRatio) / 2) + (innerSaliencyVertices[1] * scaleRatio);

	std::stack<std::pair<cv::Point, cv::Point>> stack;
	stack.push(std::pair<cv::Point, cv::Point>(Point(Lclus->globalPivot.x- offsetPosition.x, Lclus->globalPivot.y- offsetPosition.y), Point(pivotX, pivotY)));

	int counter = 0;
	int newPointX = 0;
	int newPointy = 0;
	int newsrcX = 0;
	int newsrcY = 0;

	while (!stack.empty()&& counter <3000000) {
		newPointX = stack.top().first.x;
		newPointy = stack.top().first.y;
		newsrcX = stack.top().second.x;
		newsrcY = stack.top().second.y;

		stack.pop();
		counter++;
		if (newPointX > 1 && newPointy > 1 &&
			newPointy < (out.size().height - 1) && newPointX < (out.size().width - 1)) {
			//out of bounds  whole image

			if (newsrcX > 1 && newsrcY > 1 &&
				newsrcY < (colorSrc.size().height - 1) && newsrcX < (colorSrc.size().width - 1))
			{
				//out of bounds  src image

				if (out.at<Vec3b>(newPointy, newPointX) == Vec3b(0, 0, 0) &&
					boundries.at<Vec3b>(newPointy, newPointX) == Vec3b(0, 0, 255)) {

					out.at<Vec3b>(newPointy, newPointX) = colorSrc.at<Vec3b>(newsrcY, newsrcX);
					stack.push(std::pair<cv::Point, cv::Point>(Point(newPointX + 1, newPointy), Point(newsrcX + 1, newsrcY)));
					stack.push(std::pair<cv::Point, cv::Point>(Point(newPointX - 1, newPointy), Point(newsrcX - 1, newsrcY)));
					stack.push(std::pair<cv::Point, cv::Point>(Point(newPointX, newPointy - 1), Point(newsrcX, newsrcY - 1)));
					stack.push(std::pair<cv::Point, cv::Point>(Point(newPointX, newPointy + 1), Point(newsrcX, newsrcY + 1)));
				}
			}
		}
	}

	while (!stack.empty()) {
		stack.pop();
	}
}

void cellFloodFill(Mat boundries, Mat out, Mat colorSrc,LocalCluster* clus, float scaleRatio) {

	cv::Mat innerSal = clus->image.getCroppedImage2();
	std::vector<int> innerSaliencyVertices = clus->image.getCropped2Coords();

	int pivotX = ((innerSal.size().width*scaleRatio) / 2) + (innerSaliencyVertices[0]* scaleRatio);
	int pivotY = ((innerSal.size().height*scaleRatio) / 2) + (innerSaliencyVertices[1]* scaleRatio);

	std::stack<std::pair<cv::Point, cv::Point>> stack;
	stack.push(std::pair<cv::Point, cv::Point>(Point(clus->globalPivot.x, clus->globalPivot.y), Point(pivotX, pivotY)));
	
	int counter = 0;
	int newPointX = 0;
	int newPointy = 0;
	int newsrcX = 0;
	int newsrcY = 0;

	while (!stack.empty()&& counter<2000000) {
		newPointX = stack.top().first.x;
		newPointy = stack.top().first.y;
		newsrcX = stack.top().second.x;
		newsrcY = stack.top().second.y;

		stack.pop();
		counter++;
		if (newPointX > 1 && newPointy > 1 &&
			newPointy < (RESULT_HEIGHT - 1) && newPointX < (RESULT_WIDTH - 1) &&
			newsrcX > 1 && newsrcY > 1 &&
			newsrcY < (colorSrc.size().height- 1) && newsrcX < (colorSrc.size().width - 1))
			 {

			if (out.at<Vec3b>(newPointy, newPointX) == Vec3b(0, 0, 0) &&
				boundries.at<Vec3b>(newPointy, newPointX) == Vec3b(0, 0, 255)) {

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

	while (!stack.empty()) {
		stack.pop();
	}
}


void cleanUp() {

	allImages.clear();
	allImages = std::vector<ImageAttribute>();
	allClusters.clear();
	allClusters = std::vector<Cluster*>();

	colorMappingGlobal.clear();
	colorMappingGlobal = std::map<int, Cluster*>();
	mode = 0;
}

static void help()
{
	cout << "************************************************************************************************** \n";
	cout << "\nImageHive: Interactive Content-Aware Image Summarization\n"
		"Visualisierung 2 -- TU WIEN -- Gruppe Tan2011\nRebeka Koszticsák - 1325492 **** Bettina Schlager - 1327281\n" << endl;
	cout << "************************************************************************************************** \n";

	cout << "Commands: \n"
		"\tESC - quit the program\n"
		"\tc - switch to CVT Mode\n"
		"\te - switch to enlarge Mode\n" 
		"\tv - see local voronoi tessellation cells\n" << endl;
}
