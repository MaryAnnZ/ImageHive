#pragma once
#include "Utils.h"

class ImageAttribute
{
public:
	/**
	*Ein ImageAttribute stellt ein Bild dar und speichert seine relevante Attribute
	*/
	::ImageAttribute(cv::Mat img, int idVal, std::string path);
	ImageAttribute();
	~ImageAttribute();

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
	*Gibt das Originalbild zurück
	*/
	cv::Mat getImage() { return image; }
	/**
	*Gibt die HOG Werte zurück
	*/
	std::vector<float> getHOGvalues() {return valuesHOG;}
	/**
	*Ändert die Größe des Bildes
	*/
	cv::Mat resize(cv::Size newSize);
	/**
	*Ändert die Größe des übergebenen Bildes
	*/
	cv::Mat resize(cv::Mat, cv::Size newsize);
	/**
	*Gibt die Größe des Originalbildes zurück
	*/
	cv::Size getOriginSize() { return image.size(); };
	/**
	*Gibt das Farbhistogram zurück
	*/
	std::vector<cv::Mat> getColorHis() { return bgrHist; }
	/**
	*Vergleicht das Farbhistogram des Bildes mit dem übergebenen Histogram
	*/
	float compareHist(std::vector<cv::Mat> otherHistograms);
	/**
	*Gibt das ID des Objektes zurück
	*ist nicht unbedingt eindeutig, ist von der Initialisierung abhängig
	*/
	int getId() { return id; }
	/**
	*Vergleicht die Bilder anhand ihren IDs
	*/
	bool compareImage(ImageAttribute image);
	/**
	*Gibt das Pfad zu dem Bild File zurück
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
	*Gibt die SIFT Feature Punkte zurück
	*/
	std::vector<cv::KeyPoint> getKeyPoints() { return keypoints; }
	/**
	*Gibt den SIFT Descriptor zurück
	*/
	cv::Mat getDescriptor() { return descriptor; }
	/**
	*Definiert den < Operator anhand die Größe der Bilder
	*/
	bool operator<(ImageAttribute other) const { return image.size < other.image.size; }
	/**
	*Gibt den außeren salien Bereich zurück
	*/
	cv::Mat getCropped() {return croppedImage;};
	/**
	*Gibt den inneren salient Bereich zurück
	*/
	cv::Mat getCroppedImage2() { return croppedImage2; }
	/**
	*Gibt die Koordinaten des außeren salienten Bereich zurück
	*/
	std::vector<int> getCropped1Coords() { return cropped1Coords; }
	/**
	*Gibt die Koordinaten des inneren salienten Bereich zurück
	*/
	std::vector<int> getCropped2Coords() { return cropped2Coords; }

private:
	/**
	*Das Originalbild
	*/
	cv::Mat image;
	/**
	*Id des Bildes, auf uniqueness soll bei Initialisierung geachtet werden
	*/
	int id;
	/**
	*außeres salientes Bereich
	*/
	cv::Mat croppedImage;
	/**
	*inneres salientes Bereich
	*/
	cv::Mat croppedImage2;
	/**
	*die Farbhistogram
	*/
	std::vector<cv::Mat> bgrHist;
	/**
	*die HOG
	*/
	std::vector<float> valuesHOG;
	/**
	*Hilfsvariable zur HOG-Berechnung
	*/
	std::vector<cv::Point> locationsHOG;
	/**
	*Visualisiert die Farbhistogram
	*/
	void outputHistogram();
	/**
	*Pfad zum Bild File
	*/
	std::string filePath;
	/**
	*die Objectness Bereiche
	*/
	std::vector<cv::Vec4i> objectnessBoundingBox;
	/**
	*die Objectness-Wahrscheinlichkeiten der Objectnessbereichen
	*/
	std::vector<float> objectnessValue;
	/**
	*berechnet die Varianz von den übergebenen Werten
	*/
	int getVariance(std::vector<int> values);
	/**
	*berechnet den Durchschnitt von den übergebenen Werten
	*/
	int getMean(std::vector<int> values);
	/**
	*Die SIFT Feature Points
	*/
	std::vector<cv::KeyPoint> keypoints;
	/**
	*SIFT descriptor
	*/
	cv::Mat descriptor;
	/**
	*die Koordinaten des außeren salienten Bereiches
	*lowerLeftX; lowerLeftY; upperRightX; upperRightY
	*/
	std::vector<int> cropped1Coords;
	/**
	*die Koordinaten des inneren salienten Bereiches
	*lowerLeftX; lowerLeftY; upperRightX; upperRightY
	*/
	std::vector<int> cropped2Coords;

};

