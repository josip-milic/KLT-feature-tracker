#include <iostream>
#include <math.h>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;

float interpoliraj(Mat &Slika, float x, float y) {  // pomoæna funkcija preko koje raèunamo potpikselnu interpolaciju jedne koordinate

	int xt = (int) x;
	int yt = (int) y;

	float ax = x - (int) x;
	float ay = y - (int) y;

	const unsigned char* pom = (const unsigned char*)(Slika.data + xt * Slika.step);

	float XY1 = pom[yt];

	float XY3 = pom[yt + 1];

	pom = (const unsigned char*)(Slika.data + (xt + 1) * Slika.step);

	float XY2 = pom[yt];

	float XY4 = pom[yt + 1];

	return ( (1-ax) * (1-ay) * XY1 +  //raèunanje preko formule za bilinearnu interpolaciju
           ax   * (1-ay) * XY2 +
           (1-ax) *   ay   * XY3 +
           ax   *   ay   * XY4 );
};

Mat BilinearnaInt(Mat &Slika, double piksX = 0, double piksY = 0, int DimProzor = 0) { 

	int Ret = Slika.rows;
	int Stup = Slika.cols; 

	Mat FloatSlika(Size(DimProzor, DimProzor), CV_32F);  //stvaranje matrice slike sa realnim vrijednostima piksela

	float pom1, pom2;
		
	int PolProzora = DimProzor / 2;  //raèunanje duljine polovice prozora (radi for petlja za kasnije)

	for (int i = -PolProzora; i <= PolProzora; i++) {

		float* Pom = (float*)(FloatSlika.data + (i + PolProzora) * FloatSlika.step);  //izravan pristup preko pointera
			
		for (int j = -PolProzora; j <= PolProzora; j++) {
				
			pom1 = i + piksX;
			pom2 = j + piksY;

			if ((int) pom2 >= Stup - 1) {
				pom2 = Stup - 2;
			}

			if ((int) pom1 >= Ret - 1) {
				pom1 = Ret - 2;
			}

			if ((int) pom2 < 0) {
				pom2 = 0;
			}

			if ((int) pom1 < 0) {
				pom1 = 0;
			}

			Pom[j + PolProzora] = interpoliraj(Slika, pom1, pom2);  // interpolacija piksela na realnim koordinatama (potpikselna preciznost)
		}
	}

	return FloatSlika;
}