#include <iostream>
#include <cstdlib>
#include <cstring>
#include <stdio.h>

using namespace std;

class Graph
{
private:
	int nNode;
	int** table;
	int** path;

public:

	Graph(int n, int** w):nNode(n)
	{
		cout << "constructor called" << endl;
	
		this->table = w;
		this->path = new int*[this->nNode+1];
		for(int i = 1; i <= this->nNode; i++)
		{
			this->path[i] = new int[this->nNode+1];
			memset(this->path[i], 0x00, sizeof(int) * (this->nNode+1));
		}
	}
	
	~Graph()
	{
		cout << "destructor called ..." << endl;

		for(int i = 1; i <= this->nNode; i++)
		{
			if(this->table[i]) delete this->table[i];
			if(this->path[i]) delete this->path[i];
		}

		if(this->table) delete this->table;
		if(this->path) delete this->path;
	}
	
	void floyd()
	{
		int i, j, k;

		//D^(k)[i][j] is shortest path for going to v_i to v_j
		//by only passing the node v_1, v_2, ..., v_k
		for(k=1; k<=this->nNode; k++)
		{
			for(i=1; i<=this->nNode; i++)
				for(j=1; j<=this->nNode; j++)
					//D^(k)[i][j] = argmin(D^(k-1)[i][j], D^(k-1)[i][k]+D^(k-1)[k][j])
					//When there are intermediate nodes in path for v_i to v_j,
					//P[i][j] is most beggest index of node in those.
					if(this->table[i][j] > this->table[i][k] + this->table[k][j])
					{
						this->table[i][j] = this->table[i][k] + this->table[k][j];
						this->path[i][j] = k;
					}
		}
	}
	
	void shortest_path(int i, int j)
	{
		if(!this->path[i][j]) return;
	
		shortest_path(i, this->path[i][j]);
		cout << "v" << this->path[i][j] << ' ';
		shortest_path(this->path[i][j], j);
	}
};

int read_arg_file(char** argv, Graph** grp)
{
	FILE *fp;
	int  nNode, **W;
	int  i, j, len, idx, val;
	char line[100], *ptr;
	
	if((fp = fopen(argv[1], "rt")) == NULL) return -1;

	//allocate table and path
	memset(line, 0x00, sizeof(line));
	fgets(line, sizeof(line), fp);
	line[strlen(line)-1] = '\0';
	cout << "n: " << (nNode = atoi(line)) << endl;
	W = new int*[nNode+1];
	for(i = 1; i <= nNode; i++)
	{
		W[i] = new int[nNode+1];
		for(j = 1; j <= nNode; j++)
		{
			if(i==j) W[i][j] = 0x00000000;
			else W[i][j] = 0x3FFFFFFF;
		}
	}

	//init table
	for(i=1; i<=nNode; i++)
	{
		memset(line, 0x00, sizeof(line));
		fgets(line, sizeof(line), fp);
		line[strlen(line)-1] = '\0';
		cout << "[" << i << "] ";
		ptr = strtok(line, " ");
		while(ptr != NULL)
		{
			len = strlen(ptr);
			for(j=0; j<len; j++) if(ptr[j] == ':') { ptr[j] = '\0'; break; }
			W[i][idx=atoi(ptr)] = (val = atoi(ptr+j+1));
			cout << idx << ':' << val << ' ';
			ptr = strtok(NULL, " ");
		}
		cout << endl;
	}

	*grp = new Graph(nNode, W);	
	
	fclose(fp);

	return 0;
}

int main(int argc, char* argv[])
{
	Graph *grp;
	int start, destination;

	if(argc != 2)
	{
		cerr << "usage : prg input" << endl;
		exit(-1);
	}

	if(read_arg_file(argv, &grp) < 0)
	{
		cerr << "no such file " << argv[1] << endl;
		exit(-1);	
	}

	grp->floyd();

	cout << "start node: "; cin >> start;
	cout << "destination node: "; cin >> destination;
	
	grp->shortest_path(start, destination);
	cout << endl;

	delete grp;
	return 0;
}




