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
// Number of Locks
#define MAGIC_NUMBER 100
pthread_mutex_t bfs_mutex[MAGIC_NUMBER];
pthread_mutex_t queue_mutex;
pthread_mutex_t	UF_mutex;

void ourSerialMST(ListGraph & g, ListGraph::EdgeMap<int> & c, vector<int> & res,int size, int num_edges)
{
	Union_Find UF(size);
	int i;
	vector<pair<int,int>> toSort;  //optimization here?
	toSort.resize(num_edges);
	for(i = 0 ; i < num_edges; i ++){
		ListGraph::Edge e = ListGraph::edgeFromId(i);
		toSort[i] = pair<int,int>(c[e],i);
	}
	std::sort(toSort.begin(), toSort.end());
	
	int u, v;
 
	for (i = 0; i < num_edges; ++i)
	{
		
		u = g.id(g.u(ListGraph::edgeFromId(toSort[i].second)));
		v = g.id(g.v(ListGraph::edgeFromId(toSort[i].second)));
		#if PRINT_ALG_ENABLE
		cout<< "("<<toSort[i].first<<","<<toSort[i].second<<")"<<endl;
		cout<< "u,v :("<<u<<","<<v<<")"<<endl;
		#endif
		if( !UF.find(u, v) )
		{
			UF.unite(u, v);
			res.push_back(toSort[i].second);
		}
	}
		
}
typedef struct params_t_mst
{
	ListGraph * gr;
	vector<pair<int,int>> * toS;
  	int t_id;
  	int toW;
  	Union_Find * uf;
  	vector<int> * resV;
} params_m;

void *mst_node(void *arg)
{
    params_m * p = (params_m *) arg;

    int u, v;
    u = (p->gr)->id((p->gr)->u(ListGraph::edgeFromId((*(p->toS))[p->toW].second)));
	v = (p->gr)->id((p->gr)->v(ListGraph::edgeFromId((*(p->toS))[p->toW].second)));
	if(!(*(p->uf)).find(u, v) && p->t_id == 0)
	{
		(*(p->uf)).unite(u, v);
		(*(p->resV)).push_back((*(p->toS))[p->toW].second);
	}
	else if((*(p->uf)).find(u, v) && p->t_id != 0){
		pthread_mutex_lock(&UF_mutex);
		(*(p->toS)).erase((*(p->toS)).begin() + p->toW);
		pthread_mutex_unlock(&UF_mutex);
		// cout<< "Erased 1"<<endl;
	}
	//else
//		pthread_mutex_unlock(&UF_mutex);
	pthread_exit(NULL);
		
}
void ourParallelMST(ListGraph & g, ListGraph::EdgeMap<int> & c, vector<int> & res,int size, int num_edges, int num_t)
{
	Union_Find UF(size);
	int i;
	vector<pair<int,int>> toSort;  //optimization here?
	toSort.resize(num_edges);
	for(i = 0 ; i < num_edges; i ++){
		ListGraph::Edge e = ListGraph::edgeFromId(i);
		toSort[i] = pair<int,int>(c[e],i);
	}
	std::sort(toSort.begin(), toSort.end());
	pthread_mutex_init(&(UF_mutex), NULL);
	int toWork = 0;
	int rc;
	params_m * th_params = (params_m *) malloc (num_t * sizeof(params_m));
	while(toWork < toSort.size()){
	  int num_threads = min(num_t,(int)toSort.size() - toWork);
	  pthread_t threads[num_threads];
      pthread_attr_t attr;
      void *status;

      // Initialize and set thread joinable
      pthread_attr_init(&attr);
      pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
      
     
      for(i = 0; i < num_threads; i++){
          //cout << "main() : creating thread, " << i << endl;
		  th_params[i].gr = &g;
		  th_params[i].t_id = i;
		  th_params[i].uf = &UF;
          th_params[i].toS = &toSort;
          th_params[i].toW = toWork +  i * (size - toWork) / num_threads; //calculate the node to examine
          #if PRINT_ALG_ENABLE
          cout << "Thread " << i <<" working on " << th_params[i].toW << endl;
          
          if(i==0){
          	      cout << "Thread " << i <<" working on " << th_params[i].toW << endl;
          	      cout << "Size " << toSort.size() << endl;
          }
          #endif
          th_params[i].resV = & res;
          rc = pthread_create(&threads[i], NULL, mst_node, (void *)&(th_params[i]));
          if (rc){
              cout << "Error:unable to create thread," << rc << endl;
              exit(-1);
          }
      }

      //free attribute and wait for the other threads
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
      toWork++; 
     
	}


}



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
    int localQ[10000];
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
