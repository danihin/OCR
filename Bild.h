//DEFINES removeSpeckle()
#define SPECKLE_RADIUS 4 //SPECKLE_RADIUS gibt an, wie viele Pixel entfernt von jedem einzelnen Pixel nach schwarzen Pixeln gesucht werden soll (dabei werden SPECKLE_RADIUS Schritte sowohl vertikal, horizontal, als auch diagonal gegangen und die entsprechenden Pixel ausgewertet.
#define MINDESTANZAHL_SCHWARZER_PIXEL_IN_UMGEBUNG 68	//MINDESTANZAHL_SCHWARZER_PIXEL_IN_UMGEBUNG gibt an, wie viele schwarze Pixel im Umkreis (Radius: SPECKLE_RADIUS) eines Pixels gefunden werden müssen, damit dieser nicht als speckle identifiziert wird. MAXIMALWERT: [(1+2*SPECKLE_RADIUS)^2-1]. 

//DEFINES erkenneText
#define ZEICHENUEBERLAPPUNG_TOLERANZWERT 8 //gibt die Pixel an, die sich zwei Buchstaben überlappen dürfen. Normalerweise sollte im Bereich eines erkannten Templates nämlcih kein weiteres Template erkannt werden. Diese Regel wird hiermit ein wenig aufgeweicht und ermöglicht durch etwas herumprobieren einen geeigneten Wert zu ermitteln.
#define MATCHRATE_THRESHOLD_VALUE 0.80 //Einstellen, ab welchem Übereinstimmungswert (0: sehr schlecht, 1: perfekt) ein Maximum als Match gewertet wird.
#define RESULTMATRIZEN_GRAFISCH_AUSGEBEN false //Zur späteren Erläuterung der MatchFunktion oder allgemeinen Debugzwecken können die Resultmatrizen über diese Präprozessorkonstante ausgegeben werden. Jedoch ist dies für den normalen Programmbetrieb unnötig.

//DEFINES getZeile()
#define ZEILE_SEPARIEREN_TOLERANZ 1		//Anzahl der Pixel, die oben und unten zu einer Zeile hinzugefüt werden, um für erkenneText() eine verwertbare Matrix zu erzeugen.
#define SPALTE_SEPARIEREN_TOLERANZ 0	//Anzahl der Pixel, die vor und hinter einer Spalte hinzugefüt werden, um für erkenneText() eine verwertbare Matrix zu erzeugen.

//Includes für Memberfunktionen
#include <Windows.h>
#include <opencv/cv.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <limits>
#include <string.h>

using namespace cv;
using namespace std;

//TYPEDEFS erkenneText
//Struct-Datentyp, der Informationen zu Maximas bzw. Matches zusammenfasst
typedef struct{
		int templateNr;
		Point point;
		float matchRate;
	} structMatch;
//Struct-Datentyp, der Informationen zu einem Template zusammenfasst
typedef struct{
		Mat matTemplate;
		Mat matMatchResult;
		vector< structMatch > vectorBestMatches;
		string symbol;
		string dateinameDesTemplates;
		int templateNr;
			//Debugzwecke
			int iWieHaeufigGefunden;
	} structTemplate;
//Iteratoren
typedef vector< structTemplate >::iterator vectorStructTemplateIteratorType;
typedef vector< structMatch >::iterator structMatchVectorIteratorType;


class Bild
{
	public:
		//Methoden
		Bild(void);
		Bild(string sDateiName);
		~Bild(void);
		bool ladeBild(string sDateiName);
		Mat convertToBinary(Mat matEingabeBild = Mat::zeros(0,0,0));
		void removeSpeckle();
		void richteBildHorizontalAus();
		void separiereZeilen(Mat matEingabeBild);
		string erkenneText(Mat &matWort);
		void textausgabe();


			Mat getZeile(Mat &originalBild);
			Mat getSpalte(Mat &originalBild);
			

		//static Methoden
		static bool Bild::compareVectorOfStructMatch(const structMatch &structMatchA, const structMatch &structMatchB);

		//Membervariablen
		Mat matOriginalBild;
		Mat matErgebnisBild;
		stringstream ssErkannterText;


	private:
		void ladeTemplates(void);
		//Unterfunktionen der erkenneText-Funktion, die von außen nicht sichtbar sein sollen
			bool anyMatchesLeft(void);
			void ermittleLokaleMaximaDerMatchMatrizen(const Mat& matMatchResult, vector< structMatch >& vectorStructBestMatches, const int iTemplateNr);
		//Unterfunktion der richteBildHorizontalAus()-Funktion, die von außen nicht sichtbar sein sollen
			IplImage* rotateImage(const IplImage *src, float angleDegrees);
		

			//getZeile
			Mat Set_Zeilen_Matrix(Mat &originalBild_, int iPoint_Anfang_Zeile, int iPoint_Ende_Zeile);
			bool empty(Mat &originalBild);
			
			//getSpalte
			Mat cutSpalte(Mat &originalBild);
			Mat Set_Spalten_Matrix(Mat &originalBild, int iPoint_Anfang_Spalte, int iPoint_Ende_Spalte);
			
		//Variablen
		int iTemplateCounter;
		vector<structTemplate> vectorStructTemplates;	//Speichert dynamisch die eingelesenen Templates und alle zugehörigen Informationen wie evtl. Maxima
		vector<Mat> vecMatZeilen; //Speichert Zeilen als Mat-Variablen

		//-->ggf. noch lokal machen	
			//Globaler Vektor um Wörter/Leerzeichen zu speichern
			vector<Mat> vecMatGlobal; 
			//Array um Schnittpunkte zu speichern / ARRAYGRÖßE: Anzahl Wörter pro Zeile
			int iCutPos [100];	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!vector rausmachen!!!
};
