#include <iostream>
#include <cstdlib>
#include <cstring>
#include <stdio.h>

using namespace std;

class MatXMat
{
private:
	int  nMat;
	int  *size;
	int  **table;
	int  **path;

public:
	MatXMat(int n, int* sz) : nMat(n), size(sz)
	{
		int i;

		cout << "constructor called" << endl;
	
		this->table = new int*[this->nMat+1];
		this->path = new int*[this->nMat+1];

		for(i = 1; i <= this->nMat; i++)
		{
			this->table[i] = new int[this->nMat+1];
			memset(this->table[i], 0x00, sizeof(int) * (this->nMat+1));
			
			this->path[i] = new int[this->nMat+1];
			memset(this->path[i], 0x00, sizeof(int) * (this->nMat+1));
		}
	}
	
	~MatXMat()
	{
		int i;
		cout << "destructor called ..." << endl;

		for(i = 1; i <= this->nMat; i++)
		{
			if(this->table[i]) delete this->table[i];
			if(this->path[i]) delete this->path[i];
		}

		if(this->table) delete this->table;
		if(this->path) delete this->path;
		if(this->size) delete this->size;
	}
	
	void Optimize()
	{
		//i : row
		//j : col
		//k : case
		//l : diagonal
		int i,j,k,l;
		int minima, comp;
	
		for(l = 1; l <= this->nMat-1; l++)
		{
			for(i = 1; i <= this->nMat-l; i++)
			{
				j = i + l;

				minima = 0x7FFFFFFF;

				for(k = i; k <= j-1; k++)
				{
					comp = this->table[i][k] + this->table[k+1][j] 
							+ this->size[i-1] * this->size[k] * this->size[j];
					if(minima > comp)
					{
						minima = comp;
						this->path[i][j] = k;
					}
				}					

				this->table[i][j] = minima; 
			}						
		}
	}

	void order(int i, int j)
	{
		if(i==j) cout << "A" << i;
		else
		{
			cout << "(";
			order(i, this->path[i][j]);
			order(this->path[i][j]+1, j);
			cout << ")";
		}	
	}
};

int read_arg_file(char** argv, MatXMat** opt)
{
	FILE *fp;
	int i, n, *size;
	int row, col;
	
	if((fp = fopen(argv[1], "rt")) == NULL) return -1;

	fscanf(fp, "%d", &n);
	cout << "n: " << n << endl;
	size = new int[n+1];


	fscanf(fp, "%d %d", &size[0], &size[1]);
	cout << size[0] << "X" << size[1] << endl;
	for(i = 1; i < n; i++)
	{
		fscanf(fp, "%d %d", &row, &col);
		cout << row << "X" << col << endl;
		if(size[i] != row)
		{
			fclose(fp);
			delete size;
			return -2;		
		}
		size[i+1] = col;
	}
	
	*opt = new MatXMat(n, size);	
	
	fclose(fp);

	return 0;
}

int main(int argc, char* argv[])
{
	MatXMat *opt;
	int retval, start, dest;
	
	if(argc != 2)
	{
		cerr << "usage : prg input" << endl;
		exit(-1);
	}

	if((retval = read_arg_file(argv, &opt)) < 0)
	{
		if(retval == -1)
			cerr << "no such file " << argv[1] << endl;
		
		else
			cerr << "wrong input file format " << endl;

		exit(-1);	
	}

	opt->Optimize();

	cout << "start node: "; cin >> start;
	cout << "destination node: "; cin >> dest;
	
	opt->order(start, dest);	
	cout << endl;

	delete opt;
	return 0;
}




