
//////////////////////////////////////////////
//	Fully connected multilayered feed		//
//	forward	artificial neural network using	//
//	Backpropogation	algorithm for training.	//
//////////////////////////////////////////////

#ifndef backprop_h
#define backprop_h

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

using namespace std;

class CBackProp
{

private:
//	output of each neuron
	double **out;

//	delta error value for each neuron
	double **delta;

//	vector of weights for each neuron
	double ***weight;

//	no of layers in net including input layer
	int numl;

//	vector of numl elements for size of each layer
	int *lsize;

//	learning rate
	double beta;

//	storage for weight-change made in previous epoch
	double ***prevDwt;

//  counter for batch update.
	int nBpgt;

public:

	~CBackProp();

//	initializes and allocates memory
	CBackProp(int nl,int *sz,double b);

//  copy counstructor
	CBackProp(const CBackProp&);

//	backpropogates error for one set of input
	int bpgt(double *in, double *tgt, int batch);

//	feed forwards activations for one set of inputs
	void ffwd(double *in);

//	returns mean square error of the net
	double mse(double *tgt) const;	
	
//	returns i'th output of the net
	double Out(int i) const;

//  activation function.
	double sigmoid(double in);
};

#endif
