Allgemein

Die Applikation ist in C++ unter Verwendung der OpenCV 3.1 library geschrieben. OpenCV stellt sämtliche Methoden die wir beim nachimplementieren benötigen zur Verfügung, wie die Berechnung der Histogramme und Evaluierung.Unter anderen wie für SIFT und Saliency Berechnung werden wir auch die contrib Libraries von OpenCV benutzen. Die Entwicklungsumgebung ist Visual Studio 2015.
Datensatz

Bezüglich Inputdaten haben wir uns entschieden natürliche Bilder zu verwenden, da sich diese besser für die Ähnlichkeitsberechnung eignen. Zurzeit unterstützt unsere Implementierung ein maximum von 20 Inputbildern.
Implementierung

Nach dem Importieren des Datensatzes werden Farb- und Kantenhistogramme erstellt und verglichen. Anhand der Evaluierung und die SIFT Feature Punkte werden die Daten zu Klassen zugeordnet, wobei der Korrelationswert zwischen je zwei Klassen je kleiner, zwischen den Bilder der einzelnen Klassen je größer sein soll. Um die Klassenbereiche auf dem Ergebnisbild zu bestimmen, wird Voronoi Tesselation ausgeführt, wobei die die Anzahl der Zellen die Anzahl der Klassen entspricht, und die Punkte so weit wie möglich vonenander platziert werden. Um die wichtigsten Bereiche in einem Bild detektieren zu können, werden entsprechende saliency Werte pro Pixel berechnet und entsprechend des BING Objectness Algorithmus werden innere und außere saliente Bereiche auf den ildern definiert.

Nach der Ähnlichkeitsberechnung und dem Clustering werden die globalen Cluster grob im 2D Raum angeordnet, und danach werden die Clustergrenzen mittels Voronoi Tessellation ermittelt. Diese globalen Grenzen helfen beim Lokalen Clustering und Verschiebung der Lokalen Mittelpunkte damit die Punkte gut verteilt sind. Nach der lokalen Platzierung werden mittels FloodFill die einzelnen Clusterzellen mit deren Bildern gefüllt. Der Seedpunkt des Bildes ist der Mittelpunkt des inneren lokalen Saliency Bereich, welcher die Mitte des wichtigsten Bereiches im Bild darstellen soll. Das Bild nach dem äußeren Saliency Bereich skaliert um sicher zustellen dass der innere Bereich sichtbar ist.
Interaktion

Nach dem Start kann der Benutzer die gewünschte Fenstergröße eingeben. Danach wird ein Ordner ausgewählt welcher die Bilder beinhaltet welche sortiert werden sollen. Nach einer kurzen Wartezeit sieht man auch schon das Ergebnis. Es gibt nun zwei Modi, beim ersten kann man beim Klick auf ein globales Cluster das kleine Cluster nochmal größer betrachten mit eingezeichneten Saliency Bereich. Beim zweiten Modus sieht man beim Klick auf ein globales Cluster die Centroidal Voronoi Tessellation welche durchgeführt wurde.

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