#pragma once
#include "ImageAttributes.h"
#include "MyEdge.h"
#include <map>

/**
*Stellt ein Graphstruktur der Bilder dar, wo die Kanten aehnlichkeit symbolisieren
*/
class MyGraph
{
public:

	/**
	*Struct fuer ein Bild und sein naechsten Nachbarn laut SIFT
	*/
	struct SiftImg {
		/**
		*Das Bild
		*/
		ImageAttribute img;

		/**
		*Das aehnlichste Bild zu dem laut SIFT
		*/
		ImageAttribute neighborImg;
	};
	

	MyGraph() {};
	~MyGraph() {};

	/**
	*Erzeigt einen vollstaendigen Graph allen uebergebenen Bilder, wobei alle Bilder miteinander verglichen werden
	@param all alle bilder welche im Graphen sein sollen
	*/
	void buildGraph(std::vector<ImageAttribute> all);

	/**
	*erzeugt eine Kante mit den uebergebenen Start und Endbilder und Gewichte
	@param start das Bild des Startknotens
	@param end  das Bild des Endknotens
	@param weight  das Bild des das Gewicht der Kante zwischen den zwei Knoten
	*/
	void createEdge(ImageAttribute start, ImageAttribute end, float weight);

	/**
	*Klassifiziert die Bilder in dem Graph
	*/
	int doClustering(int amountVertices);

	/**
	*ueberprueft mit welchen anderen Kanten die Kanten in der Menge der Kanten mit kelinsten Gewichten benachbart sind
	*/
	void checkNeighborhood();

	/**
	*Gibt die Bildklassen zurueck
	*/
	std::map<int, std::vector<MyEdge>> getClasses() { return classes; }

	/**
	*Gibt die Bildklassen in den Bildklassen zurueck
	*/
	std::map<int, std::vector<SiftImg>> getLocalClasses() { return classesImg; }

	/**
	*Schreibt die Klassen aus
	*/
	void classesToString();

	/**
	*Schreibt die Klassen und die Beachbarte Bilder zurueck
	*/
	void IAclassesToString();

	/**
	*Vergleicht die SIFT Werte und sucht nach naehesten Nachbarn
	*/
	void compareSiftForNeighborhood();


	
private:
	
	std::vector<MyEdge> edges;							///<die Kanten in dem Graphen
	std::vector<ImageAttribute> visitedVertices;		///<Hilfvariable fuer Klassensuche

	std::vector<MyEdge> chosenEdges;					///<Kanten mit kleinsten Gewichten
	int amountClasses = 0;								///<Anzahl der Bildklassen
	std::map<int, std::vector<MyEdge>> classes;			///<Die Bildklassen durch die Kanten dargestellt
	std::map<int, std::vector<SiftImg>> classesImg;		///<Die Bildklassen durch die Bilder und ihr Nachbarnbilder dargestellt

	/**
	*Definiert die Bilder in den Klassen
	*/
	void checkConnection(std::vector<MyEdge> connectedEdges, std::vector<MyEdge> neighboringEdges);
	
	/**
	*uebersetzt die kantendefinierten Klassen auf bilddefinierten 
	*/
	void writeIAclasses();

	/**
	*Gibt die ueberlappende SIFT Feature Punkte zurueck
	*/
	int getSiftMatches(ImageAttribute image, ImageAttribute compareWith);

};

