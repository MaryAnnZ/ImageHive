#pragma once
#include "Utils.h"

class ImageAttribute
{
public:
	/**
	*Ein ImageAttribute stellt ein Bild dar und speichert seine relevante Attribute
	@param img das Bild als Matrix
	@idVal einzigartige ID
	@path DateiPfad
	*/
	::ImageAttribute(cv::Mat img, int idVal, std::string path);
	ImageAttribute() {};
	~ImageAttribute() {};

	/**
	*Berechnet die Farbhistogram
	*/
	void calcColorHistogram();

	/**
	*Berechnet die Histogram of Oriented Gradients
	*/
	void calcHOG();

	/**
	*Vergleicht die HOG-Histograme
	*/
	float compareHOGvalue(std::vector<float> otherValues);

	/**
	*Gibt das Originalbild zurueck
	*/
	cv::Mat getImage() { return image; }

	/**
	*Gibt die HOG Werte zurueck
	*/
	std::vector<float> getHOGvalues() {return valuesHOG;}

	/**
	*Gibt die Groesse des Originalbildes zurueck
	*/
	cv::Size getOriginSize() { return image.size(); };

	/**
	*Gibt das Farbhistogram zurueck
	@return Histogram mit allen drei Kanaelen
	*/
	std::vector<cv::Mat> getColorHis() { return bgrHist; }

	/**
	*Vergleicht das Farbhistogram des Bildes mit dem uebergebenen Histogram
	@otherHistograms vergleiche histogram mit diesem histogram
	@return einen Wert zwischen 1 und 0, 0 = gleiches Histogram
	*/
	float compareHist(std::vector<cv::Mat> otherHistograms);

	/**
	*Gibt das ID des Objektes zurueck
	*ist nicht unbedingt eindeutig, ist von der Initialisierung abhaengig
	*/
	int getId() { return id; }

	/**
	*Vergleicht die Bilder anhand ihren IDs
	@image mit diesem Bild soll das Bild verglichen werden
	@return true wenn das gleich ist, sonst false
	*/
	bool compareImage(ImageAttribute image);

	/**
	*Gibt das Pfad zu dem Bild File zurueck
	*/
	std::string getPath() { return filePath; }

	/**
	*Berechnet BING Objectness und schnedet die Bereiche mit kleinem Objectness Wert ab
	*/
	void calculateObjectness();

	/**
	*Berechnet die SIFT Feature Punkte
	*/
	void calculateKeyPoints();

	/**
	*Gibt die SIFT Feature Punkte zurueck
	*/
	std::vector<cv::KeyPoint> getKeyPoints() { return keypoints; }

	/**
	*Gibt den SIFT Descriptor zurueck
	*/
	cv::Mat getDescriptor() { return descriptor; }

	/**
	*Definiert den < Operator anhand die Groesse der Bilder
	*/
	bool operator<(ImageAttribute other) const { return image.size < other.image.size; }

	/**
	*Gibt den ausseren salien Bereich zurueck
	*/
	cv::Mat getCropped() {return croppedImage;};

	/**
	*Gibt den inneren salient Bereich zurueck
	*/
	cv::Mat getCroppedImage2() { return croppedImage2; }

	/**
	*Gibt die Koordinaten des ausseren salienten Bereich zurueck
	*/
	std::vector<int> getCropped1Coords() { return cropped1Coords; }

	/**
	*Gibt die Koordinaten des inneren salienten Bereich zurueck
	*/
	std::vector<int> getCropped2Coords() { return cropped2Coords; }

	/*
	*Gibt das originalbild indem die berechneten saliency regionen markiert sind zurueck
	*/
	cv::Mat getSaliencyMarkedOriginal() { return saliencyMarkedImage; }

private:

	cv::Mat image;											///<Das Originalbild
	cv::Mat saliencyMarkedImage;							///<Das originalbild indem die berechneten saliency regionen markiert sind
	int id;													///<Id des Bildes, auf uniqueness soll bei Initialisierung geachtet werden

	cv::Mat croppedImage;									///<ausseres salientes Bereich
	cv::Mat croppedImage2;									///<inneres salientes Bereich

	std::vector<cv::Mat> bgrHist;							///<die Farbhistogram

	std::vector<float> valuesHOG;							///<die HOG
	std::vector<cv::Point> locationsHOG;					///<Hilfsvariable zur HOG-Berechnung

	std::string filePath;									///<Pfad zum Bild File

	/**
	*Visualisiert die Farbhistogram
	*/
	void outputHistogram();			


	std::vector<cv::Vec4i> objectnessBoundingBox;			///<die Objectness Bereiche
	std::vector<float> objectnessValue;						///<die Objectness-Wahrscheinlichkeiten der Objectnessbereichen

	/**
	*berechnet die Varianz von den uebergebenen Werten
	*/
	int getVariance(std::vector<int> values);

	
	int getMean(std::vector<int> values);					///<berechnet den Durchschnitt von den uebergebenen Werten
	std::vector<cv::KeyPoint> keypoints;					///<Die SIFT Feature Points
	cv::Mat descriptor;										///<SIFT descriptor

	std::vector<int> cropped1Coords;						///<die Koordinaten des ausseren salienten Bereiches, lowerLeftX; lowerLeftY; upperRightX; upperRightY
	std::vector<int> cropped2Coords;						///<die Koordinaten des inneren salienten Bereiches, lowerLeftX; lowerLeftY; upperRightX; upperRightY

};

