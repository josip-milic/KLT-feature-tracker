#ifndef DetekcijaKuteva_h
#define DetekcijaKuteva_h

#include <iostream>
#include <math.h>
#include <opencv2/opencv.hpp>
#include <string>
#include "PomocneKlase.h"
using namespace std;
using namespace cv;

std::vector<piksel> DetekcijaKuteva(Mat &Slika, double sigma, int VelProzor, int Prag);

#endif