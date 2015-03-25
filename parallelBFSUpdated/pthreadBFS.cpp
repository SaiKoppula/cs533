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

    //initialize variables
    ListGraph l;
    int size;
    int num_threads;
    size = atoi(argv[1]);
    num_threads = atoi(argv[2]);

    //Build graph -- Template uses Dao's Random Graph Generator
    random_graph(&l, size, size/10, size/2);

    #if TIMER_ENABLE
      double t1 = get_clock();
    #endif
    //Do Computation
    ourParallelBFS(&l, size, 0);
    #if TIMER_ENABLE
      double t2 = get_clock();
      printf("Time: %lf seconds\n",(t2-t1));
    #endif

    return 0;
}
