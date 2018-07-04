#include "Train.hpp"


TrainDriver::TrainDriver(char* str, int sample, int train, int test, int batch, int cvg, double th)
:nSample(sample), limTrain(train), limTest(test), nBatch(batch), nCvg(cvg), thresh(th)
{
		int i, j;

		memset(path, 0x00, sizeof(path));
		strncpy(path, str, strlen(str));
		for(i = 0; i < 10; i++)
		{
			for(j = 0; j < 10; j++)
				this->table[i][j] = (i == j) ? 1.0 : 0.0;				
		}
}

int TrainDriver::training(CBackProp* bp)
{
		double fvector[28*28];
		int i, tval;
	
		for(i=0; i< this->limTrain; i++)
		{
			tval = rand() % 10;
			make_fvector(fvector, tval);
			if(bp->bpgt(fvector, this->table[tval], this->nBatch))
			{
				cout << endl << "MSE: " << bp->mse(this->table[tval]) << "...Training..." << endl;
		
				//check convergence of weight indirectly.
				//By comparing output with true value.
				if(convergence(bp) < this->thresh) return 0;
			}
		}

		return 1;
}	

void TrainDriver::test(CBackProp* bp)
{
		int i, j, idx, tval;
		double max;
		double fvector[28*28];

		for(i = 0; i < this->limTest; i++)
		{
			tval = rand() % 10;
			cv::imshow("test img", make_fvector(fvector, tval));
			bp->ffwd(fvector);
			
			idx = 0;
			max = 0.0;
			for(j = 0; j < 10; j++)
			{
				fprintf(stdout, "[%d] %lf\n", j, bp->Out(j));
				if(max < bp->Out(j))
				{
					idx = j;
					max = bp->Out(j);
				}
			}
		
		fprintf(stdout, "result is %d\n", idx);
		cv::waitKey();
		}
}

cv::Mat TrainDriver::make_fvector(double*fv, int tval)
{
		int i;
		char str[100], temp[100];
		cv::Mat img;	

		memset(str, 0x00, sizeof(str));
		strncpy(str, this->path, strlen(this->path));

		memset(temp, 0x00, sizeof(temp));
		sprintf(temp,"%d/%d.bmp", tval, rand()%this->nSample);

		strncat(str, temp, strlen(temp));

		img = cv::imread(str,0);

		for(i = 0; i < 28*28; i++)
			fv[i] = (double)(img.data[i])/(double)0xFF;				

		return img;
}

double TrainDriver::convergence(CBackProp* bp)
{
		double fv[28*28];
		double avg;
		int i, tval;

		avg = 0.0;
		for(i=0; i<this->nCvg; i++)
		{
			tval = rand() % 10;
			make_fvector(fv, tval);
			bp->ffwd(fv);
			avg+=bp->mse(this->table[tval]);
		}
		return avg/(double)(this->nCvg);
}	

