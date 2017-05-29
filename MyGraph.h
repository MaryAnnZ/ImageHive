#pragma once
#include "ImageAttributes.h"
#include "MyEdge.h"
#include <map>

class MyGraph
{
public:
	/**
	*Struct f�r ein Bild und sein n�chsten Nachbarn laut SIFT
	*/
	struct SiftImg {
		/**
		*Das Bild
		*/
		ImageAttribute img;
		/**
		*Das �hnlichste Bild zu dem laut SIFT
		*/
		ImageAttribute neighborImg;
	};
	/**
	*Stellt ein Graphstruktur der Bilder dar, wo die Kanten �hnlichkeit symbolisieren
	*/
	MyGraph() {};
	~MyGraph() {};
	/**
	*Erzeigt einen vollst�ndigen Graph allen �bergebenen Bilder, wobei alle Bilder miteinander verglichen werden
	*/
	void buildGraph(std::vector<ImageAttribute> all);
	/**
	*erzeugt eine Kante mit den �bergebenen Start und Endbilder und Gewichte
	*/
	void createEdge(ImageAttribute start, ImageAttribute end, float weight);
	/**
	*Klassifiziert die Bilder in dem Graph
	*/
	int doClustering(int amountVertices);
	/**
	*�berpr�ft mit welchen anderen Kanten die Kanten in der Menge der Kanten mit kelinsten Gewichten benachbart sind
	*/
	void checkNeighborhood();
	/**
	*Gibt die Bildklassen zur�ck
	*/
	std::map<int, std::vector<MyEdge>> getClasses() { return classes; }
	/**
	*Gibt die Bildklassen in den Bildklassen zur�ck
	*/
	std::map<int, std::vector<SiftImg>> getLocalClasses() { return classesImg; }
	/**
	*Schreibt die Klassen aus
	*/
	void classesToString();
	/**
	*Schreibt die Klassen und die Beachbarte Bilder zur�ck
	*/
	void IAclassesToString();
	/**
	*Vergleicht die SIFT Werte und sucht nach n�hesten Nachbarn
	*/
	void compareSiftForNeighborhood();

	
private:
	/**
	*die Kanten in dem Graphen
	*/
	std::vector<MyEdge> edges;
	/**
	*Hilfvariable f�r Klassensuche
	*/
	std::vector<ImageAttribute> visitedVertices;
	/**
	*Kanten mit kleinsten Gewichten
	*/
	std::vector<MyEdge> chosenEdges;
	/**
	*Definiert die Bilder in den Klassen
	*/
	void checkConnection(std::vector<MyEdge> connectedEdges, std::vector<MyEdge> neighboringEdges);
	/**
	*Anzahl der Bildklassen
	*/
	int amountClasses = 0;
	/**
	*Die Bildklassen durch die Kanten dargestellt
	*/
	std::map<int, std::vector<MyEdge>> classes;
	/**
	*Die Bildklassen durch die Bilder und ihr Nachbarnbilder dargestellt
	*/
	std::map<int, std::vector<SiftImg>> classesImg;
	/**
	*�bersetzt die kantendefinierten Klassen auf bilddefinierten 
	*/
	void writeIAclasses();
	/**
	*Gibt die �berlappende SIFT Feature Punkte zur�ck
	*/
	int getSiftMatches(ImageAttribute image, ImageAttribute compareWith);

};

