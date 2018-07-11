#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <stdio.h>

//s : expression of set (example : 3(decimal) == 11(binary), 2^1 means v3 and 2^0 means v2.
//v : number of vertex
#define INSET(s,v)				(s&(1<<(v-2)))
#define SET_DIFFERENCE(s,v)		(s&(~(1<<(v-2))))
#define MYINFINITY				0x3FFFFFFF

using namespace std;

struct Node
{
	int   data;
	Node* ptr;

	Node(int d) : data(d), ptr(NULL) {}
};

struct HashTable
{
	int nNode;
	int tsize;
	Node** table;

	HashTable(int n) : nNode(n)
	{
		cout << endl << "hash table created" << endl;

		//don't care the v1 node.
		this->tsize = (1 << (this->nNode-1));
		this->table = new Node*[this->tsize];
		this->mktable();
	}

	~HashTable()
	{
		Node* temp;
		
		cout << endl << "hash table eleminated" << endl;
		
		for(int i = 0; i < this->tsize; i++)
		{
			while(this->table[i])
			{
				temp = this->table[i];
				this->table[i] = temp->ptr;
				delete temp;
			}
		}
	
		if(this->table) delete this->table;
	}

	//input : integer number or expression of set.
	//output : number of bit which value is 1.
	int hash_func(int idx)
	{
		int cnt = 0;
	
		//LSB means the v2 node.
		for(int i = 2; i <= this->nNode; i++)
			if(INSET(idx,i)) cnt++;
		
		return cnt;
	}

	void insert(int entry, Node* ptr)
	{
		//empty entry case
		if(!(this->table[entry]))
			this->table[entry] = ptr;

		//non-empty entry case
		else
		{
			ptr->ptr = this->table[entry];
			this->table[entry] = ptr;
		}
	}

	void mktable()
	{
		memset(this->table, 0x00, sizeof(Node*) * this->tsize);

		for(int i = 0; i < this->tsize; i++)
			this->insert(this->hash_func(i), new Node(i));
	}
};


class Graph
{
private:
	int n;		//number of vertex.
	int** W;	//weight matrix.
	int** D;	//array for DP.
	int** P;	//array for path.

	void foo(int start, int set)
	{
		int next = this->P[start][set];
		int subset = SET_DIFFERENCE(set, next);
		
		cout << next << " ";
		if(subset) foo(next, subset);
	}

	void print_calcVal(int node, int set)
	{
		int val = this->D[node][set];
		fprintf(stdout, "D[v%d][{ ", node);
		for(int i = 2; i <= this->n; i++)
		{
			if(INSET(set, i))
				fprintf(stdout, "v%d ", i);
		}
		fprintf(stdout, "}] = ");

		if(val != MYINFINITY)
			fprintf(stdout, "%d\n", val);
		else
			fprintf(stdout, "Inf\n");
	}
public:

	Graph(int _n, int** _W):n(_n), W(_W)
	{
		cout << endl << "constructor called" << endl;
		this->D = new int*[this->n+1];
		this->P = new int*[this->n+1];
		
		for(int i=1; i <= this->n; i++)
		{
			D[i] = new int[1<<(this->n-1)];
			P[i] = new int[1<<(this->n-1)];
		}
	}
	
	~Graph()
	{
		cout << endl << "destructor called" << endl;

		for(int i = 1; i <= this->n; i++)
		{
			if(this->W[i]) delete this->W[i];
			if(this->D[i]) delete this->D[i];
			if(this->P[i]) delete this->P[i];
		}

		if(this->W) delete this->W;
		if(this->D) delete this->D;
		if(this->P) delete this->P;
	}

	int travel()
	{
		//k: # of nodes.
		//i: index of node.
		//j: index of node in subset.
		int k, i, j;
		int min, val, fullset;
		Node* iter;
		HashTable hash(this->n);

		//[case1] : no any intermidiate node. (k == 0)
		//The 0 of the D[i][0] means the empty set.
		for(i = 2; i <= this->n; i++)
		{
			this->D[i][0] = W[i][1];
			this->print_calcVal(i, 0);
		}
		cout << endl;

		//[case2] : at least, there is one intermediate node. except the case of (k == n-1).
		//loop for # of nodes in the set.
		for(k = 1; k <= this->n-2; k++)
		{
			//loop for all subset.
			iter = hash.table[k];
			while(iter)
			{
				//loop for vertex which is not in subset.
				for(i = 2; i <= this->n; i++)
				{
					if(INSET(iter->data, i)) continue;
					
					//loop for vertex which is in subset.
					for(min = MYINFINITY, j = 2; j <= this->n; j++)
					{
						if(!INSET(iter->data, j)) continue;
						
						if((val = this->W[i][j] + this->D[j][SET_DIFFERENCE(iter->data, j)]) < min)
						{
							min = val;
							this->P[i][iter->data] = j;
						}
					}
					this->D[i][iter->data] = min;
					this->print_calcVal(i, iter->data);
				}
				iter = iter->ptr;
			}
			cout << endl;
		}

		//[case3] : there are n-1 nodes to be passed by. (k == n-1)
		//loop for building the set with case of (k == n-1).
		for(fullset = 0, i=2; i <= this->n; i++)
			fullset |= (1<<(i-2));

		for(min = MYINFINITY, i = 2; i<= this->n; i++)
		{
			//the variable 'min' corresponds to D[1][V-{v1}]
			if((val = this->W[1][i] + this->D[i][SET_DIFFERENCE(fullset, i)]) < min)
			{
				min = val;
				this->P[1][fullset] = i;
			}
		}
		this->D[1][fullset] = min;
		this->print_calcVal(1, fullset);
		return this->D[1][fullset];
	}

	void shortest_path()
	{
		int i, fullset = 0;	
		
		for(i=2; i <= this->n; i++)
			fullset |= (1<<(i-2));
	
		cout << endl <<  "PATH: 1 ";
		foo(1,fullset);
		cout << "1 " << endl;
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
			else W[i][j] = MYINFINITY;
		}
	}

	//init table
	for(i=1; i<=nNode; i++)
	{
		memset(line, 0x00, sizeof(line));
		fgets(line, sizeof(line), fp);
		line[strlen(line)-1] = '\0';
		ptr = strtok(line, " ");
		fprintf(stdout, "[%d] ", i);
		while(ptr != NULL)
		{
			len = strlen(ptr);
			for(j=0; j<len; j++) if(ptr[j] == ':') { ptr[j] = '\0'; break; }
			W[i][idx=atoi(ptr)] = (val = atoi(ptr+j+1));
			ptr = strtok(NULL, " ");
			fprintf(stdout, "%d:%d ", idx, val);
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
	int minlength;

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

	minlength = grp->travel();

	grp->shortest_path();

	cout << "minimal length is : " << minlength << endl;
	
	delete grp;
	
	return 0;
}




