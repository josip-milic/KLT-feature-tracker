#ifndef PomocneKlase_h
#define PomocneKlase_h

#include <iostream>
#include <math.h>
#include <string>
#include <vector>
using namespace std;

class pomaci {  //pomoæna klasa namijenjena za spremanje pomaka znaèajke
private:
	vector<float> pomakX;
	vector<float> pomakY;
	vector<float> korakPomaka;
public:
	vector<float> DajPomakX() { return pomakX; };
	vector<float> DajPomakY() { return pomakY; };
	vector<float> DajKorakPomaka() { return korakPomaka; };
	void DodajPomakX (float pX) { pomakX.push_back(pX); };
	void DodajPomakY (float pY) { pomakY.push_back(pY); };
	void DodajKorakPomaka (float korak) { korakPomaka.push_back(korak); }
	void obrisiSve() { pomakX.clear(); pomakY.clear(); korakPomaka.clear(); };
};

class piksel {  //pomoæna klasa u koju spremamo koordinate piksela i nekim sluèajevima iznos koji je Harrisova detekcija dodijelila tom pikselu
private:
	double x;
	double y;
	double HarrisIznos;
public:
	piksel(): x(0), y(0), HarrisIznos(0) {};
	piksel(double X, double Y) {x = X; y = Y; HarrisIznos = 0;};
	piksel(double X, double Y, double Z) { x = X; y = Y; HarrisIznos = Z;};
	double DajX() { return x;};
	double DajY() { return y;};
	double DajIznos() {return HarrisIznos;};
	void promijeniX(double X) { x = X;};
	void promijeniY(double Y) { y = Y;};
	void promijeniIznos(double Z) { HarrisIznos = Z;};
};

#endif