#include <pthread.h>
#include <iostream>
#include<lemon/graph_to_eps.h>
#include <lemon/list_graph.h>
#include <lemon/full_graph.h>
#include <lemon/concepts/graph.h>
#include <queue>
#include <vector>
#include <unordered_map>
#include <pthread.h>
#include <vector>
#include "../lib/GraphGen.h"
#include "../lib/support.h"
#include "../lib/algorithms.h"

#define ARGS 3
#define TIMER_ENABLE 1
#define PRINT_MST 0

using namespace lemon;
using namespace std;

void usage()
{
    printf("USAGE: ./bfs [number of nodes] [number of threads]");
}

int main(int argc, char * argv[])
{

    //Param Check
    if (argc != ARGS)
    {
        usage();
        return -1;
    }
    
    //typedef
    typedef lemon::ListGraph Graph;
 	typedef Graph::EdgeIt EdgeIt;
  	typedef Graph::Edge Edge;
  	typedef Graph::NodeIt NodeIt;
  	typedef Graph::Node Node;
  	typedef Graph::EdgeMap<int> costMap;
  	using lemon::INVALID;

  	Graph g;
  	
  	int size;
    int num_threads;
	int num_edges=0;
    size = atoi(argv[1]);
    num_threads = atoi(argv[2]);
    //Build graph -- Template uses Dao's Random Graph Generator
    random_graph(&g, size, size/10, size/2, &num_edges);
    #if PRINT_MST
	cout << "NumEdges: "<< num_edges<<endl;
	#endif
	costMap c(g);
	srand(time(NULL));
	for(int i = 0; i < num_edges; i ++)
	{
		Edge e = Graph::edgeFromId(i);
		c[e] = rand() % 8 + 2;
		#if PRINT_MST
		cout <<"("<< g.id(g.u(e))<<","<<g.id(g.v(e))<<")"<<",Value: "<<c[e]<<endl;
		#endif
	}
	
	vector<int> res_edge_id_serial;
	vector<int> res_edge_id_parallel;
	
	/* Time the serial execution */
	double t1 = get_clock();
	ourSerialMST(g, c, res_edge_id_serial, size, num_edges);
	double t2 = get_clock();
    printf("Serial Time: %lf seconds\n",(t2-t1));
	/* Time the parallel execution */
	double t3 = get_clock();
	ourParallelMST(g, c, res_edge_id_parallel, size, num_edges,num_threads);
	double t4 = get_clock();
    printf("Parallel Time: %lf seconds\n",(t4-t3));
    
    
	#if PRINT_MST
	cout << "==========================Parallel RESULT=========================="<<endl;
	for(int i = 0; i < res_edge_id_parallel.size();i ++){
		Edge a = Graph::edgeFromId(res_edge_id_parallel[i]);
		cout <<"("<< g.id(g.u(a))<<","<<g.id(g.v(a))<<")"<<",Value: "<<c[a]<<endl;
	}
	cout<<"Total Number of Edges: " << res_edge_id_parallel.size() << endl;
	cout << "==========================Serial RESULT=========================="<<endl;
	for(int i = 0; i < res_edge_id_serial.size();i ++){
		Edge a = Graph::edgeFromId(res_edge_id_serial[i]);
		cout <<"("<< g.id(g.u(a))<<","<<g.id(g.v(a))<<")"<<",Value: "<<c[a]<<endl;
	}
	
	cout<<"Total Number of Edges: " << res_edge_id_serial.size() << endl;
	#endif
}
