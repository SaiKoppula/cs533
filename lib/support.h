#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <math.h>
#include <string.h>
#include <algorithm>
using namespace std;
struct timeval tv; 
double get_clock() {
    struct timeval tv; int ok;
    ok = gettimeofday(&tv, NULL);
    if (ok<0) { printf("gettimeofday error");  }
    return (tv.tv_sec * 1.0 + tv.tv_usec * 1.0E-6); 
}

class Union_Find
{
  public:
 
	int * id;
	int * sz;
	Union_Find(int n)	//class constructor
	{
		id = new int[n];
		sz = new int[n];
		for (int i = 0; i < n; ++i)
		{
			id[i] = i;
			sz[i] = 1;
		}
	}
	
	int root(int i)
	{
		while(i != id[i])
		{
			id[i] = id[id[i]];	//path Compression
			i = id[i];
		}
		return i;
	}
	int find(int p, int q)
	{
		return root(p)==root(q);
	}
	void unite(int p, int q)
	{
		int i = root(p);
		int j = root(q);
 
		if(sz[i] < sz[j])
		{
			id[i] = j;
			sz[j] += sz[i];
		}
		else
		{
			id[j] = i;
			sz[i] += sz[j];
		}
	}
	~Union_Find(){
		free(id);
		free(sz);
	}
};
