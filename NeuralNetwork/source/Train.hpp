#ifndef train_h
#define train_h

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "BackProp.hpp"
#include "opencv/cv.hpp"

using namespace std;

class TrainDriver
{

private:
	char path[100];
	int nSample;
	int limTrain;
	int limTest;
	int nBatch;
	int nCvg;
	double thresh;
	double table[10][10];  

public:

	TrainDriver(char* str, int sample, int train, int test, int batch, int cvg, double th);

	int training(CBackProp* bp);

	void test(CBackProp* bp);

	cv::Mat make_fvector(double*fv, int tval);

	double convergence(CBackProp* bp);
};
#endif
