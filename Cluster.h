#include "Utils.h"
#include "LocalCluster.h"
#include "MyGraph.h"
#include <iostream>

/**
* Repraesentiert ein Cluster Objekt
*/
class Cluster
{
public:

	/**
	* Ertelle ein Cluster mit uebergebenen Parametern
	@param all reference zu allen bildern diesen clusters
	@param height hoehe des clusters
	@param width breite des clusters
	@param position links unten position des clusters relativ zum gesamtbild
	@param pivot mittelpunkt des clusters relativ zum gesamtbild
	*/
	Cluster(std::vector<ImageAttribute> all, int height, int width, cv::Point position, cv::Point pivot);
	~Cluster() {};
	Cluster() {};
	
	/**
	* Erstelle und fuege ein locales Cluster diesem cluster hinzu. 
	@param image reference zu bild des lokalen clusters
	@param globalPivot mittelpunkt des lokalen clusters relativ zum gesamtbild
	@param height hoehe des lokalen clusters
	@param width breite des lokalen clusters
	@param position links unten positiont des lokalen clusters relativ zum gesamtbild
	*/
	void addLocalCluster(ImageAttribute image, cv::Point globalPivot, int height, int width,cv::Point position);
	
	
	std::vector<LocalCluster*> allLocalClusters;				///< speichert reference zu allen lokalen clustern
	
	std::vector<cv::Point> polygonVertices;						///< knotenpunkte der kanten des clusters
	std::vector<std::pair<cv::Point, cv::Point>> polygonEdges;	///< kanten des clusters

	cv::Point pivot;											///< mittelpunkt des clusters relativ zum gesamtbild
	cv::Point position;											///< position links unten des clusters relativ zum gesamtbild

	int height;													
	int width;

	std::vector<ImageAttribute> allImages;						///< speichert reference zu allen Images welche zu diesem cluster gehoeren

	cv::Mat localResult;										///< referenz zu lokales bild der cluster
	cv::Mat localcvt;											///< referenz zu cvt des clusters
	
};





