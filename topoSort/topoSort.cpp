#include <pthread.h>
#include <iostream>
#include<lemon/graph_to_eps.h>
#include <lemon/list_graph.h>
#include <lemon/full_graph.h>
#include <lemon/concepts/digraph.h>
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

using namespace lemon;
using namespace std;

void usage()
{
    printf("USAGE: ./topoSort [number of nodes] [number of threads]");
}

int main(int argc, char * argv[])
{
    
    //Param Check
    if (argc != ARGS)
    {
        usage();
        return -1;
    }
    
    //initialize variables
    ListDigraph g;
    int size;
    int num_threads;
    size = atoi(argv[1]);
    num_threads = atoi(argv[2]);
    
    //Build a dag -- a path for testing
    ListDigraph::Node x = g.addNode();
    ListDigraph::Node y = g.addNode();
    ListDigraph::Node z = g.addNode();
    g.addArc(x,y);
    g.addArc(y,z);
    
#if TIMER_ENABLE
    double t1 = get_clock();
#endif
    //Do Computation
    ourSerialTopo(&g, size);
#if TIMER_ENABLE
    double t2 = get_clock();
    printf("Serial Time: %lf seconds\n",(t2-t1));
#endif
    
    
    
#if TIMER_ENABLE
    double t3 = get_clock();
#endif
    //Do Computation
    ourParallelTopo(&g, size);
#if TIMER_ENABLE
    double t4 = get_clock();
    printf("Parallel Time: %lf seconds\n",(t4-t3));
#endif
    
    
    return 0;
}