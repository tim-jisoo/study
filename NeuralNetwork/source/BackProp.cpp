#include "BackProp.hpp"

//	initializes and allocates memory on heap
CBackProp::CBackProp(int nl,int *sz,double b) :numl(nl), lsize(sz), beta(b), nBpgt(0)
{
	int i;

	//	allocate memory for output of each neuron
	out = new double*[numl];

	for( i=0;i<numl;i++){
		out[i]=new double[lsize[i]];
	}

	//	allocate memory for delta
	delta = new double*[numl];

	for(i=1;i<numl;i++){
		delta[i]=new double[lsize[i]];
	}

	//	allocate memory for weights
	weight = new double**[numl];

	for(i=1;i<numl;i++){
		weight[i]=new double*[lsize[i]];
	}
	for(i=1;i<numl;i++){
		for(int j=0;j<lsize[i];j++){
			weight[i][j]=new double[lsize[i-1]+1];
		}
	}

	//	allocate memory for previous weights
	prevDwt = new double**[numl];

	for(i=1;i<numl;i++){
		prevDwt[i]=new double*[lsize[i]];

	}
	for(i=1;i<numl;i++){
		for(int j=0;j<lsize[i];j++){
			prevDwt[i][j]=new double[lsize[i-1]+1];
		}
	}

	//	seed and assign random weights
	for(i=1;i<numl;i++)
		for(int j=0;j<lsize[i];j++)
			for(int k=0;k<lsize[i-1]+1;k++)
				weight[i][j][k]=(double)(rand())/(RAND_MAX/2) - 1;//32767

	//	initialize previous weights to 0 for first iteration
	for(i=1;i<numl;i++)
		for(int j=0;j<lsize[i];j++)
			for(int k=0;k<lsize[i-1]+1;k++)
				prevDwt[i][j][k]=(double)0.0;
}

CBackProp::~CBackProp()
{
	int i;
	//	free out
	for(i=0;i<numl;i++)
		delete[] out[i];
	delete[] out;

	//	free delta
	for(i=1;i<numl;i++)
		delete[] delta[i];
	delete[] delta;

	//	free weight
	for(i=1;i<numl;i++)
		for(int j=0;j<lsize[i];j++)
			delete[] weight[i][j];
	for(i=1;i<numl;i++)
		delete[] weight[i];
	delete[] weight;

	//	free prevDwt
	for(i=1;i<numl;i++)
		for(int j=0;j<lsize[i];j++)
			delete[] prevDwt[i][j];
	for(i=1;i<numl;i++)
		delete[] prevDwt[i];
	delete[] prevDwt;

	//	free layer info
	delete[] lsize;
}

//	sigmoid function
double CBackProp::sigmoid(double x){ return (1.0 / (1.0 + exp(-x))); }

//	mean square error
double CBackProp::mse(double *tgt) const
{
	double mse = 0.0;
	for(int i=0;i<lsize[numl-1];i++)
	{
		mse+=(tgt[i]-out[numl-1][i])*(tgt[i]-out[numl-1][i]);
	}
	return mse/2;
}


//	returns i'th output of the net
double CBackProp::Out(int i) const
{
	return out[numl-1][i];
}

// feed forward one set of input
void CBackProp::ffwd(double *in)
{
	double sum;
	int i, j, k;

	//	assign content to input layer
	for(i=0;i<lsize[0];i++)
		out[0][i]=in[i];  // output_from_neuron(i,j) Jth neuron in Ith Layer

	//	assign output(activation) value 
	//	to each neuron usng sigmoid func
	for(i=1;i<numl;i++)
	{
		for(j=0;j<lsize[i];j++)
		{
			sum=0.0;
			for(k=0;k<lsize[i-1];k++)
				sum += out[i-1][k]*weight[i][j][k];	// Apply weight to inputs and add to sum
			
			sum += weight[i][j][lsize[i-1]];		// Apply bias

			out[i][j]=sigmoid(sum);				
		}
	}
}

//	backpropogate errors from output
//	layer uptill the first hidden layer
int CBackProp::bpgt(double *in,double *tgt, int batch)
{
	double sum;
	int i,j,k;
	
	this->nBpgt++;

	//	update output values for each neuron
	ffwd(in);

	//	find delta for output layer
	for(i=0;i<lsize[numl-1];i++)
		delta[numl-1][i]=out[numl-1][i] * (1-out[numl-1][i])*(tgt[i]-out[numl-1][i]);

	//	find delta for hidden layers	
	for(i=numl-2;i>0;i--)
	{
		for(j=0;j<lsize[i];j++)
		{
			sum=0.0;
			for(int k=0;k<lsize[i+1];k++)
				sum+=delta[i+1][k]*weight[i+1][k][j];
			delta[i][j]=out[i][j]*(1-out[i][j])*sum;
		}
	}

	//update weight	
	if(this->nBpgt % batch)
	{
		for(i=1; i < numl; i++)
		{
			for(j=0; j<lsize[i];j++)
			{
				for(k=0;k<lsize[i-1];k++)
					prevDwt[i][j][k] += beta*delta[i][j]*out[i-1][k];

				prevDwt[i][j][lsize[i-1]] += beta*delta[i][j];	
			}
		}

		return 0;
	}

	else
	{
		this->nBpgt=0;
		for(i=1;i<numl;i++)
		{
			for(j=0;j<lsize[i];j++)
			{
				for(k=0;k<lsize[i-1];k++)
				{	
					prevDwt[i][j][k] += beta*delta[i][j]*out[i-1][k];
					weight[i][j][k] += prevDwt[i][j][k]/(double)batch;
					prevDwt[i][j][k] = 0.0;
				}		
				prevDwt[i][j][lsize[i-1]] += beta*delta[i][j];
				weight[i][j][lsize[i-1]] += prevDwt[i][j][lsize[i-1]]/(double)batch;
				prevDwt[i][j][lsize[i-1]] = 0.0;
			}
		}		
		return 1;
	}
}

