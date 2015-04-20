#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <math.h>
#include <string.h>
#include <algorithm>
#include <lemon/list_graph.h>
#include <lemon/full_graph.h>
#include <lemon/concepts/digraph.h>
#include <lemon/concepts/graph.h>
#include<lemon/graph_to_eps.h>
#include <lemon/core.h>
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




int graphHasCycle(ListDigraph *g, int size)
{
    // Check if a graph has a cycle using BFS
    ListDigraph::Node u;
    int i;
    int inDegree[size];
    queue<int> sourceQ;
    for(i = 0; i < size; i++)
    {
        u = g->nodeFromId(i);
        int numIn = 0;
        for (ListDigraph::InArcIt a(*g, u); a != INVALID; ++a)
            numIn++;
        inDegree[i] = numIn;
        if (numIn == 0)
            sourceQ.push(i);
    }
    
    
    while(!sourceQ.empty()){
        int source = sourceQ.front();
        sourceQ.pop();
        
        int processed[size];
        queue<int> myQ;
    
    for(i = 0; i < size; i++)
        processed[i]=0;

    
    myQ.push(source);
    int curr;
    
    while(!myQ.empty())
    {
        i = myQ.front();
        myQ.pop();
        u = g->nodeFromId(i);
        processed[i] = 1;
        for (ListDigraph::OutArcIt a(*g, u); a != INVALID; ++a)
        {
            ListDigraph::Arc arc(a);
            ListDigraph::Node v(g->target(arc));
            int j = g->id(v);
            if(processed[j] == 0)
            {
                myQ.push(j);
                //cout << i << " -> " << j << endl;
            }
            else
            {
                //cout << "random graph has a cycle!" << endl;
                return 1;
            }
        }
    }
    }
    
    return 0;

}

