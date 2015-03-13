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
using namespace lemon;

using namespace std;

FullGraph g;
int size;
int init;
unordered_map<int, int> visited;
queue<int> Q1;
queue<int> Q2;


void random_graph(ListGraph* g, int size)
{
    int prob = 3;
    
    for (int i = 0; i < size; i++)
        g->addNode();
    
    for (ListGraph::NodeIt u(*g); u != INVALID; ++u)
    {
        for (ListGraph::NodeIt v(*g); v != INVALID; ++v)
        {
            if (u != v)
            {
                if (rand() % prob == 0)
                    g->addEdge(u, v);
            }
        }
    }
}



void *bfs_node(void *arg)
{
    int index = * (int*)arg;
    FullGraph::Node n = g(index);
    //cout << "index: " << index << endl;
    
    for(FullGraph::IncEdgeIt e(g,n); e != INVALID; ++e){
        //cout << "haha" << endl;
        FullGraph::Edge edge(e);
        FullGraph::Node temp(g.oppositeNode(n,edge));
        int i = g.index(temp);
        //sychronization??
        if(!visited[i]){
            visited[i]++;
            cout << "visit " << i << endl;
            Q2.push(i);
        }
    }
    pthread_exit(NULL);
}


int main()
{
    //initialize
    size = 50000;
    init = 5;
    g.resize(size);
    
    Q1.push(init);
    visited[init] = 1;
    cout << "initial node: " << init << endl;

    while(!Q1.empty())
    {
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
            Q1.pop();
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
        
        while(!Q2.empty())
            Q2.pop();
    }
    
    
    
    return 0;
}
