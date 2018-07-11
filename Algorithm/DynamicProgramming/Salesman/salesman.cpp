#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <stdio.h>

#define MYINFINITY				0x3FFFFFFF
#define INSET(s,v)				(s&(1<<(v-2)))
#define SET_DIFFERENCE(s,v)		(s&(~(1<<(v-2))))

using namespace std;

struct Node
{
	int   data;
	Node* ptr;

	//Node(): ptr(NULL) {}
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

		this->tsize = (1 << (this->nNode-1));
		this->table = new Node*[this->tsize];
		this->mktable();
	}

	~HashTable()
	{
		cout << endl << "hash table eleminated" << endl;

		Node* temp;

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

	//input : integer number
	//output : number of bit1.
	int hash_func(int idx)
	{
		int cnt = 0;
		
		//number of bits equals to number of nodes (nNode).	
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
	int n;		//number of vertex
	int** W;	//weight matrix
	int** D;	//array for DP

public:

	Graph(int _n, int** _W):n(_n), W(_W)
	{
		cout << endl << "constructor called" << endl;
		this->D = new int*[this->n+1];
		memset(this->D, 0x00, sizeof(int*) * (this->n+1));
		for(int i=2; i <= this->n; i++)
		{
			D[i] = new int[1<<(this->n-1)];
		}
	}
	
	~Graph()
	{
		cout << endl << "destructor called ..." << endl;

		for(int i = 1; i <= this->n; i++)
		{
			if(this->W[i]) delete this->W[i];
			if(this->D[i]) delete this->D[i];
		}

		if(this->W) delete this->W;
		if(this->D) delete this->D;
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

		//case : no any intermidiate node. (k == 0)
		//The 0 of the D[i][0] means the empty set.
		for(i = 2; i <= this->n; i++)
			this->D[i][0] = W[i][1];

		//case : at least, there is one intermediate node.
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
						}
					}
					this->D[i][iter->data] = min;
				}
				iter = iter->ptr;
			}
		}

		//case : there are n-1 nodes to be passed by. (k == n-1)
		for(fullset = 0, i=0; i<= this->n-2; i++)
			fullset |= (1<<i);

		for(min = MYINFINITY, j = 2; j<= this->n; j++)
		{
			if((val = this->W[1][j] + this->D[j][SET_DIFFERENCE(fullset, j)]) < min)
				min = val;
		}

		//min : D[1][V-{v1}]
		return min;
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

	delete grp;

	cout << "minimal length is : " << minlength << endl;
	
	return 0;
}




