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

	cout << "Unesite ime videa: ";  // uno�enje imena videa i provjera da li video postoji

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

	createTrackbar("Znacajke", "KLT tracking", &vrijednost, 100, NULL);  // stvaranje "slidera" u prozoru (prvi je broj karaktristika, drugi je prag za Harrisa, a tre�i je sklopka za rad "ru�no frame po frame")
	createTrackbar("Harris", "KLT tracking", &haris, 10000, NULL);
	createTrackbar("Frame", "KLT tracking", &frame, 1, NULL);

	char c = 100;  // uno�enje nemogu�e vrijednosti da imamo s ne�im uspore�ivati

	while (c != 32) {  // �ekanje dok se ne pritisne "space"
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

	double sirina = stream.get(CV_CAP_PROP_FRAME_WIDTH);  // uzimanje podataka o �irini videa iz informacija ulazne datoteke

	double visina = stream.get(CV_CAP_PROP_FRAME_HEIGHT);  //  uzimanje podataka o visini videa iz informacija ulazne datoteke

	double fps = stream.get(CV_CAP_PROP_FPS);  // uzimanje podataka o broju sli�ica u sekundi...

	Size velicina(static_cast<int>(sirina), static_cast<int>(visina));  // Objekt sa dimenzijama za novi video

	ime.erase(ime.end() - 4, ime.end());

	VideoWriter upisi(ime + " (pracenje).avi", 0, (int) fps, velicina, true);

	if (!upisi.isOpened()) {
		cout << "\nNe mogu stvoriti novi video!!!" << endl;
		destroyWindow("KLT tracking");
        return -1;
    }

	int brojac = 0;  // inicijalizacija broja�a koji �e se koristiti za brojanje frame-ova

	std::vector<piksel> karak, karak2;  //vektori sa koordinatama karakteristika
	std::vector<pomaci> pomak;  // vektor sa pomacima (ako je potrebno)

	Mat slicica, slicica2, slicicaCB, slicica2CB;  // stvaranje matrica za frame-ove

	int cekaj;

    while(1) {

		slicica.copyTo(slicica2);  // kopiranje pro�log frame-a radi usporedbe

		bool uspjelo = stream.read(slicica);  // provjera ako je frame uspje�no pro�itan (i da li ih jo� ima)

        if(!uspjelo) {
			break;
		}

		cvtColor( slicica, slicicaCB, CV_BGR2GRAY );  // pretvorba frame-a u boji u crno bijeli

		if (brojac == 0) {  // svaku sekundu (okvirno) radi se Harrisova detekcija za obnavljanje izgubljenih zna�ajki

			struct timeb vrijeme1, vrijeme2; long trajanjems;

			ftime(&vrijeme1);

			slicicaCB = GaussFiltar(slicicaCB, 1);  // zagla�ivanje frame-a

			karak2.clear();

			karak2 = DetekcijaKuteva(slicicaCB, 1, 6, haris);  // detekcija karakteristika (Harrisovi kutevi)
			
			if (karak2.size() > 0) {
				if (karak.size() < vrijednost) {  // ako se vektor karakteristika nije pun, nadopuni ga sa na�enim karakteristikama
					std::random_shuffle(karak2.begin(), karak2.end());  // nasumi�no sortiranje karakteristika po iznosu koji je vratio Harris
				}
				
				int m = 0;
				while (karak.size() < vrijednost && m < karak2.size()) {
					karak.push_back(karak2[m]);
					m++;
				}

				for (int i = 0; i< karak.size(); i++) {  // zaokru�ivanje realnih koordinata na cijelobrojne te ozna�avanje karakteristka (ljubi�asto)
					int X1 = floor(karak[i].DajX() + 0.5);
					int Y1 = floor(karak[i].DajY() + 0.5);
					circle(slicica, Point(Y1,X1), 2, Scalar(255, 0, 255), -1);
				}
			}

			imshow("KLT tracking", slicica);  // prikaz slike

			upisi.write(slicica);  // dodavanje nove sli�ice u izlazni video

			ftime(&vrijeme2);

			trajanjems = 1000 * (vrijeme2.time - vrijeme1.time) + vrijeme2.millitm - vrijeme1.millitm;

			if (trajanjems > 1000/fps) {
				cekaj = 1;
			}

			else cekaj = 1000/fps - trajanjems + 5;

			char c = cvWaitKey(cekaj);

			if (frame == 1) {  // ako je uklju�ena sklopka za rad "frame po frame" onda se �eka pritisak tipke "space" ili "esc"
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

				karak = KLT(slicica2CB, slicicaCB, karak, pomak, 0, 9, 30, 0.01);  // ra�unanje pomaka karakteristka preko KLT algoritma

				for (int i = 0; i < karak.size(); i++) {  // brisanje nestalih karakteristika
					if (karak[i].DajX() == -1) {
						karak.erase(karak.begin() + i);
						i--;
						continue;
					}
					int X1 = floor(karak[i].DajX() + 0.5);  // vra�anje realnih koordinata u cijelobrojne
					int Y1 = floor(karak[i].DajY() + 0.5);
					circle(slicica, Point(Y1,X1), 2, Scalar(0, 0, 255), -1);  // ozna�avanje karakteristika na slici (crveno)
				}
			}

			imshow("KLT tracking", slicica);  // prikaz slike
			
			upisi.write(slicica);  // dodavanje nove sli�ice u izlazni video

			ftime(&vrijeme2);

			trajanjems = 1000 * (vrijeme2.time - vrijeme1.time) + vrijeme2.millitm - vrijeme1.millitm;

			if (trajanjems > 1000/fps) {
				cekaj = 1;
			}

			else cekaj = 1000/fps - trajanjems + 5;

			char c = cvWaitKey(cekaj);  // �ekanje odre�enog broja milisekundi (radi pravilnog izvo�enja videa)

			if (c == 27) break;  // ako se pritisne tipka "esc" onda se izlazi van iz programa

			if (frame == 1) {  // ako je uklju�ena sklopka za rad "frame po frame" onda se �eka pritisak tipke "space" ili "esc"
				c = cvWaitKey();
				if (c == 32) {
					brojac++;
					if (brojac == 20) brojac = 0;
					continue;
				}
				else if (c == 27) break;
				
			}

		}

		brojac++;  // pove�anje broja�a

		if (brojac == 20) brojac = 0;  // svakih 20 frame-ova resetiraj broja� za ulazak u petlju sa Harrisom
    }

    stream.release();  // ga�enje toka sa videom

	upisi.release();

	destroyWindow("KLT tracking");  // ga�enje prozora

	cout << "\nGotov rad sa videom!\n\n";

	return 1;
}