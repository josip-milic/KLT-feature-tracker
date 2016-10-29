#include <iostream>
#include <math.h>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;

Mat GaussFiltar(Mat &Sl, double sigma) {

	int velicina = 2 * floor(sigma * 3) + 1;  //raèuna se duljina vektora sa sigmom preko formule od profesora

	if (sigma < 0) {
		sigma = 0.3 * ((velicina - 1) * 0.5 - 1) + 0.8;  // ako je sigma manja od nule ovom formulom se sreðuje taj problem
	}

	double* kernel = new double [velicina];  // stvaranje vektora izraèunate duljine

	int vel = velicina / 2;  // raèunanje polovine duljine vektora (bitno za kasnije)
	
 	int a, b = 0;

	double sum = 0;

	for (a = vel; a > 0; a--) {
		kernel[b] = kernel[velicina - b - 1] = (1/sqrt(2 * 3.14159 * sigma * sigma)) * exp(-(a * a) / (2 * sigma * sigma));  // raèunaju se iznosi Gaussove funkcije preko formule za 1D kernel u toèkama sa cijelim brojevima (u podruèju 3 sigma preciznosti) po dva odjednom (moguæe radi simetrije vektora)
		sum += 2 * kernel[b];  // raèuna se suma iznosa svih èlanova u vektoru
		b++;
	}

	kernel[b] = (1/sqrt(2 * 3.14159 * sigma * sigma));  //raèunanje središnjeg èlana

	sum += kernel[b];

	if (sum != 1) {  // ako je ukupna suma nije 1 onda se svaki èlan dijeli sa sumom da ukupna suma iznosi 1 (ako nije 1 tada slika ispada ili presvjetla ili pretamna)
		for (a = 0; a > velicina; a++) {
			kernel[a] = kernel[a] / sum;
		}
		kernel[b] = kernel[b] / sum;
	}

	int Ret = Sl.rows;
	int Stup = Sl.cols;

	unsigned char* MatricaPomocna = new unsigned char [Ret * Stup]();

	double suma;

	int pom1, k;

	for (int i = 0; i < Ret; i++) {  // raèunanje prve konvolucije vektora i originalne slike po x-osi (radi performansi)
		
		const unsigned char* dx1 = (const unsigned char*)(Sl.data + i * Sl.step);  // izravni pristup èlanovima preko pointera (radi brzine)
		
		for ( int j = 0; j < Stup; j++) {
			for (k = -vel; k < vel; k++) {
				
				pom1 = j + k;

				if (pom1 < 0) {  // ako je piksel na bilo kojem od rubova slike ovime se uzimaju vrijednosti sa druge strane umjesto "praznine"
					pom1 = Stup + k;
				}
				if (pom1 >= Stup) {
					pom1 = 2 * Stup - pom1 - 1;
				}
				else MatricaPomocna[i * Stup + j] += dx1[pom1] * kernel[k + vel];  //"punjenje" pomoæne matrice sa prvom konvolucijom
			}
		}
	}

	for (int i = 0; i < Ret; i++) {  // raèunanje druge konvolucije vektora sa prvom konvulacijom po y-osi
		unsigned char* dx2 = (unsigned char*)(Sl.data + i * Sl.step);
		for ( int j = 0; j < Stup; j++) {
			suma = 0;
			for (k = -vel; k < vel; k++) {

				pom1 = i + k;

				if (pom1 < 0) {
					pom1 = Ret + k;
				}
				if (pom1 >= Ret) {
					pom1 = 2 * Ret - pom1 - 1;
				}
				else suma += MatricaPomocna[pom1 * Stup + j] * kernel[k + vel];  
			}
			dx2[j] = suma;  // popunjavanje završne matrice koju vraæa funkcija
		}
	}

	delete[] MatricaPomocna;  //oslobaðanje memorije

	return Sl;
};