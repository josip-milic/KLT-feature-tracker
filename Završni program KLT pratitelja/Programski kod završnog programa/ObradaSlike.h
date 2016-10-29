#ifndef ObradaSlike_h
#define ObradaSlike_h

#include <iostream>
#include <math.h>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;

Mat SobelovOperator(Mat &Slika,int x, int y);

Mat GaussFiltar(Mat &Sl, double sigma);

float interpoliraj(Mat &Slika, float x, float y);

Mat BilinearnaInt(Mat &Slika, double piksX, double piksY, int DimProzor);

void Klik(int event, int x, int y, int flags, void* ptr);

int radSlike();

int radVideo();

#endif