#include <iostream>
#include <fstream>
#include <math.h>
#include <vector>
#include <opencv2/opencv.hpp>
#include "ObradaSlike.h"
#include "PomocneKlase.h"
using namespace std;
using namespace cv;

Mat GX(Mat &Slika) {  // raèuna gradijent slike po x osi preko formule gx = (m[i][j+1] - m[i][j-1]) / 2

	int Retci = Slika.rows;
	int Stupci = Slika.cols;

	Mat Povratna(Size(Stupci,Retci), CV_32F);

	int X;

	for (int i = 0; i < Retci; i++) {

		const float* pom = (const float*)(Slika.data + i * Slika.step);
		float* pom2 = (float*)(Povratna.data + i * Povratna.step);

		for (int j = 0; j < Stupci; j++) { 
			
			X = j;

			if (X == 0) {
				X = 1;
			}
			else if (X == Stupci - 1 ) {
				X = Stupci - 2;
			}
			
			pom2[j] = (pom[X + 1] - pom[X - 1]) / 2;
		}
	}

	return Povratna;
};

Mat GY(Mat &Slika) {  // raèuna gradijent slike po y osi preko formule gy = (m[i+1][j] - m[i-1][j]) / 2

	int Retci = Slika.rows;
	int Stupci = Slika.cols;

	Mat Povratna(Size(Stupci,Retci), CV_32F);

	int Y;

	for (int i = 0; i < Retci; i++) {

		float* pom = (float*)(Povratna.data + i * Povratna.step);

		for (int j = 0; j < Stupci; j++) { 

			Y = i;

			if (Y == 0) {
				Y = 1;
			}
			else if (Y == Retci - 1 ) {
				Y = Retci - 2;
			}
			
			const float* pom2 = (const float*)(Slika.data + (Y + 1) * Slika.step);
			const float* pom3 = (const float*)(Slika.data + (Y - 1) * Slika.step);

			pom[j] = (pom2[j] - pom3[j]) / 2;
		}
	}

	return Povratna;
};

vector<piksel> KLT(Mat &Slika1, Mat &Slika2, vector<piksel> Kutevi1, vector<pomaci> &Pomak,int mod, int VelProzor,int BrojIteracija, double Prag) {

	vector<int> nadjeni;  // vektor namijenjen za spremanje broja naðenih karakteristika po iteracijama

	nadjeni.assign(BrojIteracija,0); // "punjenje" vektora sa nulama

	vector<piksel> Kutevi2; //inicijalizacija vektora sa izraèunatim naðenim karakteristikama na drugoj slici 

	VelProzor = 2 * VelProzor + 1;  //raèunanje velièine prozora preko poslanog argumenta

	int nadjeno;  

	pomaci POM;  // objekt sa pomacima (koristi se ako je potrebno)

	for (int i = 0; i < Kutevi1.size(); i++) {  // iteriranje po vektoru sa karakteristikama

		double Ix, Iy;  // iznosi sa širinom i visinom slike

		Ix = Kutevi1[i].DajX();
		Iy = Kutevi1[i].DajY();

		double Ix2, Iy2;

		int k; 

		nadjeno = 0;

		double dP[2];  // inicijalizacija iznosa konvergencije pomaka

		double p[] = {0, 0};  // inicijalizacija pomaka i njegovo postavljanje na 0

		int dalje = 1;  // ovo je zastavica koja služi za prekidanje petlje

		Mat SubProzor2 = BilinearnaInt(Slika1, Ix, Iy, VelProzor);  // prozor karakteristike sa poèetne slike izraèunat bilinearnom interpolacijom

		for (k = 0; k < BrojIteracija && dalje == 1; k++) {

			Ix2 = Ix + p[0];  // raèunanje pomaknute koordinate karakteristike na drugoj slici
			Iy2 = Iy + p[1];

			Mat SubProzor = BilinearnaInt(Slika2, Ix2, Iy2, VelProzor);  // prozor karakteristike sa slike na kojoj tražimo karakteristiku izraèunat bilinearnom interpolacijom

			Mat Greska = SubProzor2 - SubProzor;  // raèunanje razlike izmeðu prozora (matrica greške)

			int R = VelProzor;  // inicijalizacija granica prozora

			int S = VelProzor;

			Mat GradX, GradY;  // inicijalizacija i raèunanje gradijenata po x i y osi u prozoru na slici gdje tražimo karakteristiku

			GradX = GX(SubProzor); 

			GradY = GY(SubProzor);

			double DX2(0), DY2(0), DXY(0);

			for (int i = 0; i < R; i++) {
				const float* pom1 = (const float*)(GradX.data + i * GradX.step);  // raèunanje kvadrata gradijenata za aproksimaciju Hesseove matrice
				const float* pom2 = (const float*)(GradY.data + i * GradY.step);
				for (int j = 0; j < S; j++) {
					DX2 += pom1[j] * pom1[j];
					DY2 += pom2[j] * pom2[j];
					DXY += pom1[j] * pom2[j];
				}
			}

			Mat H(Size(2,2),CV_32F);  // inicijalizacija aproksimacije Hesseove matrice

			double nazivnik = DX2 * DY2 - DXY * DXY;  //raèunanje inverza Hesseove matrice H^-1 = 1 / (DX2 * DY2 - DXY * DXY) * [[DY2 -DXY][-DXY DX2]]

			if (nazivnik == 0.0) {  // provjera da li je nazivnik 0 da ne doðe do greške
				dalje = 0;
				continue;
			}

			double D = 1 / nazivnik; 

			float* H_pom = (float*)(H.data);  // pristupanje elementima preko pointera (to æe se pojavljivati i kasnije)

			H_pom[0] = (float) DY2 * D; 
				
			H_pom[1] = (float) -(DXY * D);

			H_pom = (float*)(H.data + H.step);

			H_pom[0] = (float) -(DXY * D);

			H_pom[1] = (float) DX2 * D;

			Mat T(Size(1,2), CV_32F, Scalar(0));  // stvaranje pomocnog spremnika

			float* T_pom;

			for (int a = 0; a < R; a++) {

				const float* pom1 = (const float*)(GradX.data + a * GradX.step);  // množenje matrice greške sa gradijentima (radi otkrivanja gdje je greška najmanja)
				const float* pom2 = (const float*)(GradY.data + a * GradY.step);
				const float* pom3 = (const float*)(Greska.data + a * Greska.step);

				for (int b = 0; b < S; b++) {

					T_pom = (float*)(T.data);
					T_pom[0] += pom1[b] * pom3[b];
					T_pom = (float*)(T.data + T.step);
					T_pom[0] += pom2[b] * pom3[b];

				}
			}

			H_pom = (float*)(H.data);

			float *T_pom2 = (float*)(T.data);  // stvaranje pomocnog pokazivaca za pristup elementima

			dP[1] = H_pom[0] * T_pom2[0] + H_pom[1] * T_pom[0];  // raèunanje pomaka prozora po y osi

			H_pom = (float*)(H.data + H.step);

			dP[0] = H_pom[0] * T_pom2[0] + H_pom[1] * T_pom[0]; // raèunanje pomaka prozora po x osi

			p[0] += dP[0];  // zbrajanje starog pomaka sa novim korakom pomaka

			p[1] += dP[1];

			if (mod == 1) {  // ako se odabere preko ulaznog argumenta onda se svaki iznos pomaka sprema u objekt sa pomacima
				POM.DodajPomakX(p[0]);
				POM.DodajPomakY(p[1]);
				POM.DodajKorakPomaka(abs(dP[0]) + abs(dP[1]));
			};

			if ((abs(dP[0]) + abs(dP[1])) < Prag) { // ako je pomak manji od graniènog, prekida se sa izvoðenjem
				
				nadjeno = 1;
				dalje = 0;
			}

		}

		if (nadjeno) {  // ako je karakteristika naðena onda se sprema iznos koordinata u povratni vektor
			if (mod == 1) {
				Pomak.push_back(POM);
			}
			piksel Pom(Kutevi1[i].DajX() + p[0], Kutevi1[i].DajY() + p[1]); 
			Kutevi2.push_back(Pom);
			nadjeni[k - 1]++;  // povecanje brojaèa naðenih karakteristika po iteracijama
		}

		else {  // ako karakteristika nije naðena onda se spremaju nemoguæe vrijednosti u povratni vektor (radi njihovog izbacivanja izvan funkcije)
			piksel Pom(-1,-1);
			Kutevi2.push_back(Pom);
		}
	}

	if (mod == 2) {  // ako se odabere preko ulaznog argumenta onda se brojaèi naðenih karakteristika spremaju u datoteku na disk
		
		ofstream datoteka("Nadjeno po iteracijama.csv");

		for (int i = 0; i < 30; i++) {
			datoteka << i + 1 << ";" << nadjeni[i] << endl;
		}

		datoteka.close();
	}

	return Kutevi2;  // vraæanje izraèunatih karakteristika
};