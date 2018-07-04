#include <iostream>
#include <cstdlib>
#include <cstring>
#include <stdio.h>

using namespace std;

class Graph
{
private:
	bool flag;
	int nNode;
	int** table;
	int** path;

public:
	void init()
	{
		for(int i = 1; i <= this->nNode; i++)
		{
			if(this->table[i])
				delete this->table[i];
			if(this->path[i])
				delete this->path[i];
		}

		if(this->table)
			delete this->table;
		if(this->path)
			delete this->path;
		this->nNode = 0;
		this->flag = false;
	}

	Graph(int n, int** w):nNode(n), flag(false)
	{
		this->table = new int*[this->nNode+1];
		this->path = new int*[this->nNode+1];
		for(int i = 1; i <= this->nNode; i++)
		{
			this->table[i] = new int[this->nNode+1];
			for(int j = 1; j <= this->nNode; j++)
				this->table[i][j] = w[i][j];
			
			this->path[i] = new int[this->nNode+1];
			memset(this->path[i], 0x00, sizeof(int) * (this->nNode+1));
		}
	}

	Graph(const Graph& copy):nNode(copy.nNode), flag(copy.flag)
	{
		this->init();		

		this->table = new int*[this->nNode+1];
		this->path = new int*[this->nNode+1];
		for(int i = 1; i <= this->nNode; i++)
		{
			this->table[i] = new int[this->nNode+1];
			for(int j = 1; j <= this->nNode; j++)
				this->table[i][j] = copy.table[i][j];

			this->path[i] = new int[this->nNode+1];
			memset(this->path[i], 0x00, sizeof(int) * (this->nNode+1));
		}

	}

	~Graph()
	{
		this->init();
	}
	
	void floyd()
	{
		int i, j, k;

		if(this->flag)
		{
			cout << "already knows the shortest path in this graph." << endl;
			return;
		}

		this->flag = true;

		//D^(k)[i][j] is shortest path for going to v_i to v_j by only passing the node v_1, v_2, ..., v_k
		for(k=1; k<=this->nNode; k++)
			for(i=1; i<=this->nNode; i++)
				for(j=1; j<=this->nNode; j++)
					//D^(k)[i][j] = argmin(D^(k-1)[i][j], D^(k-1)[i][k]+D^(k-1)[k][j])
					//When there are intermediate nodes in path for v_i to v_j, P[i][j] is most beggest index of node in those.
					if(this->table[i][j] > this->table[i][k] + this->table[k][j])
					{
						this->table[i][j] = this->table[i][k] + this->table[k][j];
						this->path[i][j] = k;
					}

	}

	void shortest_path(int i, int j)
	{
		if(!this->flag)
		{
			cout << "don't know the shortest path yet." << endl;
			return;
		}
		
		if(this->path[i][j])
		{
			shortest_path(i, this->path[i][j]);
			cout << "v" << this->path[i][j] << ' ';
			shortest_path(this->path[i][j], j);
		}
	}
};

int read_arg_file(char** argv, Graph** grp)
{
	FILE *fp;
	int  nNode, **W;
	int  i, j, len, idx, val;
	char line[100], *ptr;
	
	if((fp = fopen(argv[1], "rt")) == NULL) return -1;

	memset(line, 0x00, sizeof(line));
	fgets(line, sizeof(line), fp);
	line[strlen(line)-1] = '\0';
	fprintf(stdout, "num_node: %d\n", nNode = atoi(line));

	W = new int*[nNode+1];
	for(i = 1; i <= nNode; i++)
	{
		W[i] = new int[nNode+1];
		for(j = 1; j <= nNode; j++)
		{
			if(i==j) W[i][j] = 0;
			else W[i][j] = 0x7FFFFFFF>>1;
		}
	}

	for(i=1; i<=nNode; i++)
	{
		memset(line, 0x00, sizeof(line));
		fgets(line, sizeof(line), fp);
		line[strlen(line)-1] = '\0';

		ptr = strtok(line, " ");
		while(ptr != NULL)
		{
			len = strlen(ptr);
			for(j=0; j<len; j++) if(ptr[j] == ':') break;
			ptr[j] = '\0';
			idx = atoi(ptr);
			val = atoi(ptr+j+1);
			W[i][idx] = val;
			cout << idx << ':' << val << " ";
			ptr = strtok(NULL, " ");
		}
		cout << endl;
	}

	*grp = new Graph(nNode, W);	
	
	fclose(fp);

	for(i = 1; i <= nNode; i++) if(W[i]) delete W[i];
	if(W) delete W;

	return 0;
}

int main(int argc, char* argv[])
{
	Graph *grp;
	int start, destination;

	if(argc != 2)
	{
		cerr << "usage : prg graph" << endl;
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
	return 0;
}




