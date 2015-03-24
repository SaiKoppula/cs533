#include <iostream>
#include <lemon/graph_to_eps.h>
#include <lemon/list_graph.h>
#include <lemon/concepts/graph.h>
#include <queue>

void ourSerialBFS(ListGraph * g, int size, int init)
{
  //Declare and initialize variables for bookkeeping
  int processed[size];
  int processing[size];
  queue<int> myQ;
  ListGraph::Node n;

  for(int i = 0; i < size;i++)
  {
	   processed[i]=0;
	   processing[i]=0;
  }

  myQ.push(init);
  int curr;

  while(!myQ.empty())
  {
	   curr = myQ.front();
     myQ.pop();
     n = g->nodeFromId(curr);
     processed[curr] = 1;	//mark as processed
     for(ListGraph::IncEdgeIt e(*g,n); e != INVALID; ++e)
     {
         ListGraph::Edge edge(e);
         ListGraph::Node temp(g->oppositeNode(n,edge));
	       if(processed[g->id(temp)] == 0 && processing[g->id(temp)] == 0)
         {
		         myQ.push(g->id(temp));
		         processing[g->id(temp)] = 1;
	       }
	   }
     cout << "Queue Size: " << myQ.size() << " Queue Head: " << myQ.front() << endl;
  }


  //Sanity Check
  for(int j = 0; j < size;j++)
	    cout << processed[j]<< ",";
	cout << endl;

}
