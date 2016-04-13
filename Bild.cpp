#include "Bild.h"

//Standard-Konstruktor
Bild::Bild(void)
{
	this->ladeTemplates();
}

//Alternativer Konstruktor
Bild::Bild(string sDateiName)
{
	this->ladeTemplates();
	this->ladeBild(sDateiName);
}

//Destruktor
Bild::~Bild(void)
{
}

//Zu analysierendes Bild in den Speicher laden
bool Bild::ladeBild(string sDateiName)
{
	//Eingabebild in Graustufen laden und in Membervariable speichern
	matOriginalBild = imread(sDateiName, 0);
	//Kopie in ErgebnisBild ablegen - dieses wird von nun an in allen Memberfunktionen verändert
	matErgebnisBild = matOriginalBild;


	//Prüfen, ob Bild erfolgreich geladen werden konnte
	if (matOriginalBild.empty())
	{
		printf("Kein Bild gefunden...");
		getchar();
		return false;
	}
	else
	{
			//Zu Debugzwecken Zwischenergebnis anzeigen und speichern
			namedWindow("1_original_Bild_window", CV_WINDOW_AUTOSIZE);
			//Nur wegen Demo in Farbe
			imshow("1_original_Bild_window", imread(sDateiName, CV_LOAD_IMAGE_COLOR));
			imwrite("1_original.jpg", matErgebnisBild);

		return true;
	}

}

//Einlesen der Templates aus dem Templateordner. Anschließend liegen alle Templates gut strukturiert im vector vectorStructTemplates
void Bild::ladeTemplates()
{
	stringstream ssSpeicherpfadDesTemplates("");
	//Templates einlesen
	try
	{
			string sDateinameDesTemplates;
			structTemplate structTemplateVariable;

			//Datei "Zuordnung (diese Datei hier nicht umbenennen).txt" einlesen. Enthält die Namen der Templates und das entsprechende ASCII Zeichen, das bei erfolgreicher Erkennung ausgegeben wird
			ifstream isEingabestrom("Templates\\Zuordnung (diese Datei hier nicht umbenennen).txt");

			//Prüfe, ob Öffnen erfolgreich war
			if ( isEingabestrom.good() )
			{	//Erfolgreich eingelesen
				
					//Zählt Anzahl der eingelesenen Templatefiles mit
					iTemplateCounter=0;

					//schaltet auf true um, wenn der Eingabestrom in den gültigen Bereich (ab ---START---) eintritt und schaltet erneut um. wenn der gültige Bereich (bei "---ENDE---") verlassen wird
					 //muss auf false initialisiert werden.
					string sFindeAnfangEnde("");
					bool bGueltigeDaten = false;

					//alle Objekte aus dem ifstream erstellen/laden (--> Soll später von Funktion durchgeführt werden, da übersichtlicher. Das Laden kann dann folgendermaßen erfolgen: templatesEinlesen(isEingabestrom);
					while (!isEingabestrom.eof())
					{
						//nächsten String einlesen
						if ( sFindeAnfangEnde != "---START---")	//<-- evtl verwirrend. Erläuterung: Wird der gueltige Bereich verlassen (---ENDE---) wird bGueltigeDatenm invertiert und gleichzeitig das nächste Wort ausgelesen, sofern vorhanden. Handelt es sich hierbei wieder direkt um ein ---START--- (Gründe dafür sind ja erstmal egal), dann muss das bGueltigeDaten-bit direkt wieder invertiert werden, was nicht geschehen würde, wenn man vorher wieder das nächste Wort einliest.
							isEingabestrom >> sFindeAnfangEnde;	
						//Falls 
						if ( (sFindeAnfangEnde == "---START---") || (sFindeAnfangEnde == "---ENDE---") )
						{
							//Invertiere bGueltigeDaten;
							bGueltigeDaten^=true;
							//String überschreiben, damit Dateiname drin steht und in den nächsten Zeilen verwendet werden kann
							isEingabestrom >> sFindeAnfangEnde;
						}


						//Wenn das ---ENDE--- des gueltigen Bereichs der Zuordnungsdatei nicht erreicht ist.
						if ( bGueltigeDaten )
						{
							//Nächstes Zuordnungspaar auslesen
							structTemplateVariable.dateinameDesTemplates = sFindeAnfangEnde;
							isEingabestrom >> structTemplateVariable.symbol;

							structTemplateVariable.templateNr = iTemplateCounter;

							//Template einlesen
							ssSpeicherpfadDesTemplates.str(""); //leeren
							ssSpeicherpfadDesTemplates << "Templates/" << structTemplateVariable.dateinameDesTemplates;
							structTemplateVariable.matTemplate = this->convertToBinary( imread( ssSpeicherpfadDesTemplates.str() ) );	//Template muss für matchTemplate()-Funktion vom selben Typ sein, wie matErgebnisBild, d.h. 8bit-schwarzweiß und idealerweise binaer

							//(fast) vollständiges Template auf Vektor legen (-> Sammlung aller Templates)
							vectorStructTemplates.push_back( structTemplateVariable );

							//Anzahl der Templates mitzählen für for-Schleifen (wäre auch über geeigneten Iteratortyp möglich, aber wäre Variable dann dennoch nötig, um sich eine Stelle eines Veltors zu merken bzw. die templateNr im struct zu speichern)
							iTemplateCounter++;

									//Debugzwecke
									if (RESULTMATRIZEN_GRAFISCH_AUSGEBEN==true)
									{
										//Für spätere grafische Ausgabe der Matchmatrix Fenster erzeugen
										//namedWindow( structTemplateVariable.dateinameDesTemplates, CV_WINDOW_AUTOSIZE );
									}
						}

				}

			}
			else
			{
				//Sonst Fehlermeldung:
				string sException = "Fehler beim Einlesen der Datei ..\\Templates\\Zuordnung.txt";			
				throw sException;
			}
	}
	catch (string sException)
	{
		MessageBoxA(NULL, "Fehler beim Einlesen der Datei ..\\Templates\\Zuordnung.txt", NULL, MB_OK | MB_ICONINFORMATION);
	}
}

//Eingelesenes Bild wir zunächst in ein Graustufenbild konvertiert und anschließend werden die Pixel entweder schwarz (value: 0) ODER weiß (value = 255)
Mat Bild::convertToBinary(Mat matEingabeBild)
{
	//Membervariable als default-Value für matEingabeBild.
	if ( (matEingabeBild.cols==0) && (matEingabeBild.rows==0) )
	{
		//RGB Bild in Grauton umwandeln und direkt wieder in matErgebnisBild speichern
		//cvtColor(matErgebnisBild, matErgebnisBild, CV_BGR2GRAY);	//wird nun schon direkt bei imread(balbalbala , 0) gemacht.

			//Zu Debugzwecken Zwischenergebnis anzeigen und speichern
			//namedWindow("2_graues_Bild_window", CV_WINDOW_AUTOSIZE);
			//imshow("2_graues_Bild_window", matErgebnisBild);
			imwrite("2_grau.jpg", matErgebnisBild);

		//Mittels threshold-Methode ein Binärbild (schwarz-weiß) erzeugen. Syntax: http://docs.opencv.org/modules/imgproc/doc/miscellaneous_transformations.html?highlight=threshold#threshold
		threshold(matErgebnisBild, matErgebnisBild, 207, 255, THRESH_BINARY);
	
			/*---------------------------------------------------------------------------------------------
	Der nachfolgende Code zeigt beispielhaft, wie man einzelne Pixel eines Mat-Objektes ändern kann
	Die gleiche Funktion lässt sich mit der threshold-Methode erreichen. Siehe http://docs.opencv.org/modules/imgproc/doc/miscellaneous_transformations.html?highlight=threshold#threshold*/
			/*
			int iAnzahlZeilen = matOriginalBild.rows;
			int iAnzahlSpalten = matOriginalBild.cols;

			for(int iZeile=0; iZeile<iAnzahlZeilen; iZeile++)
					{

						//Returns pointer to the specified matrix row
						uchar* pixelAdresse = matOriginalBild.ptr(zeile);

						for(int spalte=0; spalte<iAnzahlSpalten; spalte++)
						{
							//Damit könnte man eventuell schneller arbeiten, darum lass ich den Code mal hier stehen.
								//Enthält die Adresse des Pixels
								//pixelAdresse++;

							//Pixelwert ändern. Mehr Informationen dazu: http://stackoverflow.com/questions/9104869/how-can-i-change-a-pixel-value-from-a-grayscaled-image-using-opencv-2-3
							if ( matErgebnisBild.at<uchar>(zeile, spalte) < 128 )	//128 ist hier der sogenannte Threshold-Wert, ab dem ein Pixel weiß gefärbt wird.
							{
								//Pixelwert 0: schwarz
								matErgebnisBild.at<uchar>(zeile, spalte) = 0;
							}
							else
							{
								//Pixelwert 255: weiß
								matErgebnisBild.at<uchar>(zeile, spalte) = 255;
							}
						}
					}
	------------------------------------------------------------------------------------------------------*/

			//Zu Debugzwecken Zwischenergebnis anzeigen und speichern
			//namedWindow("3_binaerBild_window", CV_WINDOW_AUTOSIZE);
			//imshow("3_binaerBild_window", matErgebnisBild);
			imwrite("3_binaer.jpg", matErgebnisBild);
		
		return matErgebnisBild;
	}
	else //Template soll umgewnadelt werden in Binary
	{
		//RGB Bild in Grauton umwandeln und direkt wieder in matErgebnisBild speichern
		cvtColor(matEingabeBild, matEingabeBild, CV_BGR2GRAY);

		//Mittels threshold-Methode ein Binärbild (schwarz-weiß) erzeugen. Syntax: http://docs.opencv.org/modules/imgproc/doc/miscellaneous_transformations.html?highlight=threshold#threshold
		threshold(matEingabeBild, matEingabeBild, 220, 255, THRESH_BINARY);

		return matEingabeBild;
	}

}
	//Hinweis: Für Übergabeparamter existiert ein default-Wert.

void Bild::removeSpeckle()
{
			//Speichert die vorgenommen Veränderungen
			//Mat matErgebnisBild_temp;
			//matErgebnisBild_temp = matOriginalBild;
			 vector<Point> vectorSpeckle;

			int iAnzahlZeilen = matErgebnisBild.rows;
			int iAnzahlSpalten = matErgebnisBild.cols;

			//Speichert die Anzahl gefundener schwarzer Pixel in Umgebung eines Pixels.
			int iAnzahlSchwarzerPixelInUmgebung;

			//OHNE Randbereich
			for(int iZeile=SPECKLE_RADIUS; iZeile<iAnzahlZeilen-SPECKLE_RADIUS; iZeile++)	//ersteinmal nicht bis ganz an den Rand gehen, da sonst Speicherfehler auftreten würden
					{
						//Verarbeitung des Bildes ohne den Randbereich
						for(int iSpalte=SPECKLE_RADIUS; iSpalte<iAnzahlSpalten-SPECKLE_RADIUS; iSpalte++)	//ersteinmal nicht bis ganz an den Rand gehen, da sonst Speicherfehler auftreten würden
						{
							//Initialisierung mit -1, da nur in der Umgebnung von Pixeln gesucht wird, die schwarz sind und der schwarze automatisch mitgezählt wird, was nicht sein sollte.
							iAnzahlSchwarzerPixelInUmgebung = -1;

							if ( matErgebnisBild.at<uchar>(iZeile, iSpalte) == 0 ) //aktueller Pixelwert ist schwarz? Dann Umgebnung betrachten. (Dies kann man sich sonst nämlich sparen, da an einem weißen Pixel eh nichts verändert wird.
							{													   //Aus diesem Grund kann auch iAnzahlSchwarzerPixelInUmgebung mit -1 initialisiert werden, da so der mittlere Pixel von vornerein ausgeblendet wird.
									//Schwarze Pixel in Umgebung des gerade betrachteten Pixels zählen; aber NICHT DEN gerade betrachetetn Pixel.
									for(int iUmgebungsSpalte=iSpalte-SPECKLE_RADIUS; iUmgebungsSpalte<=iSpalte+SPECKLE_RADIUS; iUmgebungsSpalte++)
									{
										for(int iUmgebungsZeile=iZeile-SPECKLE_RADIUS; iUmgebungsZeile<=iZeile+SPECKLE_RADIUS; iUmgebungsZeile++)
										{
											//Umgebungspixel ist schwarz
											if ( matErgebnisBild.at<uchar>(iUmgebungsZeile, iUmgebungsSpalte) == 0 )
											{
												iAnzahlSchwarzerPixelInUmgebung++;
											}

										}
									}


									if ( iAnzahlSchwarzerPixelInUmgebung < MINDESTANZAHL_SCHWARZER_PIXEL_IN_UMGEBUNG)									
									{
										//Pixelwert zu weiß (255) ändern
										//matErgebnisBild.at<uchar>(iZeile, iSpalte) = 255;

										//merkt sich die Pixel, deren Wert geändert werden muss, da nicht genügend schwarze Pixel in der Umgebung waren, d.h. es handelt sich um einen Speckle.
										vectorSpeckle.push_back( Point(iZeile, iSpalte) );
									}
									//else: Pixel unverändert lassen.
									
							}
						}
					}
			
			// ###### SONDERBEHANDLUNG DER RANDBEREICHE ###### 

				//oberer Randbereich (ohne Ecken)
				for(int iZeile=0; iZeile<SPECKLE_RADIUS; iZeile++)
				{
					for(int iSpalte=SPECKLE_RADIUS; iSpalte<=iAnzahlSpalten-1-SPECKLE_RADIUS; iSpalte++)
					{
						//Initialisierung mit -1, da nur in der Umgebnung von Pixeln gesucht wird, die schwarz sind und der schwarze automatisch mitgezählt wird, was nicht sein sollte.
						iAnzahlSchwarzerPixelInUmgebung = -1;
												
						if ( matErgebnisBild.at<uchar>(iZeile, iSpalte) == 0 ) //aktueller Pixelwert ist schwarz? Dann Umgebnung betrachten. (Dies kann man sich sonst nämlich sparen, da an einem weißen Pixel eh nichts verändert wird.
						{													   //Aus diesem Grund kann auch iAnzahlSchwarzerPixelInUmgebung mit -1 initialisiert werden, da so der mittlere Pixel von vornerein ausgeblendet wird.
								//Schwarze Pixel in Umgebung des gerade betrachteten Pixels zählen; aber NICHT DEN gerade betrachetetn Pixel.
								for(int iUmgebungsSpalte=iSpalte-SPECKLE_RADIUS; iUmgebungsSpalte<=iSpalte+SPECKLE_RADIUS; iUmgebungsSpalte++)
								{
									for(int iUmgebungsZeile=0; iUmgebungsZeile<=iZeile+SPECKLE_RADIUS; iUmgebungsZeile++)	//Anmerkung: das nur der Randbereich betrachett wird, kann immer von der 0.ten Zeile aus gezählt werden. Muss man sich einfach nur mal kurz klar machen.
									{
											//Umgebungspixel ist schwarz
											if ( matErgebnisBild.at<uchar>(iUmgebungsZeile, iUmgebungsSpalte) == 0 )
											{
												iAnzahlSchwarzerPixelInUmgebung++;
											}
									}
								}

								if ( iAnzahlSchwarzerPixelInUmgebung < MINDESTANZAHL_SCHWARZER_PIXEL_IN_UMGEBUNG)									
								{
									//Pixelwert zu weiß (255) ändern
									//matErgebnisBild.at<uchar>(iZeile, iSpalte) = 255;

									//merkt sich die Pixel, deren Wert geändert werden muss, da nicht genügend schwarze Pixel in der Umgebung waren, d.h. es handelt sich um einen Speckle.
									vectorSpeckle.push_back( Point(iZeile, iSpalte) );
								}
						}
						//else: Pixel unverändert lassen.
					}
				}
	

				//unterer Randbereich (ohne Ecken)
				for(int iZeile=iAnzahlZeilen-1-SPECKLE_RADIUS; iZeile<=iAnzahlZeilen-1; iZeile++)
				{
					for(int iSpalte=SPECKLE_RADIUS; iSpalte<=iAnzahlSpalten-1-SPECKLE_RADIUS; iSpalte++)
					{
						//Initialisierung mit -1, da nur in der Umgebnung von Pixeln gesucht wird, die schwarz sind und der schwarze automatisch mitgezählt wird, was nicht sein sollte.
						iAnzahlSchwarzerPixelInUmgebung = -1;

						if ( matErgebnisBild.at<uchar>(iZeile, iSpalte) == 0 ) //aktueller Pixelwert ist schwarz? Dann Umgebnung betrachten. (Dies kann man sich sonst nämlich sparen, da an einem weißen Pixel eh nichts verändert wird.
						{													   //Aus diesem Grund kann auch iAnzahlSchwarzerPixelInUmgebung mit -1 initialisiert werden, da so der mittlere Pixel von vornerein ausgeblendet wird.
								//Schwarze Pixel in Umgebung des gerade betrachteten Pixels zählen; aber NICHT DEN gerade betrachetetn Pixel.
								for(int iUmgebungsSpalte=iSpalte-SPECKLE_RADIUS; iUmgebungsSpalte<=iSpalte+SPECKLE_RADIUS; iUmgebungsSpalte++)
								{
									for(int iUmgebungsZeile=iZeile-SPECKLE_RADIUS; iUmgebungsZeile<=iAnzahlZeilen-1; iUmgebungsZeile++)	//Anmerkung: das nur der Randbereich betrachett wird, kann immer von der 0.ten Zeile aus gezählt werden. Muss man sich einfach nur mal kurz klar machen.
									{
											//Umgebungspixel ist schwarz
											if ( matErgebnisBild.at<uchar>(iUmgebungsZeile, iUmgebungsSpalte) == 0 )
											{
												iAnzahlSchwarzerPixelInUmgebung++;
											}
									}
								}

								if ( iAnzahlSchwarzerPixelInUmgebung < MINDESTANZAHL_SCHWARZER_PIXEL_IN_UMGEBUNG)									
								{
									//Pixelwert zu weiß (255) ändern
									//matErgebnisBild.at<uchar>(iZeile, iSpalte) = 255;
									
									//merkt sich die Pixel, deren Wert geändert werden muss, da nicht genügend schwarze Pixel in der Umgebung waren, d.h. es handelt sich um einen Speckle.
									vectorSpeckle.push_back( Point(iZeile, iSpalte) );
								}
						}
						//else: Pixel unverändert lassen.
					}
				}

				
				//linker Randbereich (ohne Ecken)
				for(int iSpalte=0; iSpalte<SPECKLE_RADIUS; iSpalte++)
				{
					for(int iZeile=SPECKLE_RADIUS; iZeile<=iAnzahlZeilen-1-SPECKLE_RADIUS; iZeile++)
					{
						//Initialisierung mit -1, da nur in der Umgebnung von Pixeln gesucht wird, die schwarz sind und der schwarze automatisch mitgezählt wird, was nicht sein sollte.
						iAnzahlSchwarzerPixelInUmgebung = -1;
												
						if ( matErgebnisBild.at<uchar>(iZeile, iSpalte) == 0 ) //aktueller Pixelwert ist schwarz? Dann Umgebnung betrachten. (Dies kann man sich sonst nämlich sparen, da an einem weißen Pixel eh nichts verändert wird.
						{													   //Aus diesem Grund kann auch iAnzahlSchwarzerPixelInUmgebung mit -1 initialisiert werden, da so der mittlere Pixel von vornerein ausgeblendet wird.
								//Schwarze Pixel in Umgebung des gerade betrachteten Pixels zählen; aber NICHT DEN gerade betrachetetn Pixel.
								for(int iUmgebungsZeile=iZeile-SPECKLE_RADIUS; iUmgebungsZeile<=iZeile+SPECKLE_RADIUS; iUmgebungsZeile++)
								{
									for(int iUmgebungsSpalte=0; iUmgebungsSpalte<=iSpalte+SPECKLE_RADIUS; iUmgebungsSpalte++)	//Anmerkung: das nur der Randbereich betrachett wird, kann immer von der 0.ten Zeile aus gezählt werden. Muss man sich einfach nur mal kurz klar machen.
									{
											//Umgebungspixel ist schwarz
											if ( matErgebnisBild.at<uchar>(iUmgebungsZeile, iUmgebungsSpalte) == 0 )
											{
												iAnzahlSchwarzerPixelInUmgebung++;
											}
									}
								}

								if ( iAnzahlSchwarzerPixelInUmgebung < MINDESTANZAHL_SCHWARZER_PIXEL_IN_UMGEBUNG)									
								{
									//Pixelwert zu weiß (255) ändern
									//matErgebnisBild.at<uchar>(iZeile, iSpalte) = 255;
									
									//merkt sich die Pixel, deren Wert geändert werden muss, da nicht genügend schwarze Pixel in der Umgebung waren, d.h. es handelt sich um einen Speckle.
									vectorSpeckle.push_back( Point(iZeile, iSpalte) );
								}
						}
						//else: Pixel unverändert lassen.
					}
				}
	
									
				//rechter Randbereich (ohne Ecken)
				for(int iSpalte=iAnzahlSpalten-1-SPECKLE_RADIUS; iSpalte<=iAnzahlSpalten-1;iSpalte++)
				{
					for(int iZeile=SPECKLE_RADIUS; iZeile<=iAnzahlZeilen-1-SPECKLE_RADIUS; iZeile++)
					{
						//Initialisierung mit -1, da nur in der Umgebnung von Pixeln gesucht wird, die schwarz sind und der schwarze automatisch mitgezählt wird, was nicht sein sollte.
						iAnzahlSchwarzerPixelInUmgebung = -1;

						if ( matErgebnisBild.at<uchar>(iZeile, iSpalte) == 0 ) //aktueller Pixelwert ist schwarz? Dann Umgebnung betrachten. (Dies kann man sich sonst nämlich sparen, da an einem weißen Pixel eh nichts verändert wird.
						{													   //Aus diesem Grund kann auch iAnzahlSchwarzerPixelInUmgebung mit -1 initialisiert werden, da so der mittlere Pixel von vornerein ausgeblendet wird.
								//Schwarze Pixel in Umgebung des gerade betrachteten Pixels zählen; aber NICHT DEN gerade betrachetetn Pixel.
								for(int iUmgebungsZeile=iZeile-SPECKLE_RADIUS; iUmgebungsZeile<=iZeile+SPECKLE_RADIUS; iUmgebungsZeile++)
								{
									for(int iUmgebungsSpalte=iSpalte-SPECKLE_RADIUS; iUmgebungsSpalte<=iAnzahlSpalten-1; iUmgebungsSpalte++)	//Anmerkung: das nur der Randbereich betrachett wird, kann immer von der 0.ten Zeile aus gezählt werden. Muss man sich einfach nur mal kurz klar machen.
									{
											//Umgebungspixel ist schwarz
											if ( matErgebnisBild.at<uchar>(iUmgebungsZeile, iUmgebungsSpalte) == 0 )
											{
												iAnzahlSchwarzerPixelInUmgebung++;
											}
									}
								}

								if ( iAnzahlSchwarzerPixelInUmgebung < MINDESTANZAHL_SCHWARZER_PIXEL_IN_UMGEBUNG)									
								{
									//Pixelwert zu weiß (255) ändern
									//matErgebnisBild.at<uchar>(iZeile, iSpalte) = 255;
									
									//merkt sich die Pixel, deren Wert geändert werden muss, da nicht genügend schwarze Pixel in der Umgebung waren, d.h. es handelt sich um einen Speckle.
									vectorSpeckle.push_back( Point(iZeile, iSpalte) );
								}
						}
						//else: Pixel unverändert lassen.
					}
				}

			// ###### SONDERBEHANDLUNG DER ECKEN ###### 

				//Ecke oben links
						//Initialisierung mit -1, da nur in der Umgebnung von Pixeln gesucht wird, die schwarz sind und der schwarze automatisch mitgezählt wird, was nicht sein sollte.
						iAnzahlSchwarzerPixelInUmgebung = -1;

						if ( matErgebnisBild.at<uchar>(0, 0) == 0 ) //Pixelwert ist schwarz? Dann Umgebnung betrachten. (Dies kann man sich sonst nämlich sparen, da an einem weißen Pixel eh nichts verändert wird.
						{											//Aus diesem Grund kann auch iAnzahlSchwarzerPixelInUmgebung mit -1 initialisiert werden, da so der mittlere Pixel von vornerein ausgeblendet wird.
								//Schwarze Pixel in Umgebung des gerade betrachteten Pixels zählen; aber NICHT DEN gerade betrachetetn Pixel.
								for(int iUmgebungsZeile=0; iUmgebungsZeile<=SPECKLE_RADIUS; iUmgebungsZeile++)
								{
									for(int iUmgebungsSpalte=0; iUmgebungsSpalte<=SPECKLE_RADIUS; iUmgebungsSpalte++)	//Anmerkung: das nur der Randbereich betrachett wird, kann immer von der 0.ten Zeile aus gezählt werden. Muss man sich einfach nur mal kurz klar machen.
									{
											//Umgebungspixel ist schwarz
											if ( matErgebnisBild.at<uchar>(iUmgebungsZeile, iUmgebungsSpalte) == 0 )
											{
												iAnzahlSchwarzerPixelInUmgebung++;
											}
									}
								}

								if ( iAnzahlSchwarzerPixelInUmgebung < MINDESTANZAHL_SCHWARZER_PIXEL_IN_UMGEBUNG)									
								{
									//Pixelwert zu weiß (255) ändern
									matErgebnisBild.at<uchar>(0, 0) = 255;
								}
						}

				//Ecke oben rechts
						//Initialisierung mit -1, da nur in der Umgebnung von Pixeln gesucht wird, die schwarz sind und der schwarze automatisch mitgezählt wird, was nicht sein sollte.
						iAnzahlSchwarzerPixelInUmgebung = -1;

						if ( matErgebnisBild.at<uchar>(0, iAnzahlSpalten-1) == 0 ) //Pixelwert ist schwarz? Dann Umgebnung betrachten. (Dies kann man sich sonst nämlich sparen, da an einem weißen Pixel eh nichts verändert wird.
						{														   //Aus diesem Grund kann auch iAnzahlSchwarzerPixelInUmgebung mit -1 initialisiert werden, da so der mittlere Pixel von vornerein ausgeblendet wird.
								//Schwarze Pixel in Umgebung des gerade betrachteten Pixels zählen; aber NICHT DEN gerade betrachetetn Pixel.
								for(int iUmgebungsZeile=0; iUmgebungsZeile<=SPECKLE_RADIUS; iUmgebungsZeile++)
								{
									for(int iUmgebungsSpalte=iAnzahlSpalten-1-SPECKLE_RADIUS; iUmgebungsSpalte<=iAnzahlSpalten-1; iUmgebungsSpalte++)	//Anmerkung: das nur der Randbereich betrachett wird, kann immer von der 0.ten Zeile aus gezählt werden. Muss man sich einfach nur mal kurz klar machen.
									{
											//Umgebungspixel ist schwarz
											if ( matErgebnisBild.at<uchar>(0, iUmgebungsSpalte) == 0 )
											{
												iAnzahlSchwarzerPixelInUmgebung++;
											}
									}
								}

								if ( iAnzahlSchwarzerPixelInUmgebung < MINDESTANZAHL_SCHWARZER_PIXEL_IN_UMGEBUNG)									
								{
									//Pixelwert zu weiß (255) ändern
									matErgebnisBild.at<uchar>(0, iAnzahlSpalten-1) = 255;
								}
						}

				//Ecke unten links
						//Initialisierung mit -1, da nur in der Umgebnung von Pixeln gesucht wird, die schwarz sind und der schwarze automatisch mitgezählt wird, was nicht sein sollte.
						iAnzahlSchwarzerPixelInUmgebung = -1;

						if ( matErgebnisBild.at<uchar>(iAnzahlZeilen-1, 0) == 0 ) //Pixelwert ist schwarz? Dann Umgebnung betrachten. (Dies kann man sich sonst nämlich sparen, da an einem weißen Pixel eh nichts verändert wird.
						{														  //Aus diesem Grund kann auch iAnzahlSchwarzerPixelInUmgebung mit -1 initialisiert werden, da so der mittlere Pixel von vornerein ausgeblendet wird.
								//Schwarze Pixel in Umgebung des gerade betrachteten Pixels zählen; aber NICHT DEN gerade betrachetetn Pixel.
								for(int iUmgebungsZeile=iAnzahlZeilen-1-SPECKLE_RADIUS; iUmgebungsZeile<=iAnzahlZeilen-1; iUmgebungsZeile++)
								{
									for(int iUmgebungsSpalte=0; iUmgebungsSpalte<=SPECKLE_RADIUS; iUmgebungsSpalte++)	//Anmerkung: das nur der Randbereich betrachett wird, kann immer von der 0.ten Zeile aus gezählt werden. Muss man sich einfach nur mal kurz klar machen.
									{
											//Umgebungspixel ist schwarz
											if ( matErgebnisBild.at<uchar>(iUmgebungsZeile, 0) == 0 )
											{
												iAnzahlSchwarzerPixelInUmgebung++;
											}
									}
								}

								if ( iAnzahlSchwarzerPixelInUmgebung < MINDESTANZAHL_SCHWARZER_PIXEL_IN_UMGEBUNG)									
								{
									//Pixelwert zu weiß (255) ändern
									matErgebnisBild.at<uchar>(iAnzahlZeilen-1, 0) = 255;
								}
						}

				//Ecke unten rechts
						//Initialisierung mit -1, da nur in der Umgebnung von Pixeln gesucht wird, die schwarz sind und der schwarze automatisch mitgezählt wird, was nicht sein sollte.
						iAnzahlSchwarzerPixelInUmgebung = -1;

						if ( matErgebnisBild.at<uchar>(iAnzahlZeilen-1, iAnzahlSpalten-1) == 0 ) //Pixelwert ist schwarz? Dann Umgebnung betrachten. (Dies kann man sich sonst nämlich sparen, da an einem weißen Pixel eh nichts verändert wird.
						{																		 //Aus diesem Grund kann auch iAnzahlSchwarzerPixelInUmgebung mit -1 initialisiert werden, da so der mittlere Pixel von vornerein ausgeblendet wird.
								//Schwarze Pixel in Umgebung des gerade betrachteten Pixels zählen; aber NICHT DEN gerade betrachetetn Pixel.
								for(int iUmgebungsZeile=iAnzahlZeilen-1-SPECKLE_RADIUS; iUmgebungsZeile<=iAnzahlZeilen-1; iUmgebungsZeile++)
								{
									for(int iUmgebungsSpalte=iAnzahlSpalten-1-SPECKLE_RADIUS; iUmgebungsSpalte<=iAnzahlSpalten-1; iUmgebungsSpalte++)	//Anmerkung: das nur der Randbereich betrachett wird, kann immer von der 0.ten Zeile aus gezählt werden. Muss man sich einfach nur mal kurz klar machen.
									{
											//Umgebungspixel ist schwarz
											if ( matErgebnisBild.at<uchar>(iUmgebungsZeile, iUmgebungsSpalte) == 0 )
											{
												iAnzahlSchwarzerPixelInUmgebung++;
											}
									}
								}

								if ( iAnzahlSchwarzerPixelInUmgebung < MINDESTANZAHL_SCHWARZER_PIXEL_IN_UMGEBUNG)									
								{
									//Pixelwert zu weiß (255) ändern
									matErgebnisBild.at<uchar>(iAnzahlZeilen-1, iAnzahlSpalten-1) = 255;
								}
						}
				

	//Änderungen übernehmen/durchführen
	while( !vectorSpeckle.empty() )
	{
		//Pixelwert zu weiß (255) ändern
		matErgebnisBild.at<uchar>(vectorSpeckle.back().x, vectorSpeckle.back().y) = 255;
		vectorSpeckle.pop_back();
	}

									
			//Zu Debugzwecken Zwischenstufe anzeigen und speichern
			//namedWindow("4_ohne_speckle_window", CV_WINDOW_AUTOSIZE);
			//imshow("4_ohne_speckle_window", matErgebnisBild);
			imwrite("4_ohne_speckle.jpg", matErgebnisBild);
	
}

//Bild so ausrichten, dass die Zeilen horizontal vorliegen
void Bild::richteBildHorizontalAus()
{
	
	//Mat ausgabeBild = Mat::zeros(src.size(), src.type());
	Size size = matErgebnisBild.size();

	//Matrix wird invertiert
	bitwise_not(matErgebnisBild, matErgebnisBild);

	//Vector um Endpunkte zu speichern
	vector<cv::Vec4i> lines;

	
	//Probabilistic Hough Transform 
	HoughLinesP(matErgebnisBild, lines, 1, CV_PI/180, 100, size.width / 2.87f, 200);	//!!!!!!!!!!!zerstört Funktionsstack  ->Absturz bei return

	Mat disp_lines(size, CV_8UC1, Scalar(0, 0, 0));
    double angle = 0.;
    unsigned nb_lines = lines.size();

	
    for (unsigned i = 0; i < nb_lines; ++i)
    {
        line(disp_lines, Point(lines[i][0], lines[i][1]),
					Point(lines[i][2], lines[i][3]), Scalar(255, 0 ,0));
        angle += atan2((double)lines[i][3] - lines[i][1],
                       (double)lines[i][2] - lines[i][0]);
    }

    angle /= nb_lines; // mean angle, in radians.

	imwrite("8_ohne_speckle.jpg", disp_lines);
 
    //cout << "File " << ": " << angle * 180 / CV_PI << std::endl;

	//Matrix wird rotiert
	IplImage iplErgebnisBild = matErgebnisBild;
	matErgebnisBild = rotateImage(&iplErgebnisBild, -angle * 180 / CV_PI);	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!mat statt ipl Image
	bitwise_not(matErgebnisBild, matErgebnisBild);

	/*
	//namedWindow("vRot", CV_WINDOW_AUTOSIZE);
	//imshow("vRot", matErgebnisBild);
 
    //imshow("vRot2", disp_lines);
    waitKey(0);
    destroyWindow("vRot2");
	*/

	imwrite("5_hough.jpg", matErgebnisBild);
	
	return;
}
//---------ANFANG: Unterfunktion von Bild::richteBildHorizontalAus()----------
	// Rotate the image clockwise (or counter-clockwise if negative).
	IplImage* Bild::rotateImage(const IplImage *src, float angleDegrees)
	{
		// Create a map_matrix, where the left 2x2 matrix
		// is the transform and the right 2x1 is the dimensions.
		float m[6];
		CvMat M = cvMat(2, 3, CV_32F, m);
		int w = src->width;
		int h = src->height;
		float angleRadians = angleDegrees * ((float)CV_PI / 180.0f);
		m[0] = (float)( cos(angleRadians) );
		m[1] = (float)( sin(angleRadians) );
		m[3] = -m[1];
		m[4] = m[0];
		m[2] = w*0.5f;  
		m[5] = h*0.5f;  

		// Make a spare image for the result
		CvSize sizeRotated;
		sizeRotated.width = cvRound(w);
		sizeRotated.height = cvRound(h);

		// Rotate
		IplImage *imageRotated = cvCreateImage( sizeRotated,
			src->depth, src->nChannels );

		// Transform the image
		cvGetQuadrangleSubPix( src, imageRotated, &M);

		return imageRotated;
	}
//---------ENDE: Unterfunktion von Bild::richteBildHorizontalAus()----------

void Bild::textausgabe()
{
	//Txt-Datei anlegen
	ofstream text;
	text.open ("erkannterText.txt");

	//Zähler um Fenster gegebenfalls Fenster fortlaufen zu nummerieren
	int iZaehler = 0;

	
	//Vektor mit Zeilen wird erstellt und gefüllt
	getZeile(matErgebnisBild);

	//Zeilen in vecMatZeilen werden einzeln betrachtet und spaltenweise ausgelesen
	for (vector<Mat>::iterator iter = vecMatZeilen.begin(); iter != vecMatZeilen.end(); ++iter)                 
	{
		//Könnte so wesentlich eleganter programmiert werden
		/*string sWindowname = convertInt(iZaehler);
		string sVecName = "Zeile " + sWindowname;
		vector<Mat> sVecName; 
		ecZeilen.push_back(sVecName);
		vector<vector<Mat>> vecZeilen;
		vecZeilen.push_back(vecMatAktuelleSpalte);*/

		//NEU!!!! Erneute Binarisierung
		//threshold(*iter, *iter, 250, 255, THRESH_BINARY);

		//Aktuelle Zeile wird ausgewertet
		getSpalte(*iter);

		//Wörte der aktuellen Zeile wurden in vecMatGlobal gespeichert und werden jetzt ausgelesen 
		for (vector<Mat>::iterator iter = vecMatGlobal.begin(); iter != vecMatGlobal.end(); ++iter)                 
		{
			//Ausgabe der einzelnen Wörter
			/*string sWindowname = convertInt(iZaehler);
			//namedWindow(sWindowname, CV_WINDOW_AUTOSIZE);
			//imshow(sWindowname, *iter);*/

			//Wenn Matrix keine schwarzen Pixel enthält -> Leerzeichen
			if(empty(*iter) == true)
			{
				text << " ";
			}
			else
			{
				text << erkenneText(*iter);
			}

			iZaehler++;
		}
		//Nach jeder Zeile folgt ein Zeilenumbruch
		text << "\n";

		//Wörter in Zeile werden gelöscht, damit die neuen Wörter der neuen Zeile gespeichert werden können
		vecMatGlobal.clear();
	}

	//Eingabe in Textdatei schliesen
	text.close();
	ShellExecuteA(0,"open","notepad.exe","erkannterText.txt",NULL,SW_SHOW);

}

//Markiert Zeilen in Bild und speicher Zeilenmatrix in vecMatZeilen
Mat Bild::getZeile(Mat &originalBild)
{
	int iAnzahl_zeilen = originalBild.rows;
	int iAnzahl_spalten = originalBild.cols;

	Mat matReturnBild = originalBild;
	Mat matReturnBild_neu;

	//obere Linie von Zeile  NICHT gesetzt
	bool bLine = false;

	int iPoint_Anfang_Zeile;	
	int iPoint_Ende_Zeile;

	for(int zeile=0; zeile < iAnzahl_zeilen; zeile++)
	{
		int iPixelzaehler = 0;	

		for(int spalte=0; spalte < iAnzahl_spalten; spalte++)
		{
			if(matReturnBild.at<uchar>(zeile, spalte)== 0)
			{
				iPixelzaehler++;
			}
			//Setzt obere Linie von Zeile, wenn EIN Pixel in Zeile Schwarz ist
			if((matReturnBild.at<uchar>(zeile, spalte) == 0) && (bLine == false))
			{
				//void line(Mat& img, Point pt1, Point pt2, const Scalar& color, int thickness=1, int lineType=8, int shift=0)
				//line(matReturnBild,Point(0, zeile-1), Point(iAnzahl_spalten, zeile-1),Scalar(0,0,0), 1, 8, 0);
				iPoint_Anfang_Zeile = zeile - ZEILE_SEPARIEREN_TOLERANZ;															//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! Debug mäßig Zeile etwas höher /tiefer machen.
				//obere Linie von Zeile wurde gesetzt
				bLine = true;
			}	
		}

		//Setzt untere Linie von Zeile, wenn ALLE Pixel in Zeile weiß sind
		if((iPixelzaehler == 0) && (bLine == true))
		{
			//line(matReturnBild,Point(0, zeile), Point(iAnzahl_spalten, zeile), Scalar(0,0,0), 1, 8, 0);								//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! Debug mäßig Zeile etwas höher /tiefer machen.
			iPoint_Ende_Zeile = zeile-1+ ZEILE_SEPARIEREN_TOLERANZ;
			matReturnBild_neu = Set_Zeilen_Matrix(matReturnBild, iPoint_Anfang_Zeile, iPoint_Ende_Zeile);
			//Speicher Zeile in Vector vecMatZeilen
			vecMatZeilen.push_back(matReturnBild_neu);
			bLine = false;
		}
	}


			//Debugzwecke
			//vector Inhalte mit ImageWatch anzeigen
			for (vector<Mat>::iterator iter = vecMatZeilen.begin(); iter != vecMatZeilen.end(); ++iter)                 
			{
					matReturnBild_neu = *iter;
			}



	//imshow("vRot", matReturnBild_neu);
	//imwrite("zeilen_einzeln", matReturnBild_neu);

	return matReturnBild_neu;
}
//Markiert Wörter und Leerzeichen in Zeile
Mat Bild::getSpalte(Mat &originalBild)
{
	Mat matReturnBild = cutSpalte(originalBild);

	int iAnzahl_zeilen = matReturnBild.rows;
	int iAnzahl_spalten = matReturnBild.cols;

	//Der Wert 2.5 hat sich bei verschiedenen Fonts als optimal erwiesen
	int iLeerzeichen = iAnzahl_zeilen/3.5;

	//obere Linie von Zeile  NICHT gesetzt
	bool bLine = false;

	//Setze Schnittstellen in Array
	iCutPos[0] = 0;

	//Zur Berechnung von Abstand zwischen Buchstaben
	int iPos1 = 0;
	int iPos2 = 0;

	//Um Wörter und leerzeichen abzuschneiden
	int iCutPos1 = 0;
	int iCutPos2 = 0;

	int iZaehler = 1;

	for(int spalte=0; spalte < iAnzahl_spalten; spalte++)
	{
		int iPixelzaehler = 0;	

		for(int zeile=0; zeile < iAnzahl_zeilen; zeile++)
		{
			//Muss unabhängig von bLine gepüft werden
			if(matReturnBild.at<uchar>(zeile, spalte)== 0)
			{
				iPixelzaehler++;
			}

			//Setzt linke Linie von Spalte, wenn EIN Pixel in Spalte Schwarz ist
			if((matReturnBild.at<uchar>(zeile, spalte) == 0) && (bLine == false)) //&& (iPos1 != 0)
			{	
				//Setze iPos2 um später Abstand zwischen Buchstaben zu berechnen
				iPos2 = spalte;

				//Wenn Abstand > Lererzeichen
				if((iPos2-iPos1) > iLeerzeichen)
				{
					//Zeichne Hilfslinien
					/*//Zeichne rechte Linie von Wort
					//line(matReturnBild,Point(iPos1, 0), Point(iPos1, iAnzahl_zeilen),Scalar(0,0,0), 1, 8, 0);
					//Zeichne linke Linie von Wort
					//line(matReturnBild,Point(iPos2-1, 0), Point(iPos2-1, iAnzahl_zeilen),Scalar(0,0,0), 1, 8, 0);*/
					
					//Setze Schnittstellen
					iCutPos[iZaehler] = iPos1;
					iCutPos[iZaehler+1] = iPos2;

					iZaehler = iZaehler + 2;
				}

				//linke Linie von Spalte wurde gesetzt
				bLine = true;
			}	
		}
		
		//Setzt rechte Linie von Spalte, wenn ALLE Pixel in Spalte weiß sind
		if((iPixelzaehler == 0) && (bLine == true))
		{
			//Setze iPos1 um später Abstand zwischen Buchstaben zu berechnen
			iPos1 = spalte;

			//rechte Linie von Spalte wurde gesetzt
			bLine = false;
		}
	}
	//Letzer Schnitt für letzes Wort ist am Ende der Matrix
	iCutPos[iZaehler] = iAnzahl_spalten;

	//Wörter werden Ausgelesen und gespeichert
	for(int i = 0; i <= iZaehler-1; i++)
	{

		Set_Spalten_Matrix(matReturnBild, iCutPos[i]-SPALTE_SEPARIEREN_TOLERANZ,iCutPos[i+1]+SPALTE_SEPARIEREN_TOLERANZ);
	}

	//Setze iCutPos wieder auf NULL
	for(int i = 0; i <= 99; i++)
	{
		iCutPos[i] = 0;
	}

	return matReturnBild;
}

//---------ANFANG: Unterfunktion von Bild::separiereZeilen()----------
	//Erstellt Matrix von Zeile
	Mat Bild::Set_Zeilen_Matrix(Mat &originalBild_, int iPoint_Anfang_Zeile, int iPoint_Ende_Zeile)
	{
		int iAnzahl_zeilen = originalBild_.rows;
		int iAnzahl_spalten = originalBild_.cols;
	
		//Zeilenhöhe entspricht "höchstem" Symbol
		int iZeilenhöhe = iPoint_Ende_Zeile - iPoint_Anfang_Zeile + 1;

		Mat matZeilen_Matrix (iZeilenhöhe,iAnzahl_spalten, CV_8UC1);

		for(int zeile=iPoint_Anfang_Zeile; zeile<=iPoint_Ende_Zeile; zeile++)
		{
			for(int spalte=0; spalte < iAnzahl_spalten; spalte++)
			{
				 matZeilen_Matrix.at<uchar>(zeile-iPoint_Anfang_Zeile, spalte) = originalBild_.at<uchar>(zeile, spalte);
			}
		}
		return matZeilen_Matrix;
	}
	//Prüft ob Matrix eine Nullmatrix ist
	bool Bild::empty(Mat &originalBild)
	{
		int iAnzahl_zeilen = originalBild.rows;
		int iAnzahl_spalten = originalBild.cols;

		for(int spalte=0; spalte < iAnzahl_spalten; spalte++)
		{
			for(int zeile=0; zeile < iAnzahl_zeilen; zeile++)
			{
				if(originalBild.at<uchar>(zeile, spalte) == 0)
				{
					return false;
		
				}
			}
		}
		return true;
	}

	//Erstellt Matrix von Wörtern und leerzeichen
	Mat Bild::Set_Spalten_Matrix(Mat &originalBild, int iPoint_Anfang_Spalte, int iPoint_Ende_Spalte)
	{
		int iAnzahl_zeilen = originalBild.rows;

		//Breite des Wortes
		int iWortbreite = iPoint_Ende_Spalte - iPoint_Anfang_Spalte + 1;

		Mat matZeilen_Matrix (iAnzahl_zeilen, iWortbreite, CV_8UC1);

		for(int spalte=iPoint_Anfang_Spalte; spalte < iPoint_Ende_Spalte; spalte++)
		{
			//Weiße Spalte am Anfang einfügen. ABER: iWortbreite +2*SPALTE_SEPARIEREN_TOLERANZ
			/*
			if (spalte==iPoint_Anfang_Spalte || iPoint_Anfang_Spalte==iPoint_Ende_Spalte)
			{
				for(int zeile=0; zeile <iAnzahl_zeilen; zeile++)
			}
			*/

			for(int zeile=0; zeile <iAnzahl_zeilen; zeile++)
			{
				 matZeilen_Matrix.at<uchar>(zeile, spalte-(iPoint_Anfang_Spalte)) = originalBild.at<uchar>(zeile, spalte);
			}
		}
		//Wort wird in globalem Vektor gespeichert
		vecMatGlobal.push_back(matZeilen_Matrix);

		return matZeilen_Matrix;
	}

	//Schneidet am Anfang und Ende der Zeile jeweils den weißen Bereich ab
	Mat Bild::cutSpalte(Mat &originalBild)
	{
		int iAnzahl_zeilen = originalBild.rows;
		int iAnzahl_spalten = originalBild.cols;

		int iPos1 = 0;
		int iPos2 = 0;

		//Mat von links auslesen und linke Grenze in iPos1 speichern
		for(int spalte=0; spalte < iAnzahl_spalten; spalte++)
		{
			for(int zeile=0; zeile < iAnzahl_zeilen; zeile++)
			{
				if(originalBild.at<uchar>(zeile, spalte) == 0)
				{
					//Spalten-Koordinate von erstem schwarzen Pixel wird in iPos1 gespeichert
					iPos1 = spalte-SPALTE_SEPARIEREN_TOLERANZ;
					//Schleife verlassen sobald der erste schwarze Pixel gefunden ist
					break;
				}
			
			}
			//Schleife verlassen sobald der erste schwarze Pixel gefunden ist
			if(iPos1 != 0) break;
		}

		//Mat von rechts auslesen und rechte Grenze in iPos2 speichern
		for(int spalte=iAnzahl_spalten-1; spalte >= 0; spalte--)
		{
			for(int zeile = 0; zeile < iAnzahl_zeilen; zeile++)
			{
				if(originalBild.at<uchar>(zeile, spalte) == 0)
				{
					//Spalten-Koordinate von letzten schwarzen Pixel wird in iPos2 gespeichert
					iPos2 = spalte+1+SPALTE_SEPARIEREN_TOLERANZ;
					//Schleife verlassen sobald der erste schwarze Pixel gefunden ist
					break;
				}
			
			}
			//Schleife verlassen sobald der erste schwarze Pixel gefunden ist
			if(iPos2 != 0) break;
		}

		//Neue Anzahl von Spalten
		int iAnzahl_spalten_neu = iPos2 - iPos1 + 1;

		//Erstelle neue Matrix 
		Mat matReturnBild (iAnzahl_zeilen, iAnzahl_spalten_neu, CV_8UC1);

		//Kopiere originalBild(ohne weißen Rand) in matReturnBild
		for(int spalte=iPos1; spalte < iPos2; spalte++)
		{
			for(int zeile = 0; zeile < iAnzahl_zeilen; zeile++)
			{
				matReturnBild.at<uchar>(zeile, spalte-iPos1) = originalBild.at<uchar>(zeile, spalte);
			}
		}
		return matReturnBild;
	}


//---------ENDE: Unterfunktion von Bild::separiereZeilen()----------


//Zeilen in einzelne Matrizen zerlegen -->auch Wörter?!
void Bild::separiereZeilen(Mat matEingabeBild)
{





































}

//Text auf einer Wort/Zeilen-Matix erkennen		//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! Zu untersuchende Matrix soll sich diese Funktion nach Möglichkeit von einer queue (<- damit Reihenfolge der Wörter besetehen bleibt) holen können, welche in Bild::separiereZeilen() erzeugt werden soll
string Bild::erkenneText(Mat &matWort)
{
	//erkannten Text löschen für neues WOrt
	ssErkannterText.str("");

	//Variablendeklarationen
	int iTemplateNr = 0;	//Zählervariable für Templates mit 0 initialisieren
	int iMatchMethod = TM_SQDIFF_NORMED;	//Bei Tests hat TM_SQDIFF_NORMED das beste Ergebnis geliefert.


	//-----------MATCHING_METHODE ANFANG------------
	//Ermittelt alle Matches

	//Zur Speicherung der MatchErgebnisse
	//structMatch structMatchDaten;

	//Variablenn zur Zwischenspeicherung der Größe der Resultmatrix deklarieren und initialisieren.
	int iResultMatrixCols = 0;
	int iResultMatrixRows = 0;

	//Für jedes Template die matchMatrix erzeugen und auf großem TemplateStapel ablegen
	for (iTemplateNr=0; iTemplateNr<=iTemplateCounter-1; iTemplateNr++)
	{
		//matchMatrizengröße berechnen
		iResultMatrixCols =  matWort.cols - vectorStructTemplates[iTemplateNr].matTemplate.cols + 1;
		iResultMatrixRows = matWort.rows - vectorStructTemplates[iTemplateNr].matTemplate.rows + 1;

		if (iResultMatrixCols>0 && iResultMatrixRows>0)
		{
					vectorStructTemplates[iTemplateNr].matMatchResult.create( iResultMatrixCols, iResultMatrixRows, CV_32FC1 );	//!!!!!!!!!!!!!!stürzt ab bei negativen Werten für iResultMatrixCols bzw. iResultMatrixRows

				/// Do the Matching and Normalize
				matchTemplate( matWort, vectorStructTemplates[iTemplateNr].matTemplate, vectorStructTemplates[iTemplateNr].matMatchResult, iMatchMethod);
				normalize( vectorStructTemplates[iTemplateNr].matMatchResult, vectorStructTemplates[iTemplateNr].matMatchResult, 0, 1, NORM_MINMAX, -1, Mat() );

				// For SQDIFF and SQDIFF_NORMED, the best matches are lower values. For all the other methods, the higher the better
				if( iMatchMethod  == CV_TM_SQDIFF || iMatchMethod == CV_TM_SQDIFF_NORMED || iMatchMethod == TM_CCORR)
				{
					vectorStructTemplates[iTemplateNr].matMatchResult = 1.0 - vectorStructTemplates[iTemplateNr].matMatchResult;
				}
	
			//Für jedes Template alle Maxima ermitteln und speichern
			ermittleLokaleMaximaDerMatchMatrizen(vectorStructTemplates[iTemplateNr].matMatchResult, vectorStructTemplates[iTemplateNr].vectorBestMatches, iTemplateNr);

			//MaximaPunkte von links nach rechts sortieren, d.h. die Maxima liegen so, dass das erste Maximum eines Templates von links aus hinten im Vektor liegt (d.h. rausholen über pop())
			stable_sort(vectorStructTemplates[iTemplateNr].vectorBestMatches.begin(), vectorStructTemplates[iTemplateNr].vectorBestMatches.end(), &Bild::compareVectorOfStructMatch );
		}
	/*
				//Debugzwecke: Ausgabe der sortierten Werte in Konsole zum Überprüfen, obs klappt.
				structMatchVectorIteratorType iteratorStructMatchVector;
				for (iteratorStructMatchVector = vectorStructTemplates[iTemplateNr].vectorBestMatches.begin(); iteratorStructMatchVector != vectorStructTemplates[iTemplateNr].vectorBestMatches.end(); iteratorStructMatchVector++)
				{
					cout << iteratorStructMatchVector->point.x << " (" << iteratorStructMatchVector->matchRate << ")" <<", ";
				}
				cout << endl << endl;
	
				//Debugzwecke
				vectorStructTemplates[iTemplateNr].iWieHaeufigGefunden=0;

	*/
	}
	//-----> nun befindet sich im templateVector zu jedem Template ein vector mit möglichen Matches.
	

		//Debugzwecke:
		int iGefundeneSymboleGesamt = 0;

	while ( this->anyMatchesLeft() )
	{
		iGefundeneSymboleGesamt++;

		//erste signifikante Übereinstimmung eines Templates im Bild ermitteln.
			//Merkt sich immer das Template, das die erste Übereinstimmung liefert
			structMatch structFirstMatch;
					structFirstMatch.templateNr = -1;
					structFirstMatch.point = Point(matWort.cols+1,matWort.rows+1);
					structFirstMatch.matchRate = -1;
			//Zählervariable für TemplateVektor definieren und erzeugen
			vectorStructTemplateIteratorType iteratorTemplateVector;
			for( iteratorTemplateVector = vectorStructTemplates.begin(); iteratorTemplateVector != vectorStructTemplates.end(); iteratorTemplateVector++)
			{
				//Prüfen, ob aktuelles Template noch auf dem Bild erkannt wird UND wenn ja, dann auch noch weiter links als das bisheriges? (->Reihenfolge der AND Verknüpfung beachten!)
					//--> Template merken.
				if( (!iteratorTemplateVector->vectorBestMatches.empty()) && (iteratorTemplateVector->vectorBestMatches.back().point.x <= structFirstMatch.point.x))
				{
					structFirstMatch = iteratorTemplateVector->vectorBestMatches.back();
				}
			}
			//-----> nun beinhaltet structFirstMatch alle Informationen zu der ersten Übereinstimmung im Bild

			
			//Als nächstes werden alle anderen Übereinstimmungen aus dem TemplateVector geholt, die im Bereich xmin bis x_min+matTemplate[structFirstMatch.templateNr].cols-1 liegen,
			//verglichen, und dasjenige mit der besten MatchRate wird als erkanntes Template gewählt. Alle anderen Punkte werden während der Schleife direkt aus dem TemplateVector entfernt
			structMatch structBestMatch;
					structBestMatch.templateNr = -1;
					structBestMatch.point = Point(matWort.cols+1,matWort.rows+1);
					structBestMatch.matchRate = -1;
			for( iteratorTemplateVector = vectorStructTemplates.begin(); iteratorTemplateVector != vectorStructTemplates.end(); iteratorTemplateVector++)
			{
				structMatchVectorIteratorType iteratorStructMatchVector;
				int iGoodMatchesCounter = 0;
				for( iteratorStructMatchVector=iteratorTemplateVector->vectorBestMatches.begin(); iteratorStructMatchVector!=iteratorTemplateVector->vectorBestMatches.end(); iteratorStructMatchVector++) //rückwärts, da sowieso nur die hinteren Vektorelemente in zu prüfendem Bereich liegen. Dann kann man da auch anfangen und sobald der erste Match außerhalb des gesuchten Bereichs liegt, folgt aufgrund der Sortierung, dass alle nachfolgenden auch außerhalb liegen -> Abbruch der inneren For-Schleife.
				{
				
					//Prüfen, ob aktuelles Template im gesuchten Bereich liegt? --> Template merken.
					if(    (iteratorStructMatchVector->point.x >= structFirstMatch.point.x )															
					    && (iteratorStructMatchVector->point.x < structFirstMatch.point.x+vectorStructTemplates[structFirstMatch.templateNr].matTemplate.cols-ZEICHENUEBERLAPPUNG_TOLERANZWERT) )
					{
						//matchRate ist echtgrößer als die desjenigen Templates, in dessen Bereich gesucht wird -> Template merken
						if (iteratorStructMatchVector->matchRate > structBestMatch.matchRate)
						{
							structBestMatch = *iteratorStructMatchVector;
						}

						 iGoodMatchesCounter++;
						//>>>>>>Eigentlich würde hier das entsprechende Element gepopt werden, aber das geht wegen der for-Schleife nicht.
						//Darum mitzählen, wie viele im kritischen Bereich lagen werden müssen und nach Beendigung der Schleife Matches entfernen.<<<<<<<
					}
					//Aufgrund der Sortierung des Vektors folgen keine Matches mehr im betrachteten Bereich, wenn einmal einer außerhalb lag.
				}

				//Alle Matches, die im kritischen Bereich lagen, werden hier nun gelöscht.
				for (int i=iGoodMatchesCounter; i>0; i--)
				{
						iteratorTemplateVector->vectorBestMatches.pop_back();
				}
				 
			}
			//------> nun befinden sich in structBestMatch die Informationen über dasjenige Template, das mit höchster Wahrscheinlichkeit den ersten Buchstaben bildet.	


				//Debugzwecke: Ausgabe firstMatch-Informationen
				cout << endl << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
				cout << " Debuginformationen zum " << iGefundeneSymboleGesamt << ". gefundenen Symbol:" << endl << endl;
				cout << "   structFirstMatch-Daten:" << endl
					 << "    symbol: " << vectorStructTemplates[structFirstMatch.templateNr].symbol << "\t(templateNr: " << structFirstMatch.templateNr << ")" << endl
					 << "    starting point: " <<  structFirstMatch.point.x << "   (ending point: " << structFirstMatch.point.x+vectorStructTemplates[structFirstMatch.templateNr].matTemplate.cols << ")" << endl
					 << "    matchRate: " <<  structFirstMatch.matchRate << endl << endl;
				//Debugzwecke: Ausgabe firstMatch-Informationen
				cout << "   structBestMatch-Daten:" << endl
					 << "    symbol: " << vectorStructTemplates[structBestMatch.templateNr].symbol << "\t(templateNr: " << structBestMatch.templateNr << ")" << endl
					 << "    starting point: " <<  structBestMatch.point.x << "   (ending point: " << structBestMatch.point.x+vectorStructTemplates[structBestMatch.templateNr].matTemplate.cols << ")" << endl
					 << "    matchRate: " <<  structBestMatch.matchRate << endl;
				cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl << endl;


		//Außerdem darf im Bereich des bestMatch-Templates ebenfalls kein neues Template beginnen, weshlab eine ähnliche Funktion hier nun die entsprechenden Punkte löscht.
			//dazu wird nun nochmal von vorne jeder location vector nach einem entsprechenden Elementen durchsucht und mit dem bestMatch verglichen
		for( iteratorTemplateVector = vectorStructTemplates.begin(); iteratorTemplateVector != vectorStructTemplates.end(); iteratorTemplateVector++)
		{
					structMatchVectorIteratorType iteratorStructMatchVector;
					int iGoodMatchesCounter = 0;
					for( iteratorStructMatchVector=iteratorTemplateVector->vectorBestMatches.begin(); iteratorStructMatchVector!=iteratorTemplateVector->vectorBestMatches.end(); iteratorStructMatchVector++) //rückwärts, da sowieso nur die hinteren Vektorelemente in zu prüfendem Bereich liegen. Dann kann man da auch anfangen und sobald der erste Match außerhalb des gesuchten Bereichs liegt, folgt aufgrund der Sortierung, dass alle nachfolgenden auch außerhalb liegen -> Abbruch der inneren For-Schleife.
					{
				
						//Prüfen, ob aktuelles Template im gesuchten Bereich liegt? --> Template merken.
						// !!!!!!!!!!!!! HINWEIS: WENN SICH BUCHSTABEN BEI SCHRIFTART ÜBERLAPPEN KÖNNEN, MUSS DER BEREICH HIER UNBEDINGT VERKÜRZT WERDEN!!! ANDERNFALLS WERDEN BUCHSTABEN ÜBERSPRUNGEN. !!!!!!!!!!!!!
						if(    (iteratorStructMatchVector->point.x >= structBestMatch.point.x )															
							&& (iteratorStructMatchVector->point.x < structBestMatch.point.x+vectorStructTemplates[structBestMatch.templateNr].matTemplate.cols-ZEICHENUEBERLAPPUNG_TOLERANZWERT) )
						{
							 iGoodMatchesCounter++;
							//>>>>>>Eigentlich würde hier das entsprechende Element gepopt werden, aber das geht wegen der for-Schleife nicht.
							//Darum mitzählen, wie viele im kritischen Bereich lagen werden müssen und nach Beendigung der Schleife Matches entfernen.<<<<<<<
						}
						//Aufgrund der Sortierung des Vektors folgen keine Matches mehr im betrachteten Bereich, wenn einmal einer außerhalb lag.
					}

					for (int i=iGoodMatchesCounter; i>0; i--)
					{
							iteratorTemplateVector->vectorBestMatches.pop_back();
					}
		}



		//Buchstaben des Matches an erkannte Buchstabenliste anhängen
		ssErkannterText << vectorStructTemplates[structBestMatch.templateNr].symbol;
		
		//Makieren des ersten Buchstabens in eingabeBild bzw. einer Kopie davon für grafische Ausgabe
		//structBestMatch in ausgabeBild einzeichnen
		Point pointBestMatch = structBestMatch.point;
		int iHoeheTemplate = vectorStructTemplates[structBestMatch.templateNr].matTemplate.rows;
		int iBreiteTemplate = vectorStructTemplates[structBestMatch.templateNr].matTemplate.cols;
		rectangle( matWort, pointBestMatch, Point( pointBestMatch.x + iBreiteTemplate , pointBestMatch.y + iHoeheTemplate ), Scalar::all(0), 2, 8, 0 );
		rectangle( vectorStructTemplates[structBestMatch.templateNr].matMatchResult, pointBestMatch, Point( pointBestMatch.x + iBreiteTemplate , pointBestMatch.y + iHoeheTemplate ), Scalar::all(0), 2, 8, 0 );
			

				//Debugzwecke...
				vectorStructTemplates[structBestMatch.templateNr].iWieHaeufigGefunden++;

				//Debugzwecke: Ausgabe der Variablenwerte
				/* //oben neue Ausgabe für Variablenwert eingefügt
				cout << endl << "Symbol: " << vectorStructTemplates[structFirstMatch.templateNr].symbol << " (FirstMatch.templateNr=" << structFirstMatch.templateNr << ")" << endl
					 << "FirstMatch.x=" << structFirstMatch.point.x << " Breite_des_Templates=" << iBreiteTemplate << " Ende_Template=" <<  structFirstMatch.point.x+iBreiteTemplate << endl
					 << "Symbol: " << vectorStructTemplates[structBestMatch.templateNr].symbol <<" (BestMatch.templateNr=" << structBestMatch.templateNr << ")" << endl
					 << "BestMatch.x=" << structBestMatch.point.x << " BestMatch.rate=" << structBestMatch.matchRate << endl << endl;
					 */


	}
	//Da die Punkte, die sich mit diesem überlagern würden und jenem Maximum selbst, gelöscht wurden, kann durch wiederholte Ausführung der Schleife sukzessive
	//das gesamte Wort analysiert werden.
		

				//Debugzwecke:
				cout << "----------------------------------------------" << endl;

				/*
				//Debugzwecke:
					//Ergebnis der matchFunktionen grafisch ausgeben (geht später auch etwas schöner in einer Schleife)
				for (iTemplateNr=0; iTemplateNr<iTemplateCounter-1; iTemplateNr++)
				{
					cout << "Gefundene " << vectorStructTemplates[iTemplateNr].symbol << "'s: \t" << vectorStructTemplates[iTemplateNr].iWieHaeufigGefunden << endl
						 << "Match Methode: \t" << iMatchMethod << endl << endl;
					int test=vectorStructTemplates.size();
					//eingabeBild ausgeben, wobei die erkannten Zeichen umrahmt werden
					if (RESULTMATRIZEN_GRAFISCH_AUSGEBEN==true)
					{
						imshow( vectorStructTemplates[iTemplateNr].dateinameDesTemplates, vectorStructTemplates[iTemplateNr].matMatchResult);
					}
				}
				*/

	
				//Debugzwecke:
				cout << "----------------------------------------------" << endl << endl;

		//Erkannte Zeichenfolge ausgeben
		cout << "Texterkennung: " << ssErkannterText.str() << endl;
	//-----------MATCHING_METHODE ENDE------------


	//Fenster erstellen
    //namedWindow( "erkenneText_EingabeBild", CV_WINDOW_AUTOSIZE );
	//Eingabebild mit erkannten Templates anzeigen
	//imshow( "erkenneText_EingabeBild", matWort );


   //waitKey(0);

	return ssErkannterText.str();
}


//---------ANFANG: Unterfunktionen von Bild::erkenneText()----------
	//Um den Vektor mit den Maximas mittels verfügbarer Sotierfunktionen sortieren zu können, wird eine Callback-Funktion benötigt, die den Vergleich zweier structs definiert
	bool Bild::compareVectorOfStructMatch(const structMatch &structMatchA, const structMatch &structMatchB)
{
	return (structMatchA.point.x > structMatchB.point.x);
};

	//Funktion zählt, wie viele mögliche (signifikante) Matches es noch insgesamt gibt und gibt diesen Wert als integer zurück
	bool Bild::anyMatchesLeft()
	{
		vectorStructTemplateIteratorType iteratorTemplateVector;
		//von jedem Template die verbleibenden MatchVektoren auf "Leerheit" prüfen. Sobald auch nur ein Match in einem Template vorhanden ist, gilt selbstverständlich: anyMatchesLeft==true, was zu einem vorzeitigen Beenden der Schleife führt.
		for( iteratorTemplateVector = vectorStructTemplates.begin(); iteratorTemplateVector != vectorStructTemplates.end(); iteratorTemplateVector++)
		{
			if ( !iteratorTemplateVector->vectorBestMatches.empty() ) 
			{
				return true;
			}
		}

		return false;
	}

	void Bild::ermittleLokaleMaximaDerMatchMatrizen(const Mat& matMatchResult, vector< structMatch >& vectorStructBestMatches, const int iTemplateNr)
	{
		float fLastValue = -1.0f * numeric_limits<float>::max();
		float* fpSrcData = reinterpret_cast<float*>(matMatchResult.data);

		//struct deklarieren
		structMatch structMatchDaten;
		structMatchDaten.templateNr = iTemplateNr;

		//Sorgt dafür, dass nach einem Maximum ein wenig gewartet wird, bevor das nächste gefunden werden kann. (<=> Entprellung)
		int iWaitAfterMaximum = ((int) 0.5*(vectorStructTemplates[iTemplateNr].matTemplate.cols))+1;

		bool bPositiveSteigung = false;

		for(int i = 0; i < matMatchResult.rows; i++)
		{
			for(int j = 0; j < matMatchResult.cols; j++)
			{

				if ( (bPositiveSteigung) || (iWaitAfterMaximum>=0.5*(vectorStructTemplates[iTemplateNr].matTemplate.cols)))

					//neuer Maximalwert wird auf queue abgelegt. Nebenbedingung: Übereinstimmung liegt bei über 80%.
					if ( (fpSrcData[i*matMatchResult.cols + j] >= fLastValue) && (fpSrcData[i*matMatchResult.cols + j]) > MATCHRATE_THRESHOLD_VALUE )
					{
						//Wenn die Werte aktuell monoton steigen, muss der letzte Wert immer wieder aus der queue gelöscht werden
						if (bPositiveSteigung)
							vectorStructBestMatches.pop_back();

						//struct mit aktuellen Werten füllen
						structMatchDaten.point = Point(j, i);
						structMatchDaten.matchRate = fpSrcData[i*matMatchResult.cols + j];

						//Struct an vector hängen
						vectorStructBestMatches.push_back(structMatchDaten);

						bPositiveSteigung = true;

						//Spalte des letzten Maximums merken.
						iWaitAfterMaximum = -1;
					}

					if (fpSrcData[i*matMatchResult.cols + j] < fLastValue)
					{
						bPositiveSteigung = false;
					}

				iWaitAfterMaximum++;
				fLastValue = fpSrcData[i*matMatchResult.cols + j];

			}
		}
	}
//---------ENDE: Unterfunktionen von Bild::erkenneText()----------

