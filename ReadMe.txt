Allgemein

Die Applikation ist in C++ unter Verwendung der OpenCV 3.1 library geschrieben. OpenCV stellt s�mtliche Methoden die wir beim nachimplementieren ben�tigen zur Verf�gung, wie die Berechnung der Histogramme und Evaluierung.Unter anderen wie f�r SIFT und Saliency Berechnung werden wir auch die contrib Libraries von OpenCV benutzen. Die Entwicklungsumgebung ist Visual Studio 2015.
Datensatz

Bez�glich Inputdaten haben wir uns entschieden nat�rliche Bilder zu verwenden, da sich diese besser f�r die �hnlichkeitsberechnung eignen. Zurzeit unterst�tzt unsere Implementierung ein maximum von 20 Inputbildern.
Implementierung

Nach dem Importieren des Datensatzes werden Farb- und Kantenhistogramme erstellt und verglichen. Anhand der Evaluierung und die SIFT Feature Punkte werden die Daten zu Klassen zugeordnet, wobei der Korrelationswert zwischen je zwei Klassen je kleiner, zwischen den Bilder der einzelnen Klassen je gr��er sein soll. Um die Klassenbereiche auf dem Ergebnisbild zu bestimmen, wird Voronoi Tesselation ausgef�hrt, wobei die die Anzahl der Zellen die Anzahl der Klassen entspricht, und die Punkte so weit wie m�glich vonenander platziert werden. Um die wichtigsten Bereiche in einem Bild detektieren zu k�nnen, werden entsprechende saliency Werte pro Pixel berechnet und entsprechend des BING Objectness Algorithmus werden innere und au�ere saliente Bereiche auf den ildern definiert.

Nach der �hnlichkeitsberechnung und dem Clustering werden die globalen Cluster grob im 2D Raum angeordnet, und danach werden die Clustergrenzen mittels Voronoi Tessellation ermittelt. Diese globalen Grenzen helfen beim Lokalen Clustering und Verschiebung der Lokalen Mittelpunkte damit die Punkte gut verteilt sind. Nach der lokalen Platzierung werden mittels FloodFill die einzelnen Clusterzellen mit deren Bildern gef�llt. Der Seedpunkt des Bildes ist der Mittelpunkt des inneren lokalen Saliency Bereich, welcher die Mitte des wichtigsten Bereiches im Bild darstellen soll. Das Bild nach dem �u�eren Saliency Bereich skaliert um sicher zustellen dass der innere Bereich sichtbar ist.
Interaktion

Nach dem Start kann der Benutzer die gew�nschte Fenstergr��e eingeben. Danach wird ein Ordner ausgew�hlt welcher die Bilder beinhaltet welche sortiert werden sollen. Nach einer kurzen Wartezeit sieht man auch schon das Ergebnis. Es gibt nun zwei Modi, beim ersten kann man beim Klick auf ein globales Cluster das kleine Cluster nochmal gr��er betrachten mit eingezeichneten Saliency Bereich. Beim zweiten Modus sieht man beim Klick auf ein globales Cluster die Centroidal Voronoi Tessellation welche durchgef�hrt wurde.

***********************************************************
Start the program by starting "ImageHive.exe"

In the "data" folder you can find several test data.
With some datasets the algorithm works better than others concerning clustering to 
similarity. data images as in the directory "unnatural" does not work very well with
similarity calculation than eg. natural datasets as "vacation"


If you want to try it with your own dataset keep in mind:
* .png are prefered 
* concerning calculation time: use small input images (no more than 250KB per image)
* right now more than 25 is not supported - at your own risk! :)


After start you can take a look at local clusters by clicking on them, and also 
can see saliency regions with "c" and click on the cluster.

Keep in mind to NOT close the big image, since it is not working probably after!

Have fun