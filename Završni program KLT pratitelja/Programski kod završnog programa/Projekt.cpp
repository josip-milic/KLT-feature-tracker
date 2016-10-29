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

int main() {  //Glavni program namijenjen za izbor naèina rada
	
	cout<<"\t***KLT tracker verzija 1.0***\n\n";

	int unos(5);

	while (unos) {

		unos = 5;

		cout << "GLAVNI IZBORNIK\n\n";

		cout << "Rad sa slikama: 1\n";

		cout << "Rad sa videom: 2\n";

		cout << "Kraj rada: 0\n\nUnos:";

		cin >> unos;

		while (unos < 0 || unos > 2) {
			cout << "\nUnesen je nepodrzan nacin rada!!!\n\nUnesite ponovno: ";
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			cin >> unos;
		}

		if ( unos == 1 ) {
			if (radSlike() == -1) {
				cout << "\nGreska prilikom rada!\n\n";
			}
			continue;
		}
		else if ( unos == 2 ) {
			if (radVideo() == -1){
				cout << "\nGreska prilikom rada!\n\n";
			}
			continue;
		}
		else continue;
	}

	cout << "\nGotov rad s programom!\n\nDovidjenja..." << endl;

	return 0;
}
