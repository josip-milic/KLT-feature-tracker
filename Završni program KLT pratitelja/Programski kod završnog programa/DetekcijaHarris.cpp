#include <iostream>
#include <math.h>
#include <vector>
#include <opencv2/opencv.hpp>
#include "PomocneKlase.h"
#include "ObradaSlike.h"
using namespace std;
using namespace cv;

std::vector<piksel> DetekcijaKuteva(Mat &Slika, double sigma, int VelProzor, int Prag) {

	Mat GradX, GradY;

	GradX = SobelovOperator(Slika, 1, 0);  // raèunanje gradijenta po x i y osi preko napisane funkcije SobelovOperator

	GradY = SobelovOperator(Slika, 0, 1);

	int Retci = Slika.rows;
	int Stupci = Slika.cols;

	int* gradX = new int [Retci * Stupci];  // ovo su kvadrati gradijenata po X, Y te X i Y zajedno. Bitno za kasnije!
	int* gradY = new int [Retci * Stupci];
	int* gradXY = new int [Retci * Stupci];

	for (int i = 0; i < Retci; i++) {  // raèunanje kvadrata gradijenata

		int* dx1 = (int*)(GradX.data + i * GradX.step);  // izravno pristupanje elementima matrice preko pokazivaèa
		int* dx2 = (int*)(GradY.data + i * GradY.step);

		for (int j = 0; j < Stupci; j++) {
			
			int gX, gY;

			gX = dx1[j];

			gY = dx2[j];

			gradX[i * Stupci + j] = gX * gX;
			gradY[i * Stupci + j] = gY * gY;
			gradXY[i * Stupci + j] = gX * gY;

		}
	}

	int velicina = 2 * floor(sigma * 3) + 1;  // ovo je manje-više kod od Gaussa od prije stavljen ovdje da se odjednom izraèunaju 3 konvolucije (brže od 3 poziva funkcije)

	if (sigma < 0) {
		sigma = 0.3 * ((velicina - 1) * 0.5 - 1) + 0.8;
	}

	double* kernel = new double [velicina];

	int vel = velicina / 2;
	
 	int a, b = 0;

	double sum = 0;

	for (a = vel; a > 0; a--) {
		kernel[b] = kernel[velicina - b - 1] = (1/sqrt(2 * 3.14159 * sigma * sigma)) * exp(-(a * a) / (2 * sigma * sigma));
		sum += 2 * kernel[b]; 
		b++;
	}

	kernel[b] = (1/sqrt(2 * 3.14159 * sigma * sigma));

	sum += kernel[b];

	if (sum != 1) {
		for (a = 0; a > velicina; a++) {
			kernel[a] = kernel[a] / sum;
		}
		kernel[b] = kernel[b] / sum;
	}

	int pom1;
	
	double* Mat1 = new double [Retci * Stupci]();  // pomocni spremnici za konvoluciju kvadrata gradijenata sa Gaussovim kernelom!
	double* Mat2 = new double [Retci * Stupci]();
	double* Mat3 = new double [Retci * Stupci]();

	for (int i = 0; i < Retci; i++) {
		for ( int j = 0; j < Stupci; j++) {
			for (int k = -vel; k < vel; k++) {
				
				pom1 = j + k;

				if (pom1 < 0) {
					pom1 = Stupci + k;
				}
				if (pom1 >= Stupci) {
					pom1 = 2 * Stupci - pom1 - 1;
				}
				else {
					Mat1[i * Stupci + j] += gradX[i * Stupci + pom1] * kernel[k + vel];
					Mat2[i * Stupci + j] += gradY[i * Stupci + pom1] * kernel[k + vel];
					Mat3[i * Stupci + j] += gradXY[i * Stupci + pom1] * kernel[k + vel];
				}
			}
		}
	}

	delete[] gradX;
	delete[] gradY;
	delete[] gradXY;

	double* sX = new double [Retci * Stupci]();  // krajnje izaèunata konvolucija gradijenata i Gaussa je ovdje spremljena
	double* sY = new double [Retci * Stupci]();
	double* sXY = new double [Retci * Stupci]();

	for (int i = 0; i < Retci; i++) {
		for ( int j = 0; j < Stupci; j++) {
			for (int k = -vel; k < vel; k++) {
				
				pom1 = j + k;

				if (pom1 < 0) {
					pom1 = Stupci + k;
				}
				if (pom1 >= Stupci) {
					pom1 = 2 * Stupci - pom1 - 1;
				}
				else {
					sX[i * Stupci + j] += Mat1[i * Stupci + pom1] * kernel[k + vel];
					sY[i * Stupci + j] += Mat2[i * Stupci + pom1] * kernel[k + vel];
					sXY[i * Stupci + j] += Mat3[i * Stupci + pom1] * kernel[k + vel];
				}
			}
		}
	}

	delete[] Mat1;
	delete[] Mat2;
	delete[] Mat3;

	double Det, Trag, lamda1, lamda2, korijen, C;

	std::vector<piksel> lista;  // ovo je spremnik za poèetnu listu naðenih "kuteva"!

	double* mapa = new double [Stupci * Retci]();

	for (int i = 0; i < Retci; i++) {
		for ( int j = 0; j < Stupci; j++) {

			Trag = sX[i * Stupci + j] + sY[i * Stupci + j];  // izraèunat trag matrice [[sX, sXY], [sXY, sY]]

			Det = sX[i * Stupci + j] * sY[i * Stupci + j] - sXY[i * Stupci + j] * sXY[i * Stupci + j];  // determinanta iste te matrice

			korijen = sqrt(Trag * Trag - 4 * Det);

			lamda1 = (Trag + korijen) / 2;  // raèunanje svojstvenih vrijednosti

			lamda2 = (Trag - korijen) / 2;

			C = (lamda1 * lamda2) / (lamda1 + lamda2);  // odaziv kuta (èim je veæi, vjerojatnije je da je kut)

			if (C > Prag) {  // usporedba sa pragom (obièno je prag oko 10, ali može i više)
				mapa[i * Stupci + j] = C;
				piksel Max(i, j, C);
				lista.push_back(Max);
			}
		}
	}

	delete[] sX;
	delete[] sY;
	delete[] sXY;

	std::vector<piksel> kutevi;

	double Iznos;

	for (int i = 0; i < lista.size(); i++) {  // usporeðuju su okolni susjedi (okolica velièine prozora koji je poslan kao argument) u matrici kuteva. Ako je neki susjed veæi od kuta s kojim usporeðujemo, kut ne spremamo!

		int X = lista[i].DajX();
		int Y = lista[i].DajY();
		int Z = lista[i].DajIznos();
		
		Iznos = mapa[X * Stupci + Y];
		int zastavica = 1;

		for (int j = -VelProzor; j < VelProzor; j++) {
			if (zastavica == 0) {
				break;
			}
			for ( int k = -VelProzor; k < VelProzor; k++) {
				if ((X + j) * Stupci + (Y + k) > Retci * Stupci) {
					continue;
				}
				if ((X + j) * Stupci + (Y + k) < 0) {
					continue;
				}
				if (mapa[(X + j) * Stupci + (Y + k)] > Iznos) {
					mapa[X * Stupci + Y] = 0;
					zastavica = 0;
					break;
				}
			}
		}

		if (zastavica) {
			piksel A(X, Y, Z);
			kutevi.push_back(A);  // ako je najveæi u okolici spremamo ga u vektor
		}
	}

	delete[] mapa;

	return kutevi;  // vraæamo vektor kuteva
};