#include <iostream>
#include <lemon/graph_to_eps.h>
#include <lemon/list_graph.h>
#include <lemon/full_graph.h>
#include <lemon/concepts/graph.h>
#include <vector>
using namespace std;
using namespace lemon;
/**
*   Noob Dao's version of generating a random undirected
*	graph, each node will have number of arcs in a given
*	range
*	
*	Parameters:
*		size: Number of nodes in the graph
*		lowb: The least number of arcs each node has
*		highB the largest number of arcs each node has
**/
void random_graph(ListGraph * g, int size, int lowB, int highB)
{
    int numArcs[size];
	vector<ListGraph::Node> vNode;
	for(int i = 0; i < size; i++){
		vNode.push_back(g->addNode());
		numArcs[i]=0;
	}
	int counter = 0;
	for (ListGraph::NodeIt u(*g); u != INVALID; ++u)
    {
		int sel;
		//cout << "Working on Node " << counter<<endl;
		ListGraph::Node n(u);
		int numArc = rand() % (highB-lowB+1 - (highB/2)) + lowB;  //get a number between lowB and highB
		while(numArcs[counter] < numArc){
		    
			srand(time(NULL));
			do{
		    	sel = rand() % size;
			}
			while(sel != counter && numArcs[sel] < highB);
			g->addEdge(n,vNode[sel]);
			numArcs[counter]++;
			numArcs[sel]++;
		}
		counter++;
    }
	//debug
    for(int i = 0;i < size;i++)
		cout << numArcs[i] << " ";
    cout << endl;
};
