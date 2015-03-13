#include <pthread.h>
#include <iostream>
#include<lemon/graph_to_eps.h>
#include <lemon/list_graph.h>
#include <lemon/full_graph.h>
#include <lemon/concepts/graph.h>
#include <queue>
#include <vector>
#include <unordered_map>
#include <deque>
using namespace lemon;
using namespace std;

ListGraph g;
int size;
int init;
unordered_map<int, int> visited;
deque<int> Q1;
deque<int> Q2;


void random_graph(ListGraph* lg, int size)
{
    cout << "------------------------------------------------------------" << endl;
    cout << "                      a ring with " << size << " nodes                     " << endl;
    
    int prob = 5;
    
    for (int i = 0; i < size; i++)
    {
        ListGraph::Node n = lg->addNode();
    }
    
    for (ListGraph::NodeIt u(*lg); u != INVALID; ++u)
    {
        for (ListGraph::NodeIt v(*lg); v != INVALID; ++v)
        {
            // create a ring!
            if ((lg->id(u) == lg->id(v) - 1)||(lg->id(u) == 0 && lg->id(v) == size-1))
                lg->addEdge(u, v);
            
            // create a random graph P[an edge exists] = 1/prob
            /*
            if (u != v)
            {
                if (rand() % prob == 0)
                {
                    lg->addEdge(u, v);
                    //cout << lg->id(u) << " <-> " << lg->id(v) << endl;
                }
            }
             */
        }
    }
    
    // connect all nodes
    /*
    for (ListGraph::NodeIt u(*lg); u != INVALID; ++u)
    {
        ListGraph::IncEdgeIt e(*lg,u);
        if (e == INVALID)
        {
            cout << "unconnected node: " << lg->id(u) << endl;
        }
    }
    */
    cout << "------------------------------------------------------------" << endl;
}



void *bfs_node(void *arg)
{
    int index = * (int*)arg;
    ListGraph::Node n = g.nodeFromId(index);
    //cout << "index: " << index << endl;
    
    for(ListGraph::IncEdgeIt e(g,n); e != INVALID; ++e){
        //cout << "haha" << endl;
        ListGraph::Edge edge(e);
        ListGraph::Node temp(g.oppositeNode(n,edge));
        int i = g.id(temp);
        //sychronization??
        if(!visited[i]){
            visited[i]++;
            //cout << "visit " << i << endl;
            Q2.push_back(i);
        }
    }
    pthread_exit(NULL);
}


int main()
{
    //initialize
    size = 50;
    init = size/2;
    random_graph(&g, size);
    
    Q1.push_back(init);
    visited[init] = 1;
    cout << "initial node: " << init << endl;
    int depth = 1;

    while(!Q1.empty())
    {
        cout << "--------------------- depth = " << depth << "--------------------" << endl;
        depth++;
        
        int rc;
        int i;
        int num_threads = Q1.size();
        pthread_t threads[num_threads];
        pthread_attr_t attr;
        void *status;
        
        // Initialize and set thread joinable
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
        
        for(i = 0; i < num_threads; i++){
            //cout << "main() : creating thread, " << i << endl;
            rc = pthread_create(&threads[i], NULL, bfs_node, (void *)&Q1.front());
            Q1.pop_front();
            if (rc){
                cout << "Error:unable to create thread," << rc << endl;
                exit(-1);
            }
        }

        // free attribute and wait for the other threads
        pthread_attr_destroy(&attr);
        for(i = 0; i < num_threads; i++){
            rc = pthread_join(threads[i], &status);
            if(rc){
                cout << "Error:unable to join," << rc << endl;
                exit(-1);
            }
            //cout << "Main: completed thread id :" << i ;
            //cout << "  exiting with status :" << status << endl;
        }


        
        Q1.swap(Q2);
        
        for (i = 0; i < Q1.size(); i++)
        {
            cout << "visit " << Q1[i] << endl;
        }
    }
    
    
    
    return 0;
}
