#include <iostream>
#include <math.h>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;

//ovo je kod kojim se ra�una gradijent slike preko Sobelovog operatora

Mat SobelovOperator(Mat &Slika,int x, int y) { //prvi argument je referenca na sliku koju �aljemo na obradu, drugi argument je za odabir gradijenta po x osi, tre�i argument je za odabir gradijentaa po y osi

	int Retci = Slika.rows;  //Dobivanje �irine i visine slike u broju piksela
	int Stupci = Slika.cols;

	if (x) {  //RA�UNANJE GRADIJENTA ZA X OS

		int* MatricaPom = new int [Retci * Stupci](); //stvaranje pomo�ne matrice za spremanje me�uvrijednosti

		int kernelX2[3] = {1, 2, 1};  // iznosi separiranih x kernela Sobelovog operatora
		int kernelX1[3] = {1, 0, -1};

		int X = 0, Y = 0, indeks = 0;

		int i, j, k;

		double najveci = 0, najmanji = 1000000;  // stavljanje nemogu�ih vrijednosti za ekstreme koji �e biti bitni kasnije (za svaki slu�aj) :)

		for (i = 0; i < Retci; i++) {  // ra�unanje prve kovolucije
			const unsigned char* dx1 = (const unsigned char*) (Slika.data + i *Slika.step);  //pristupanje elementima preko pointera (radi brzine izvo�enja)
			for ( j = 0; j < Stupci; j++) {
				for ( k = -1; k < 2; k++) { 

					X = j + k;

					if (X < 0) {
						X= Stupci - 1;
					}
					else if (X >= Stupci) {
						X = 2 * Stupci - X - 1;
					}

					MatricaPom[i * Stupci + j] += dx1[X] * kernelX1[k+1];  //mno�enje vrijednosti piksela sa prvim separiranim kernelom
				}
			}
		}

		Mat GradX(Size(Stupci, Retci), CV_32S);  //stvaranje matrice gradijenata po x osi

		int Gx(0);

		for (i = 0; i < Retci; i++) {  // ra�unanje druge konvolucije
			int* dx2 = (int*)(GradX.data + i * GradX.step); 
			for ( j = 0; j < Stupci; j++) {
				Gx = 0;
				for ( k = -1; k < 2; k++) { 

					X = i + k;

					if (X < 0) {
						X= Retci - 1;
					}
					else if (X >= Retci) {
						X = 2 * Retci - X - 1;
					}

					Gx += MatricaPom[X * Stupci + j] * kernelX2[k+1];  //mno�enje vrijednosti piksela sa drugim separiranim kernelom
				}
		
				dx2[j] = Gx;
			}
		}

		delete[] MatricaPom;  //osloba�anje memorije

		return GradX;
	}

	else if (y) {  //RA�UNANJE GRADIJENTA ZA Y OS

		int* MatricaPom = new int [Retci * Stupci]();  //stvaranje pomo�ne matrice za spremanje me�uvrijednosti

		int kernelY2[3] = {1, 0, -1};  // iznosi separiranih x kernela Sobelovog operatora
		int kernelY1[3] = {1, 2, 1};

		int X = 0, Y = 0, indeks = 0;
		int i, j, k;

		double najveci = 0, najmanji = 1000000;  // stavljanje nemogu�ih vrijednosti za ekstreme koji �e biti bitni kasnije (za svaki slu�aj) :)

		for (i = 0; i < Retci; i++) {  //ra�unanje prve konvolucije
			const unsigned char* dx1 = (const unsigned char*) (Slika.data + i *Slika.step);  //pristupanje elementima preko pointera (radi brzine izvo�enja)
			for ( j = 0; j < Stupci; j++) {
				for ( k = -1; k < 2; k++) { 

					X = j + k;

					if (X < 0) {
						X= Stupci - 1;
					}
					else if (X >= Stupci) {
						X = 2 * Stupci - X - 1;
					}

					MatricaPom[i * Stupci + j] += dx1[X] * kernelY1[k+1];  //mno�enje vrijednosti piksela sa prvim separiranim kernelom
				}
			}
		}

		Mat GradY(Size(Stupci, Retci), CV_32S);  //stvaranje matrice gradijenata po y osi

		int Gy(0);

		for (i = 0; i < Retci; i++) {  // ra�unanje druge konvolucije
			int* dx2 = (int*) (GradY.data + i * GradY.step);
			for ( j = 0; j < Stupci; j++) {
				Gy = 0;
				for ( k = -1; k < 2; k++) { 
	
					X = i + k;

					if (X < 0) {
						X= Retci - 1;
					}
					else if (X >= Retci) {
						X = 2 * Retci - X - 1;
					}

					Gy += MatricaPom[X * Stupci + j] * kernelY2[k+1];  //mno�enje vrijednosti piksela sa drugim separiranim kernelom
				}
		
				dx2[j] = Gy;
			}
		}

		delete[] MatricaPom; //osloba�anje memorije

		return GradY;
	}

	else {
		cout << "Greska odabira nacina rada (Sobel)!!!\n";
		exit(1);
	}
}
