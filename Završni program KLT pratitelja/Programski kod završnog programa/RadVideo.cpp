#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <opencv2/opencv.hpp>
#include <string>
#include <fstream>
#include <sys/timeb.h>
#include "PomocneKlase.h"
#include "ObradaSlike.h"
#include "DetekcijaKuteva.h"
#include "KLTpracenje.h"
#include "opencv2\highgui\highgui.hpp"
using namespace std;
using namespace cv;

int radVideo() {

	cout<<"\nVIDEO\n\n";

	string ime;

	cout << "Unesite ime videa: ";  // unošenje imena videa i provjera da li video postoji

	cin >> ime;

	ime = ime + ".avi";

	if (FILE *file = fopen(ime.c_str(), "r")) {
		fclose(file);
	}
	else {
		cout<<"\nDatoteka ne postoji!\n";
		destroyWindow("KLT tracking");
        return -1;
    } 

	namedWindow("KLT tracking");  // stvaranje prozora

	int vrijednost = 15;  // inicijalizacija vrijednosti za "slidere"

	int haris = 1000;

	int frame = 0;

	createTrackbar("Znacajke", "KLT tracking", &vrijednost, 100, NULL);  // stvaranje "slidera" u prozoru (prvi je broj karaktristika, drugi je prag za Harrisa, a treæi je sklopka za rad "ruèno frame po frame")
	createTrackbar("Harris", "KLT tracking", &haris, 10000, NULL);
	createTrackbar("Frame", "KLT tracking", &frame, 1, NULL);

	char c = 100;  // unošenje nemoguæe vrijednosti da imamo s neèim usporeðivati

	while (c != 32) {  // èekanje dok se ne pritisne "space"
		c = cvWaitKey();
	}

	Mat source;

    VideoCapture stream(ime);  // otvaranje toka sa videom i provjera otvaranja

    if (!stream.isOpened())
    {
        cout << "\nNe mogu otvoriti video!!!" << endl;
		destroyWindow("KLT tracking");
        return -1;
    }

	double sirina = stream.get(CV_CAP_PROP_FRAME_WIDTH);  // uzimanje podataka o širini videa iz informacija ulazne datoteke

	double visina = stream.get(CV_CAP_PROP_FRAME_HEIGHT);  //  uzimanje podataka o visini videa iz informacija ulazne datoteke

	double fps = stream.get(CV_CAP_PROP_FPS);  // uzimanje podataka o broju slièica u sekundi...

	Size velicina(static_cast<int>(sirina), static_cast<int>(visina));  // Objekt sa dimenzijama za novi video

	ime.erase(ime.end() - 4, ime.end());

	VideoWriter upisi(ime + " (pracenje).avi", 0, (int) fps, velicina, true);

	if (!upisi.isOpened()) {
		cout << "\nNe mogu stvoriti novi video!!!" << endl;
		destroyWindow("KLT tracking");
        return -1;
    }

	int brojac = 0;  // inicijalizacija brojaèa koji æe se koristiti za brojanje frame-ova

	std::vector<piksel> karak, karak2;  //vektori sa koordinatama karakteristika
	std::vector<pomaci> pomak;  // vektor sa pomacima (ako je potrebno)

	Mat slicica, slicica2, slicicaCB, slicica2CB;  // stvaranje matrica za frame-ove

	int cekaj;

    while(1) {

		slicica.copyTo(slicica2);  // kopiranje prošlog frame-a radi usporedbe

		bool uspjelo = stream.read(slicica);  // provjera ako je frame uspješno proèitan (i da li ih još ima)

        if(!uspjelo) {
			break;
		}

		cvtColor( slicica, slicicaCB, CV_BGR2GRAY );  // pretvorba frame-a u boji u crno bijeli

		if (brojac == 0) {  // svaku sekundu (okvirno) radi se Harrisova detekcija za obnavljanje izgubljenih znaèajki

			struct timeb vrijeme1, vrijeme2; long trajanjems;

			ftime(&vrijeme1);

			slicicaCB = GaussFiltar(slicicaCB, 1);  // zaglaðivanje frame-a

			karak2.clear();

			karak2 = DetekcijaKuteva(slicicaCB, 1, 6, haris);  // detekcija karakteristika (Harrisovi kutevi)
			
			if (karak2.size() > 0) {
				if (karak.size() < vrijednost) {  // ako se vektor karakteristika nije pun, nadopuni ga sa naðenim karakteristikama
					std::random_shuffle(karak2.begin(), karak2.end());  // nasumièno sortiranje karakteristika po iznosu koji je vratio Harris
				}
				
				int m = 0;
				while (karak.size() < vrijednost && m < karak2.size()) {
					karak.push_back(karak2[m]);
					m++;
				}

				for (int i = 0; i< karak.size(); i++) {  // zaokruživanje realnih koordinata na cijelobrojne te oznaèavanje karakteristka (ljubièasto)
					int X1 = floor(karak[i].DajX() + 0.5);
					int Y1 = floor(karak[i].DajY() + 0.5);
					circle(slicica, Point(Y1,X1), 2, Scalar(255, 0, 255), -1);
				}
			}

			imshow("KLT tracking", slicica);  // prikaz slike

			upisi.write(slicica);  // dodavanje nove slièice u izlazni video

			ftime(&vrijeme2);

			trajanjems = 1000 * (vrijeme2.time - vrijeme1.time) + vrijeme2.millitm - vrijeme1.millitm;

			if (trajanjems > 1000/fps) {
				cekaj = 1;
			}

			else cekaj = 1000/fps - trajanjems + 5;

			char c = cvWaitKey(cekaj);

			if (frame == 1) {  // ako je ukljuèena sklopka za rad "frame po frame" onda se èeka pritisak tipke "space" ili "esc"
				c = cvWaitKey();
				if (c == 32) {
					brojac++;
					continue;
				}
				else if (c == 27) break;
			}

		}
		
		else {

			struct timeb vrijeme1, vrijeme2; long trajanjems;

			ftime(&vrijeme1);

			cvtColor( slicica2, slicica2CB, CV_BGR2GRAY );  // pretvaranje slike u boji u crno bijelu

			if (karak.size() != 0) {

				karak = KLT(slicica2CB, slicicaCB, karak, pomak, 0, 9, 30, 0.01);  // raèunanje pomaka karakteristka preko KLT algoritma

				for (int i = 0; i < karak.size(); i++) {  // brisanje nestalih karakteristika
					if (karak[i].DajX() == -1) {
						karak.erase(karak.begin() + i);
						i--;
						continue;
					}
					int X1 = floor(karak[i].DajX() + 0.5);  // vraæanje realnih koordinata u cijelobrojne
					int Y1 = floor(karak[i].DajY() + 0.5);
					circle(slicica, Point(Y1,X1), 2, Scalar(0, 0, 255), -1);  // oznaèavanje karakteristika na slici (crveno)
				}
			}

			imshow("KLT tracking", slicica);  // prikaz slike
			
			upisi.write(slicica);  // dodavanje nove slièice u izlazni video

			ftime(&vrijeme2);

			trajanjems = 1000 * (vrijeme2.time - vrijeme1.time) + vrijeme2.millitm - vrijeme1.millitm;

			if (trajanjems > 1000/fps) {
				cekaj = 1;
			}

			else cekaj = 1000/fps - trajanjems + 5;

			char c = cvWaitKey(cekaj);  // èekanje odreðenog broja milisekundi (radi pravilnog izvoðenja videa)

			if (c == 27) break;  // ako se pritisne tipka "esc" onda se izlazi van iz programa

			if (frame == 1) {  // ako je ukljuèena sklopka za rad "frame po frame" onda se èeka pritisak tipke "space" ili "esc"
				c = cvWaitKey();
				if (c == 32) {
					brojac++;
					if (brojac == 20) brojac = 0;
					continue;
				}
				else if (c == 27) break;
				
			}

		}

		brojac++;  // poveæanje brojaèa

		if (brojac == 20) brojac = 0;  // svakih 20 frame-ova resetiraj brojaè za ulazak u petlju sa Harrisom
    }

    stream.release();  // gašenje toka sa videom

	upisi.release();

	destroyWindow("KLT tracking");  // gašenje prozora

	cout << "\nGotov rad sa videom!\n\n";

	return 1;
}