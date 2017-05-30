#pragma once
#include "ImageAttributes.h"

class MyEdge
{
public:
	/**
	*Stellt eine aehnlichkeitskante zwischen zwei Bilder dar
	@param iaStart das start Knotenbild
	@param isEnd das edn Knotenbild
	*/
	MyEdge(ImageAttribute iaStart, ImageAttribute isEnd, float edgeWeight);
	~MyEdge();

	/**
	*Gibt das Startbild zurueck
	*/
	ImageAttribute getStartImage() { return imageStart; }

	/**
	*Gibt das Endbild zurueck
	*/
	ImageAttribute getEndImage() { return imageEnd; }

	/**
	*Gibt das Gewicht (aehnlichkeit) der Kante zurueck
	*/
	float getWeight() { return weight; }

	/**
	*Vergleicht zwei Kanten ob sie die selbe sind
	@return true wenn die zwei Kanten die selben sind
	*/
	bool compareTo(MyEdge otherEdge);

	/**
	*Gibt zurueck ob zwei Kanten benachbart sind
	@return true wenn die zwei kanten benachbart sind, sonst falsch
	*/
	bool connectedWith(MyEdge otherEdge);

	/**
	*Definiert eine Nachbarkante
	*/
	void addNedigbor(MyEdge edge);

	/**
	*Gibt alle Nachbarnkanten zurueck
	*/
	std::vector<MyEdge> getNeighbors() { return neighbors; }

private:
	
	ImageAttribute imageStart;				///<Das Startbild
	ImageAttribute imageEnd;				///<Das Endbild
	float weight;							///<aehnlichkeitsgewicht
	std::vector<MyEdge> neighbors;			///<Nackbarnkanten
};

