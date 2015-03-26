#include <iostream>
#include <lemon/graph_to_eps.h>
#include <lemon/list_graph.h>
#include <lemon/concepts/graph.h>
#include <queue>
#include <vector>
#include <unordered_map>
#include <pthread.h>
#include <vector>
#define PRINT_ALG_ENABLE 1

pthread_mutex_t bfs_mutex;

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
  unordered_map<int, int> *visited;
  queue<int> *Q1;
  queue<int> *Q2;
} params;

void *bfs_node(void *arg)
{
    params * p = (params *) arg;
    int index = p->index;
    ListGraph::Node n = p->l->nodeFromId(index);
    //cout << "index: " << index << endl;
    for(ListGraph::IncEdgeIt e(*(p->l),n); e != INVALID; ++e){
        //cout << "haha" << endl;
        ListGraph::Edge edge(e);
        ListGraph::Node temp((p->l)->oppositeNode(n,edge));
        int i = p->l->id(temp);
        //sychronization??
        pthread_mutex_lock(&bfs_mutex);
        if(!(*(p->visited))[i]){
            (*(p->visited))[i] = 1;
            #if PRINT_ALG_ENABLE
            cout << "visit " << i << endl;
            #endif
            p->Q2->push(i);
        }
        pthread_mutex_unlock(&bfs_mutex);
    }
    pthread_exit(NULL);
}

void ourParallelBFS(ListGraph * l, int size, int init)
{

  //Parameter inialization
  unordered_map<int, int> visited;
  queue<int> Q1;
  queue<int> Q2;


  Q1.push(init);
  visited[init] = 1;


  while(!Q1.empty())
  {
      int rc;
      int i;
      int num_threads = Q1.size(); //CHANGE THIS
      pthread_t threads[num_threads];
      pthread_attr_t attr;
      void *status;

      // Initialize and set thread joinable
      pthread_mutex_init(&bfs_mutex, NULL);
      pthread_attr_init(&attr);
      pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
      params * th_params = (params *) malloc (num_threads * sizeof(params));
      
      for(i = 0; i < num_threads; i++){
          //cout << "main() : creating thread, " << i << endl;
          th_params[i].l = l;
		      th_params[i].index = Q1.front();;
          th_params[i].Q1 = &Q1;
		      th_params[i].Q2 = &Q2;
          th_params[i].visited = &visited;
          rc = pthread_create(&threads[i], NULL, bfs_node, (void *)&(th_params[i]));
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
          #if PRINT_ALG_ENABLE
          cout << "Main: completed thread id :" << i ;
          cout << "  exiting with status :" << status << endl;
          #endif
      }



      Q1.swap(Q2);

      while(!Q2.empty())
          Q2.pop();
  }

}
