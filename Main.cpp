#include "Bild.h"
#define DATEINAME_DER_BILDDATEI "Text.jpg"

int main(void)
{
	//Bild test;
	Bild bildObjekt(DATEINAME_DER_BILDDATEI);

	bildObjekt.convertToBinary();
	bildObjekt.removeSpeckle();

	bildObjekt.richteBildHorizontalAus();

	bildObjekt.textausgabe();

	//Programm anhalten
	cvWaitKey(0);
	//alle Fenster schlieﬂen
	destroyAllWindows();

	return 0;
}