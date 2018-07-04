#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "BackProp.hpp"
#include "Train.hpp"
#include "opencv/cv.hpp"

int read_arg_file(char** argv, CBackProp** bp, TrainDriver** td)
{
	FILE *fp1, *fp2;
	int numLayers, *lSz; 
	double beta, alpha;
	
	char path[100];
	int nSample, limTrain, limTest, nBatch, nCvg;
	double thresh;

	//step1: allocate network
	if((fp1 = fopen(argv[1], "rt")) == NULL) return -1;

	fscanf(fp1, "%d", &numLayers);
	fprintf(stdout, "num layer: %d\n", numLayers);
	
	lSz = (int*)malloc(sizeof(int) * numLayers);
	for(int i = 0; i < numLayers; i++){
		fscanf(fp1, "%d", &lSz[i]);
		fprintf(stdout, "%d ", lSz[i]);
	}

	fscanf(fp1, "%lf", &beta);
	fprintf(stdout, "\nbeta: %lf\n", beta);

	*bp = new CBackProp(numLayers, lSz, beta);	
	fclose(fp1);

	//step2: allocate training manager
	if((fp2 = fopen(argv[2], "rt")) == NULL) return -2;
	
	memset(path, 0x00, sizeof(path));
	fscanf(fp2, "%s", path);
	fprintf(stdout, "data path : %s\n", path);
	
	fscanf(fp2, "%d", &nSample);
	fprintf(stdout, "num sample : %d\n", nSample);

	fscanf(fp2, "%d", &limTrain);
	fprintf(stdout, "limit of train : %d\n", limTrain);

	fscanf(fp2, "%d", &limTest);
	fprintf(stdout, "limit of test : %d\n", limTest);

	fscanf(fp2, "%d", &nBatch);
	fprintf(stdout, "batch size : %d\n", nBatch);

	fscanf(fp2, "%d", &nCvg);
	fprintf(stdout, "number of test for convergence check : %d\n", nCvg); 
	
	fscanf(fp2, "%lf", &thresh);
	fprintf(stdout, "Thresh : %lf\n", thresh);

	*td = new TrainDriver(path, nSample, limTrain, limTest, nBatch, nCvg, thresh);
	fclose(fp2);
	
	return 0;
}

int main(int argc, char* argv[])
{
	int retval;
	CBackProp *bp;
	TrainDriver *td;

	if(argc != 3)
	{
		fprintf(stderr, "usage : prg xxx.cfg xxx.train\n");
		exit(-1);
	}

	srand((unsigned)time(NULL));

	if((retval = read_arg_file(argv, &bp, &td)) < 0)
	{
		if(retval == -1)
		{
			fprintf(stderr, "no such file [%s]\n", argv[1]);
			exit(-1);	
		}
		else
		{
			fprintf(stderr, "no such file [%s]\n", argv[2]);
			exit(-1);
		}
	}
	
	fprintf(stdout,"\nNow training the network...\nPress any key\n");	
	getchar();

	if(!(td->training(bp))) fprintf(stdout, "success of weight convergence\n");
	else fprintf(stdout, "failure of weight convergence\n");

	fprintf(stdout, "\nNow using the trained network\nmake predictions on test data...\n");
	getchar();

	td->test(bp);

	delete bp;
	delete td;
	return 0;
}




