#include <iostream>
#include <lemon/graph_to_eps.h>
#include <lemon/list_graph.h>
#include <lemon/concepts/graph.h>
#include <queue>
#include <vector>
#include <unordered_map>
#include <pthread.h>
#include <vector>
#include <lemon/concepts/digraph.h>
#include <functional>
#define PRINT_ALG_ENABLE 0
#define CHECK_TOPO_ENABLE 0
#define PRINT_DIJ_ENABLE 0
// Number of Locks
#define MAGIC_NUMBER 100
pthread_mutex_t bfs_mutex[MAGIC_NUMBER];
pthread_mutex_t queue_mutex;
pthread_mutex_t	UF_mutex;
//dij
unsigned const int MAAX=99999;

void ourSerialDij(ListGraph & g, ListGraph::EdgeMap<int> & c, vector<int> & res,int size, int num_edges, int start, int dest){
	MinIndexedPQ dist(size);
	dist.insert(start,0);
//	int prev[size];
    for(int i = 0; i < size; i++){
    	if(i != start)
    		dist.insert(i,MAAX);
//    	prev[i] = -1;	
   	}
   	
   	while(1)
   	{
   		ListGraph::Node n = g.nodeFromId(dist.minIndex());
   		int curr_dist = dist.minKey();
   		for(ListGraph::IncEdgeIt e(g,n); e != INVALID; ++e){
   	    	ListGraph::Edge edge(e);
        	ListGraph::Node temp(g.oppositeNode(n,edge));
        	if(dist.keyOf(g.id(temp)) > (curr_dist + c[edge])){
        		dist.changeKey(g.id(temp), curr_dist+c[edge]);
//        		prev[g.id(temp)] = g.id(n);
//        		cout<< "Setting node " << g.id(temp) << "'s prev to be " << g.id(n)<<endl;
        	}
   	 	}
   	 	if(g.id(n) == dest)
   	 	{
   	 		cout << "The minimum Distance is " << dist.minKey() << endl;
   	 		break;
   	 	}
   	 	dist.deleteMin();
   	}
   	

    	
}
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
  	int * toE;
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
		(p->toE)[p->t_id] = p->toW;
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
	int u,v;
	for (i = 0; i < num_edges/2; ++i)
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
	int toWork = i;
	int rc;
	int toErase[num_t];
	for( int i = 0; i < num_t;i ++){
		toErase[i] = -1;
	}
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
          th_params[i].toE = toErase;
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
      int offset = 0;
      
      for( int i = 0; i < num_t;i ++){
		if(toErase[i] != -1){
			toSort.erase(toSort.begin() + toErase[i] - offset);
			offset++;
			toErase[i] = -1;
			//cout << "working!" << endl;
		}
	  }
	  //cout << "Get rid of " <<offset<< endl;
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

void ourParallelBFS(ListGraph * l, int size, int init, int max_threads)
{

  //Parameter inialization
  int visited[size];
  for (int i = 0; i < size; i++)
        visited[i] = 0;
  unordered_map<int, int> currentQ;
  unordered_map<int, int> nextQ;


  currentQ[init] = 1;
  visited[init] = 1;

  auto it = currentQ.begin();

  while(!currentQ.empty())
  {
      int rc;
      int i;
      int num_threads = (currentQ.size() > max_threads) ? max_threads : currentQ.size(); //CHANGE THIS
      //cout << currentQ.size() << " " << max_threads << " " << num_threads << " " << nextQ.size() << endl;
      pthread_t threads[num_threads];
      pthread_attr_t attr;
      void *status;

      // Initialize and set thread joinable
      pthread_attr_init(&attr);
      pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
      params * th_params = (params *) malloc (num_threads * sizeof(params));
      
    
      //auto it = currentQ.begin();
      if (nextQ.empty())
          it = currentQ.begin();
      
      for (i = 0; i < num_threads; i++) {
          th_params[i].l = l;
          th_params[i].index = it->first;
          //cout << "launch " << it->first;
          th_params[i].visited = visited;
          th_params[i].count = 0;
          rc = pthread_create(&threads[i], NULL, bfs_node, (void *)&(th_params[i]));
          if (rc){
              cout << "Error:unable to create thread," << rc << endl;
              exit(-1);
          }
          
          auto prev = it;
          it++;
          currentQ.erase(prev);
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
      
      
      
      
      
      //currentQ.clear();
      int j;
      for (i = 0; i < num_threads; i++)
      {
          for (j = 0; j < th_params[i].count; j++)
          {
              int temp = th_params[i].localQ[j];
              visited[temp] = 1;
              //currentQ[temp] = 1;
              nextQ[temp] = 1;
          }
      }
      
      if (currentQ.empty())
      {
          currentQ = nextQ;
          nextQ.clear();
      }
      

  }

}




int ourSerialTopo(ListDigraph * g, int size)
{
    //Declare and initialize variables for bookkeeping
    int inDegree[size];
    queue<int> myQ;
    queue<int> result;
    ListDigraph::Node u, v;
    int i;
    
    //Get indegrees of all nodes in the graph
    for(i = 0; i < size; i++)
    {
        u = g->nodeFromId(i);
        int numIn = 0;
        for (ListDigraph::InArcIt a(*g, u); a != INVALID; ++a)
            numIn++;
        inDegree[i] = numIn;
        if (numIn == 0)
            myQ.push(i);
    }
    
    if (myQ.empty())
    {
        cout << "ERROR: Input graph contains cycle." << endl;
        return -1;
    }
    
    //Visit the nodes with zero indegree
    while(!myQ.empty())
    {
        /*
        for (int k = 0; k < size; k++)
            cout << k << ": " << inDegree[k] << endl;
        cout << endl << endl << endl << endl;
         */
        i = myQ.front();
        myQ.pop();
        u = g->nodeFromId(i);
        for (ListDigraph::OutArcIt a(*g, u); a != INVALID; ++a)
        {
            ListDigraph::Arc arc(a);
            ListDigraph::Node v(g->target(arc));
            int j = g->id(v);
            inDegree[j]--;
            if (inDegree[j] == 0)
                myQ.push(j);
        }
        inDegree[i] = -1;
        result.push(i);
    }
    
    for(i = 0; i < size; i++)
    {
        if (inDegree[i] != -1)
        {
            cout << "ERROR: Input graph contains cycle." << endl;
            return -1;
        }
    }
    
#if PRINT_ALG_ENABLE
    
    unordered_map<int, int> test;
    i = 0;
    while(!result.empty())
    {
        #if CHECK_TOPO_ENABLE
        test[result.front()] = i;
        i++;
        #endif
        cout << result.front() << ",";
        result.pop();
    }
    cout << endl;
    
    #if CHECK_TOPO_ENABLE
    for (ListDigraph::ArcIt a(*g); a != INVALID; ++a)
    {
        int s = g->id(g->source(a));
        int t = g->id(g->target(a));
        if (test[s] >= test[t])
        {
            cout << "ERROR at " << s << " -> " << t << endl;
            //cout << test[s] << test[t] << endl;
        }
    }
    #endif
#endif
    
    return 0;
}





typedef struct init_params_t
{
    ListDigraph * g;
    int* inDegree;
    int i;
    int size;
    int num_threads;
    int num_ret;
    int ret[10000];
} init_params;

void *topo_init(void *arg)
{
    init_params * p = (init_params *) arg;
    ListDigraph * g = p->g;
    int* inDegree = p->inDegree;
    int i = p->i;
    int num_threads = p->num_threads;
    int size = p->size;
    int num_ret = 0;
    for (int k = 0; k < (size-1)/num_threads+1; k++) {
        int index = k*num_threads+i;
        if (index >= size)
            break;
        ListDigraph::Node u = g->nodeFromId(index);
        int numIn = 0;
        for (ListDigraph::InArcIt a(*g, u); a != INVALID; ++a)
            numIn++;
        inDegree[index] = numIn;
        if (numIn == 0) {
            p->ret[num_ret] = index;
            num_ret++;
        }
    }
    p->num_ret = num_ret;
    pthread_exit(NULL);
}



int ourParallelTopo(ListDigraph * g, int size, int num_threads)
{
    //Declare and initialize variables for bookkeeping
    int inDegree[size];
    queue<int> myQ;
    queue<int> result;
    ListDigraph::Node u, v;
    
    int rc;
    int i, j, k;
    pthread_t initThreads[num_threads];
    pthread_attr_t attr;
    void *status;
    
    // Initialize and set thread joinable
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    init_params * initParams = (init_params *) malloc (num_threads * sizeof(init_params));

    
        for (i = 0; i < num_threads; i++) {
            initParams[i].g = g;
            initParams[i].inDegree = inDegree;
            initParams[i].i = i;
            initParams[i].size = size;
            initParams[i].num_threads = num_threads;
            rc = pthread_create(&initThreads[i], NULL, topo_init, (void *)&(initParams[i]));
            if (rc){
                cout << "Error:unable to create thread," << rc << endl;
                exit(-1);
            }
        }
        
        // free attribute and wait for the other threads
        pthread_attr_destroy(&attr);
        
        for(i = 0; i < num_threads; i++){
            rc = pthread_join(initThreads[i], &status);
            if(rc){
                cout << "Error:unable to join," << rc << endl;
                exit(-1);
            }
        }
    
    
    for (i = 0; i < num_threads; i++)
    {
        for (j = 0; j < initParams[i].num_ret; j++)
            myQ.push(initParams[i].ret[j]);
    }
    
    
    if (myQ.empty())
    {
        cout << "ERROR: Input graph contains cycle." << endl;
        return -1;
    }
    
    //Visit the nodes with zero indegree
    while(!myQ.empty())
    {
        i = myQ.front();
        myQ.pop();
        u = g->nodeFromId(i);
        for (ListDigraph::OutArcIt a(*g, u); a != INVALID; ++a)
        {
            ListDigraph::Arc arc(a);
            ListDigraph::Node v(g->target(arc));
            int j = g->id(v);
            inDegree[j]--;
            if (inDegree[j] == 0)
                myQ.push(j);
        }
        inDegree[i] = -1;
        result.push(i);
    }
    
    for(i = 0; i < size; i++)
    {
        if (inDegree[i] != -1)
        {
            //cout << "ERROR: Input graph contains cycle." << endl;
            return -1;
        }
    }
    
#if PRINT_ALG_ENABLE
    
    unordered_map<int, int> test;
    i = 0;
    while(!result.empty())
    {
#if CHECK_TOPO_ENABLE
        test[result.front()] = i;
        i++;
#endif
        cout << result.front() << ",";
        result.pop();
    }
    cout << endl;
    
#if CHECK_TOPO_ENABLE
    for (ListDigraph::ArcIt a(*g); a != INVALID; ++a)
    {
        int s = g->id(g->source(a));
        int t = g->id(g->target(a));
        if (test[s] >= test[t])
        {
            cout << "ERROR at " << s << " -> " << t << endl;
            //cout << test[s] << test[t] << endl;
        }
    }
#endif
#endif
    
    return 0;

    

}



