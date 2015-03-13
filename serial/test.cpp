#include <iostream>
#include <lemon/graph_to_eps.h>
#include <lemon/list_graph.h>
#include <lemon/full_graph.h>
#include <lemon/concepts/graph.h>
#include <queue>
#include "../lib/ourTime.h"

using namespace lemon;
using namespace std;
int g_size = 5000;
int main()
{
     /* ListDigraph g;
      ListDigraph::Node u = g.addNode();
      ListDigraph::Node v = g.addNode();
      ListDigraph::Arc  a = g.addArc(u, v);
      ListDigraph::Node w = g.addNode();*/
    FullGraph g(g_size);
    queue<int> myQ;
    /** BFS **/
    int init =5;
    FullGraph::Node n;
    int processed[g_size];
    int processing[g_size];
    for(int i = 0; i < g_size;i++){
	processed[i]=0;
	processing[i]=0;
    }
    myQ.push(init);
    int curr;
    double t1 = get_clock();
    while(!myQ.empty()){
	curr = myQ.front();
	myQ.pop();
	n = g(curr);
	processed[curr] = 1;	//mark as processed
	for(FullGraph::IncEdgeIt e(g,n); e != INVALID; ++e){
	    FullGraph::Edge edge(e);
	    FullGraph::Node temp(g.oppositeNode(n,edge));
	    if(processed[g.index(temp)] == 0 && processing[g.index(temp)] == 0){
		myQ.push(g.index(temp));
		processing[g.index(temp)] = 1;
	    }
	}
	cout<<myQ.front()<<endl;	
	/*for(int j = 0; j < g_size;j++)
	    cout << processed[j]<< ",";
	    cout << endl;*/
    }
    double t2 = get_clock();
    printf("Time: %lf seconds\n",(t2-t1));
    cout << "Hello World! This is LEMON library here." << endl;
    cout << "We have a directed graph with " << g.nodeNum() << " nodes "
         << "and " << g.arcNum() << " arc." << endl;
    return 0;
}
