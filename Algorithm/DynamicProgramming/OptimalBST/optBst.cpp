#include <iostream>
#include <stdio.h>
#include <cstring>
#include <cstdlib>
#include <cfloat>

using namespace std;

struct Node
{
	char key[10];
	Node * left;
	Node * right;

	Node(char* k):left(NULL), right(NULL)
	{
		memset(this->key, 0x00, sizeof(this->key));
		strncpy(this->key, k, strlen(k));
	}
};

class BinarySearchTree
{
private:
	int n;
	Node* root;

	void trim(Node* rt)
	{
		if(rt->left) this->trim(rt->left);
		if(rt->right) this->trim(rt->right);
		cout << "class(Node): key(" << rt->key << ") deleted..." << endl;
		delete rt;
	}

	void inorder(Node* rt)
	{
		if(rt->left) this->inorder(rt->left);
		cout << rt->key << "\t";
		if(rt->right) this->inorder(rt->right);
	}

public:
	BinarySearchTree(int _n, Node* _ptr) : n(_n), root(_ptr)
	{
		cout << endl << "[constructor] from class(BinarySearchTree) called..." << endl;
	}
	
	~BinarySearchTree()
	{ 
		cout << endl << "[destructor] from class(BinarySearchTree) called..." << endl;
		if(root) this->trim(root);
	}

	bool search(char* k)
	{
		int ret;
		Node* it = this->root;
		
		while(it)
		{
			if(!(ret = strcmp(it->key, k))) 
				break;
			else if(ret < 0)
				it = it->right;
			else
				it = it->left;
		}

		return (it) ? true : false;
	}

	void show()
	{
		cout << endl << "[show] from class(BinarySearchTree) called..." << endl;
		this->inorder(this->root);
		cout << endl;
	}
};

struct Token
{
	char  key[10];
	float poss;

	Token() {}
	/*
	Token(): poss(0.0f)
	{ 
		memset(this->key, 0x00, sizeof(this->key));
	}
	*/
	Token(char* k, float p):poss(p)
	{
		memset(this->key, 0x00, sizeof(this->key));
		strncpy(this->key, k, strlen(k));
	}
};

class Tree_Optimizer
{
private:
	int		n;		//# of input.
	int		**R;	//Array for tree construction.
	float	**A;	//Array for dynamic programming.
	Token	*t;		//Array for input sequence.

	void sort()
	{
		int i, j, idx;
		Token temp;

		for(i = 1; i <= this->n-1; i++)
		{
			idx = i;
			for(j = i+1; j <= this->n; j++)
			{
				if(strcmp(this->t[idx].key, this->t[j].key) > 0)
					idx = j;
			}
			if(idx==i) continue;	
			temp = this->t[i];
			this->t[i] = this->t[idx];
			this->t[idx] = temp;
		}
	}

	Node* tree(int a, int b)
	{
		int k;
		Node* root;

		k = this->R[a][b];

		if(!k) return NULL;
		else
		{
			root = new Node(this->t[k].key);
			root->left = tree(a, k-1);
			root->right = tree(k+1, b);
			return root;
		}
	}
	
public:
	Tree_Optimizer(int _n, Token* _t) : n(_n), t(_t)
	{
		int i,j;
		cout << endl << "[constructor] from class(Tree_Optimizer) called..." << endl;

		//I do not use the memory of idx-0. Instead, use idx from 1 to n(input#). 
		//The idx-(n+1) is neccessary for dynamic programming.
		this->A = new float*[this->n+2];
		this->R = new int*[this->n+2];
		for(i = 1; i <= this->n + 1 ; i++)
		{
			//Likewise, i use idx from 1 to n rather than 0 to (n-1).
			this->A[i] = new float[this->n+1];
			this->A[i][i-1] = 0.0f;
			
			this->R[i] = new int[this->n+1];
			this->R[i][i-1] = 0;
		}
	}

	~Tree_Optimizer()
	{
		int i;
		cout << endl << "[destructor] from class(Tree_Optimizer) called..." << endl;

		for(i = 1; i <= this->n + 1 ; i++)
		{
			if(this->A[i]) delete this->A[i];
			if(this->R[i]) delete this->R[i];
		}

		if(this->A) delete this->A;
		if(this->R) delete this->R;
		if(this->t) delete this->t;
	}
	
	//return : minimal average searching time.
	float optimize_tree()
	{
		//i : row
		//j : col
		//k : case
		//l : diagonal
		int i, j, k, l;	
		float minima, term, comp;

		//we cannot assure the orderness of input file.
		this->sort();

		for(i=1; i <= this->n; i++)
		{
			this->A[i][i] = this->t[i].poss;
			this->R[i][i] = i;
		}

		//at first, diagonal
		for(l=1; l<=this->n-1; l++)
		{
			//at second, row and column.
			for(i = 1; i <= this->n-l; i++)
			{
				j = i + l;
				
				//at third, all possible case.
				for(minima = FLT_MAX, k = i; k <= j; k++)
				{
					comp = this->A[i][k-1] + this->A[k+1][j];
					if(minima > comp)
					{
						minima = comp;
						this->R[i][j] = k;
					}
				}

				for(term = 0.0f, k = i; k <=j ; k++) term += this->t[k].poss; 
				this->A[i][j] = minima + term;
			}
		}

		return this->A[1][this->n];
	}

	void construct_tree(BinarySearchTree ** bst)
	{
		*bst = new BinarySearchTree(this->n, tree(1, this->n));
	}
};

int read_arg_file(char** argv, Tree_Optimizer** opt)
{
	int		i,j,len,n;
	char	str1[100];
	char	str2[100];
	FILE	*fp;
	Token	*tk;
	
	if((fp = fopen(argv[1], "rt")) == NULL) return -1;
	
	fscanf(fp, "%d", &n);
	cout << "n: " << n << endl;
	tk = new Token[n+1];
	
	for(i = 1; i <= n; i++)
	{
		memset(str1, 0x00, sizeof(str1));
		memset(str2, 0x00, sizeof(str2));
		fscanf(fp, "%s %s", str1, str2);
		for(len = strlen(str2), j=0;j<len;j++)
			if(str2[j] == '/'){
				str2[j] = '\0';
				break;
			}
		tk[i] = Token(str1, (float)atoi(str2)/(float)atoi(str2+j+1));
		cout << tk[i].key << " " << tk[i].poss << endl; 
	}
	
	*opt = new Tree_Optimizer(n, tk);

	fclose(fp);

	return 0;
}

int main(int argc, char* argv[])
{
	Tree_Optimizer		*opt;
	BinarySearchTree	*bst;

	if(argc != 2)
	{
		cerr << "usage : prg input" << endl;
		exit(-1);
	}

	if(read_arg_file(argv, &opt) < 0)
	{
		cerr << "no such file " << argv[1] << endl;
		exit(-1);
	}

	cout << "minimal searching time : " << opt->optimize_tree() << endl;

	opt->construct_tree(&bst);

	bst->show();

	delete bst;
	delete opt;

	return 0;
}





