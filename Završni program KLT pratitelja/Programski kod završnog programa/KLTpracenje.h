#ifndef KLT_h
#define KLT_h

#include <iostream>
#include <math.h>
#include <vector>
#include <opencv2/opencv.hpp>
#include "ObradaSlike.h"
#include "PomocneKlase.h"
using namespace std;
using namespace cv;

Mat GX(Mat &Slika);

Mat GY(Mat &Slika);

vector<piksel> KLT(Mat &Slika1, Mat &Slika2, vector<piksel> Kutevi1, vector<pomaci> &Pomak, int mod, int VelProzor,int BrojIteracija, double Prag);

#endif