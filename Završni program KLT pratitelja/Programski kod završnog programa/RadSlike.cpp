#include <iostream>
#include <math.h>
#include <opencv2/opencv.hpp>
#include <string>
#include <fstream>
#include "PomocneKlase.h"
#include "ObradaSlike.h"
#include "DetekcijaKuteva.h"
#include "KLTpracenje.h"
#include "opencv2\highgui\highgui.hpp"
using namespace std;
using namespace cv;

void Klik(int event, int x, int y, int flags, void* ptr) {  // funkcija namijenjena za akciju lijevog klika ako se klikne na sliku
     if  ( event == EVENT_LBUTTONDOWN ) {
		 Point *p = (Point*) ptr;
		 p->x = x;  // spremanje koordinata kliknute toèke
		 p->y = y;

	 }
};

int radSlike() {

	int form(6);  // stavljanje nemoguæe vrijednosti radi inicijalizacije i ulaska u petlju

	while (form < 1 || form > 5) {
		cout << "\nSLIKE\n\nOdaberite format:\n.bmp  -> 1\n.jpg  -> 2\n.png  -> 3\n.tiff -> 4\n.ppm  -> 5\n\nUnos:";
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		cin >> form;

		if (form < 1 || form > 5) {
			cout << "\nUnesen je nepostojeci broj formata!\nMolimo unesite ponovno.\n";
		}
	}

	string format;

	switch (form) {  // odabir formata slike
		case (1):  format = ".bmp"; break;
		case (2):  format = ".jpg"; break;
		case (3):  format = ".png"; break;
		case (4):  format = ".tiff"; break;
		default:   format = ".ppm"; break;
	}

	Mat SlikaOrg1, SlikaOrg2, SlikaCB, SlikaCB2;

	string imeSlike1, imeSlike2;  // unošenje imena slika

	cout << "\nUnesite ime prve slike: ";

	cin >> imeSlike1;

	SlikaOrg1 = imread( imeSlike1 + format, 1 );  // otvaranje prve slike (i provjere otvaranja)

	if ( !SlikaOrg1.data )	{
		cout<<"\nNe mogu otvoriti sliku 1!"<<endl;
		return -1;
    }

	cvtColor( SlikaOrg1, SlikaCB, CV_BGR2GRAY );  // pretvorba slike u boji u crno bijelu

	cout << "\nUnesite ime druge slike: ";

	cin >> imeSlike2;

	SlikaOrg2 = imread( imeSlike2 + format, 1 );  // otvaranje druge slike (i provjere otvaranja)

	if ( !SlikaOrg2.data )	{
		cout<<"\nNe mogu otvoriti sliku 2!"<<endl;
		return -1;
	}

	cvtColor( SlikaOrg2, SlikaCB2, CV_BGR2GRAY );  // pretvorba slike u boji u crno bijelu

	int izbor(5);  // stavljanje nemoguæe vrijednosti radi inicijalizacije i ulaska u petlju
	
	while (izbor > 2 || izbor < 1) {  // odabir nacina rada

		cout << "\nPracenje rucno odabrane znacajke: 1\nPracenje nadenih Harrisovih kutova: 2\n\nUnos:";

		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

		cin >> izbor;

		if (izbor > 2 || izbor < 1) {
			cout << "\nUnesen je pogresan nacin rada!\nMolimo unesite ponovno.\n";
		}

	}

	if (izbor == 1) {  // ispis tablica
		
		namedWindow( "Izbor znacajke", CV_WINDOW_AUTOSIZE);  // stvaranje dvaju prozora sa slikama
		imshow( "Izbor znacajke", SlikaOrg1);

		namedWindow( "Pracena znacajka", CV_WINDOW_AUTOSIZE);
		imshow("Pracena znacajka", SlikaOrg2);

		Point tocka;

		setMouseCallback("Izbor znacajke", Klik, &tocka);  // dodavanje akcije lijevom kliku miša

		while( 1 ) {
			if( cvWaitKey(100) == 32 ) break;  // kada se stisne "space" omoguæuje se nastavak rada
		}

		circle(SlikaOrg1, Point(tocka.x,tocka.y), 2, Scalar(0, 0, 255), -1);  // crtanje toèke na kliknuto mjeto

		piksel Pix(tocka.y,tocka.x);  // objekt gdje se sprema kliknuta toèka i naðena toèka

		vector<piksel> Tocka;

		Tocka.push_back(Pix);

		vector<pomaci> pomak; // vektor s pomacima

		Tocka = KLT(SlikaCB, SlikaCB2, Tocka, pomak, 1, 8, 30, 0.01);

		if (Tocka[0].DajX() != -1) {

			int X = floor(Tocka[0].DajX() + 0.5);  // vraæanje izraèunatih piksela u cijelobrojne vrijednosti
			int Y = floor(Tocka[0].DajY() + 0.5);
			
			circle(SlikaOrg2, Point(Y,X), 2, Scalar(0, 255, 0), -1);  // crtanje toèke na naðenu karakteristiku

			imshow( "Izbor znacajke", SlikaOrg1);  // prikaz slika

			imshow( "Pracena znacajka", SlikaOrg2);

			ofstream pomakX("PomakX.csv"), pomakY("PomakY.csv"), pomakDP("PomakDP.csv");  // otvaranje tokova datoteka za spremanje pomaka (CSV format)
	
			vector<float> pom1 = pomak[0].DajPomakX(), pom2 = pomak[0].DajPomakY(), pom3 = pomak[0].DajKorakPomaka();
	
			for (int i = 0; i < pom1.size(); i++) {
				pomakX << i + 1 << ";" << pom1[i] <<endl;
				pomakY << i + 1<< ";" << pom2[i] <<endl;
				pomakDP << i + 1<< ";" << pom3[i] <<endl;
			}
	
			pomakX.close();  // zatvaranje tokova
			pomakY.close();
			pomakDP.close();
	
			cvWaitKey();
	
			destroyWindow("Izbor znacajke");  // gašenje obaju prozora
			destroyWindow("Pracena znacajka");
		}
		else {
			cout<<"\nGreska!!! Tocka nije nadjena!!!\n";  // ispis i gašenje prozora ako nije naðena karakteristika

			destroyWindow("Izbor tocke");
			destroyWindow("Nadjena znacajka");
		}
	}
	else {

		std::vector<piksel> Kut1, Kut2;  // inicijalizacija vektora sa kutevima

		Kut1 = DetekcijaKuteva(SlikaCB, 1, 6, 10000);  // traženje karakteristika sa Harrisovim algoritmom

		for (int i = 0; i< Kut1.size(); i++) {
			int X1 = floor(Kut1[i].DajX() + 0.5);  // crtanje toèka na naðenim karakteristikama
			int Y1 = floor(Kut1[i].DajY() + 0.5);
			circle(SlikaOrg1, Point(Y1,X1), 2, Scalar(0, 0, 255), -1);
		}

		namedWindow( "Harrisovi kutovi", CV_WINDOW_AUTOSIZE);  // otvaranje novog prozora
		imshow( "Harrisovi kutovi", SlikaOrg1);

		vector<pomaci> pomak;

		while( 1 ) {
			if( cvWaitKey(100) == 32 ) break;  // èekanje da se pritisne tipka "space"
		}

		Kut2 = KLT(SlikaCB, SlikaCB2, Kut1, pomak, 2, 8, 30, 0.01);  // traženje karakteristika na drugoj slici preko KLT-a

		for (int i = 0; i < Kut2.size(); i++) {  // brisanje karakteristika koje nisu naðene
			if (Kut2[i].DajX() == -1) {
				Kut2.erase( Kut2.begin() + i);
			}
		}

		for (int i = 0; i< Kut2.size(); i++) {  // crtanje naðenih karakteristika
			int X2 = floor(Kut2[i].DajX() + 0.5);
			int Y2 = floor(Kut2[i].DajY() + 0.5);
			circle(SlikaOrg2, Point(Y2,X2), 2, Scalar(255, 0, 255), -1);
		}

		namedWindow( "Pracene znacajke", CV_WINDOW_AUTOSIZE); // otvaranje drugog prozora
		imshow("Pracene znacajke", SlikaOrg2);

		cvWaitKey();  // èekanje na pritisak bilo koje tipke

		destroyWindow("Harrisovi kutovi");  // zatvaranje prozora
		destroyWindow("Pracene znacajke");

	}

	cout << "\nGotov rad sa slikama!\n\n";
	return 1;
}
