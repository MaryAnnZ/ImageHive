#include "Utils.h"
#include "ImageAttributes.h"


/**
* Repraesentiert ein Lokales cluster objekt
*/
class LocalCluster
{
public:
	
	LocalCluster() {};
	~LocalCluster() {};

	/**
	* Ertelle ein Cluster mit uebergebenen Parametern
	@param img reference zu bild des lokalen clusters
	@param globalPivot mittelpunkt des lokalen clusters relativ zum gesamtbild
	@param height hoehe des lokalen clusters
	@param width breite des lokalen clusters
	@param position links unten positiont des lokalen clusters relativ zum gesamtbild
	*/
	LocalCluster(ImageAttribute img, cv::Point globalPivot, int height, int width, cv::Point position);
		
	std::vector<cv::Point> globalPolygonVertices;						///< knotenpunkte der kanten des clusters
	std::vector<std::pair<cv::Point, cv::Point>> globalPolygonEdges;	///< kanten des clusters	

	cv::Point globalPivot;												///< mittelpunkt des clusters relativ zum gesamtbild
	cv::Point position;													///< position links unten des clusters relativ zum gesamtbild
		
	cv::Mat getImage() { return image.getImage(); };					

	/**
	* Referenz zum Bild des Clusters welche skaliert wurde damit die Saliency Bereiche fuer die gegebene Clustergroesse ausreicht
	@param mode 1 = mehr saliency sichtbar, 0 = weniger saliency bereiche sichtbar
	@return das skalierte bild
	*/
	cv::Mat getSaliencyCroppedImage(int mode);	

	/**
	* Referenz zum Bild des Clusters welche skaliert wurde damit die Saliency Bereiche fuer die gegebene Clustergroesse ausreicht
	@param mode 1 = mehr saliency sichtbar, 0 = weniger saliency bereiche sichtbar
	@return die passende skalierung des saliency bereichs
	*/
	float getLastScalingRatio() { return lastScalingRatio; };
		
	int cellHeight;									///< hoehe der voronoi zelle
	int cellWidth;									///< breite der voronoi zelle
		
	void calculateBoundingBox();					///< berechnet die bounding box des clusters, fuer die bildskalierung
	std::vector<cv::Point> boundingVertices;		///< Knotenpunkte der boundingbox

	ImageAttribute image;							///< Refernz zum Bild des Clusters			

private:
	
	float lastScalingRatio;							///< Skalierung des BIldes anhand der Groesse des Clusters			

	int boundingHeight;								///< hoehe der bounding box
	int boundingWidth;								///< breite der bounding box		
};



