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
  int edges[size][size];
	vector<ListGraph::Node> vNode;
	for(int i = 0; i < size; i++){
		vNode.push_back(g->addNode());
		numArcs[i]=0;
    for(int j = 0; j < size; j++) edges[i][j] = 0;
	}
	int counter = 0;
	for (ListGraph::NodeIt u(*g); u != INVALID; ++u)
    {
    srand(time(NULL));
		int sel;
		//cout << "Working on Node " << counter<<endl;
		ListGraph::Node n(u);
		int numArc = rand() % (highB-lowB+1 - (highB/2)) + lowB;  //get a number between lowB and highB
		cout << "Generating " << numArc << " edges for node " << g->id(u) << endl;
    while(numArcs[g->id(u)] < numArc){

			srand(time(NULL));
			do{
		    	sel = rand() % size;
			}
			while(sel == g->id(u) || numArcs[sel] >= highB || edges[g->id(u)][sel] == 1);
			g->addEdge(u,g->nodeFromId(sel));
      edges[g->id(u)][sel] = 1;
      edges[sel][g->id(u)] = 1;
			numArcs[g->id(u)]++;
			numArcs[sel]++;
		}

    }

  for (ListGraph::NodeIt u(*g); u != INVALID; ++u)
  {
    cout << "Edges for Node " << g->id(u) << ": ";
    for(ListGraph::IncEdgeIt e(*g, u); e != INVALID; ++e)
    {
      cout << g->id(g->oppositeNode(u, e)) << " ";
    }
    cout << endl;
  }

	//debug
    for(int i = 0;i < size;i++)
		cout << numArcs[i] << " ";
    cout << endl;
};

void full_graph(ListGraph * g, int size)
{
  int edges[size][size];
  int numArcs[size];
	vector<ListGraph::Node> vNode;
	for(int i = 0; i < size; i++){
		vNode.push_back(g->addNode());
		numArcs[i]=0;
    for(int j = 0; j < size; j++) edges[i][j] = 0;
	}
	int counter = 0;
	for (ListGraph::NodeIt u(*g); u != INVALID; ++u)
    {
      for (ListGraph::NodeIt v(*g); v != INVALID; ++v)
        if (g->id(u) != g->id(v) && edges[g->id(u)][g->id(v)] == 0)
        {
          g->addEdge(u,v);
          edges[g->id(u)][g->id(v)] = 1;
          edges[g->id(v)][g->id(u)] = 1;
          numArcs[g->id(u)]++;
          numArcs[g->id(v)]++;
        }
    }

  for (ListGraph::NodeIt u(*g); u != INVALID; ++u)
  {
    cout << "Edges for Node " << g->id(u) << ": ";
    for(ListGraph::IncEdgeIt e(*g, u); e != INVALID; ++e)
    {
      cout << g->id(g->oppositeNode(u, e)) << " ";
    }
    cout << endl;
  }

	//debug
    for(int i = 0;i < size;i++)
		cout << numArcs[i] << " ";
    cout << endl;
};
