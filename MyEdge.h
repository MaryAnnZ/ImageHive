#pragma once
#include "ImageAttributes.h"

class MyEdge
{
public:
	/**
	*Stellt eine �hnlichkeitskante zwischen zwei Bilder dar
	*/
	MyEdge(ImageAttribute iaStart, ImageAttribute isEnd, float edgeWeight);
	~MyEdge();
	/**
	*Gibt das Startbild zur�ck
	*/
	ImageAttribute getStartImage() { return imageStart; }
	/**
	*Gibt das Endbild zur�ck
	*/
	ImageAttribute getEndImage() { return imageEnd; }
	/**
	*Gibt das Gewicht (�hnlichkeit) der Kante zur�ck
	*/
	float getWeight() { return weight; }
	/**
	*Vergleicht zwei Kanten ob sie die selbe sind
	*/
	bool compareTo(MyEdge otherEdge);
	/**
	*Gibt zur�ck ob zwei Kanten benachbart sind
	*/
	bool connectedWith(MyEdge otherEdge);
	/**
	*Definiert eine Nachbarkante
	*/
	void addNedigbor(MyEdge edge);
	/**
	*Gibt alle Nachbarnkanten zur�ck
	*/
	std::vector<MyEdge> getNeighbors() { return neighbors; }

private:
	/**
	*Startbild
	*/
	ImageAttribute imageStart;
	/**
	*Endbild
	*/
	ImageAttribute imageEnd;
	/**
	*�hnlichkeitsgewicht
	*/
	float weight;
	/**
	*Nackbarnkanten
	*/
	std::vector<MyEdge> neighbors;
};

