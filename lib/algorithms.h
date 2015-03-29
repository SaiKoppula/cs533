#include <iostream>
#include <lemon/graph_to_eps.h>
#include <lemon/list_graph.h>
#include <lemon/concepts/graph.h>
#include <queue>
#include <vector>
#include <unordered_map>
#include <pthread.h>
#include <vector>
#define PRINT_ALG_ENABLE 0



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
     #if PRINT_ALG_ENABLE
     cout << "Queue Size: " << myQ.size() << " Queue Head: " << myQ.front() << endl;
     #endif
  }


  //Sanity Check
  #if PRINT_ALG_ENABLE
  for(int j = 0; j < size;j++)
	    cout << processed[j]<< ",";
	cout << endl;
  #endif
}



typedef struct params_t
{
	ListGraph * l;
    int index;
    int* visited;
    int localQ[1000];
    int count;
} params;

void *bfs_node(void *arg)
{
    params * p = (params *) arg;
    int index = p->index;
    ListGraph::Node n = p->l->nodeFromId(index);
    for(ListGraph::IncEdgeIt e(*(p->l),n); e != INVALID; ++e){
        ListGraph::Edge edge(e);
        ListGraph::Node temp((p->l)->oppositeNode(n,edge));
        int i = p->l->id(temp);
        if(!p->visited[i]){
            #if PRINT_ALG_ENABLE
            cout << "visit " << i << endl;
            #endif
            p->localQ[p->count] = i;
            p->count++;
        }
    }
    pthread_exit(NULL);
}

void ourParallelBFS(ListGraph * l, int size, int init)
{

  //Parameter inialization
  int visited[size];
    for (int i = 0; i < size; i++)
        visited[i] = 0;
  unordered_map<int, int> currentQ;


  currentQ[init] = 1;
  visited[init] = 1;


  while(!currentQ.empty())
  {
      int rc;
      int i;
      int num_threads = currentQ.size(); //CHANGE THIS
      pthread_t threads[num_threads];
      pthread_attr_t attr;
      void *status;

      // Initialize and set thread joinable
      pthread_attr_init(&attr);
      pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
      params * th_params = (params *) malloc (num_threads * sizeof(params));
      
      auto it = currentQ.begin();
      for (i = 0; i < num_threads; i++) {
          th_params[i].l = l;
          th_params[i].index = it->first;
          th_params[i].visited = visited;
          th_params[i].count = 0;
          rc = pthread_create(&threads[i], NULL, bfs_node, (void *)&(th_params[i]));
          if (rc){
              cout << "Error:unable to create thread," << rc << endl;
              exit(-1);
          }
          it++;
      }
      

      // free attribute and wait for the other threads
      pthread_attr_destroy(&attr);
      for(i = 0; i < num_threads; i++){
          rc = pthread_join(threads[i], &status);
          if(rc){
              cout << "Error:unable to join," << rc << endl;
              exit(-1);
          }
          #if PRINT_ALG_ENABLE
          cout << "Main: completed thread id :" << i ;
          cout << "  exiting with status :" << status << endl;
          #endif
      }
      
      
      currentQ.clear();
      int j;
      for (i = 0; i < num_threads; i++)
      {
          for (j = 0; j < th_params[i].count; j++)
          {
              int temp = th_params[i].localQ[j];
              visited[temp] = 1;
              currentQ[temp] = 1;
          }
      }
      

  }

}
