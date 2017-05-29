#pragma once
#include "ImageAttributes.h"

class MyEdge
{
public:
	/**
	*Stellt eine Ähnlichkeitskante zwischen zwei Bilder dar
	*/
	MyEdge(ImageAttribute iaStart, ImageAttribute isEnd, float edgeWeight);
	~MyEdge();
	/**
	*Gibt das Startbild zurück
	*/
	ImageAttribute getStartImage() { return imageStart; }
	/**
	*Gibt das Endbild zurück
	*/
	ImageAttribute getEndImage() { return imageEnd; }
	/**
	*Gibt das Gewicht (Ähnlichkeit) der Kante zurück
	*/
	float getWeight() { return weight; }
	/**
	*Vergleicht zwei Kanten ob sie die selbe sind
	*/
	bool compareTo(MyEdge otherEdge);
	/**
	*Gibt zurück ob zwei Kanten benachbart sind
	*/
	bool connectedWith(MyEdge otherEdge);
	/**
	*Definiert eine Nachbarkante
	*/
	void addNedigbor(MyEdge edge);
	/**
	*Gibt alle Nachbarnkanten zurück
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
	*Ähnlichkeitsgewicht
	*/
	float weight;
	/**
	*Nackbarnkanten
	*/
	std::vector<MyEdge> neighbors;
};

