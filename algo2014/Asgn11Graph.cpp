/*sample input 
  first line refers n ,then n inputs of edges
  6     
  0 4
  0 5
  1 4
  2 4
  2 3
  4 5
*/  
#include <bits/stdc++.h>
using namespace std;
class Graph
{
	int V;
	list <int> *adjList;
	void DFSUtil(int from,bool visited[]);
	bool isCyclicUtil(int from,bool visited[],bool recStack[]);
	bool isEvenEdges(bool recStack[]);
public:
	Graph(int V);
	void addEdge(int src,int dst);
	void printGraph();
	void DFS();
	bool isBipartite();
	bool isCyclic();
};

Graph::Graph(int V)
{
	this->V = V;
	adjList = new list<int>[V];
}
void Graph::addEdge(int src,int dst)
{
	adjList[src].push_back(dst);
	adjList[dst].push_back(src);
}
void Graph::printGraph()
{
	for(int i=0;i<V;i++)
	{
		cout<<"    "<<i<<" :  ";
		for(list<int>::iterator it = adjList[i].begin();it!=adjList[i].end();it++)
		{
			cout<<*it<<" ";
		}
		cout<<endl;
	}
}
void Graph::DFSUtil(int from,bool visited[])
{
	cout<<from<<" ";
	visited[from] = true;
	for(list <int>::iterator it = adjList[from].begin();it!=adjList[from].end();it++)
	{
		if(!visited[*it])
		{
			DFSUtil(*it,visited);
		}
	}
}
void Graph::DFS()
{
	cout<<"    ";
	bool visited[V];
	for(int i=0;i<V;i++)
	{
		visited[i] = false;
	}
	for(int i=0;i<V;i++)
	{
		if(!visited[i])
		{
			DFSUtil(i,visited);
		}
	}
	cout<<endl;
}
bool Graph::isEvenEdges(bool recStack[])
{
	int count = 0;
	cout<<"\nCyclic elements are : ";
	for(int i=0;i<V;i++)
	{
		if(recStack[i] = true)
		{
			cout<<i<<" ";
			count++;
		}
	}
	cout<<endl;
	if(count%2 == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}
bool Graph::isCyclicUtil(int v, bool visited[], bool recStack[])
{
    if(visited[v] == false)
    {
        visited[v] = true;
        recStack[v] = true;
        list<int>::iterator i;
        for(i = adjList[v].begin(); i != adjList[v].end(); ++i)
        {
            if ( !visited[*i] && isCyclicUtil(*i, visited, recStack) )
            {
            	return isEvenEdges(recStack);
            }
            else if (recStack[*i])
            {
            	return isEvenEdges(recStack);
            }
        }
 
    }
    recStack[v] = false;
    return false;
}
bool Graph::isCyclic()
{
    bool *visited = new bool[V];
    bool *recStack = new bool[V];
    for(int i = 0; i < V; i++)
    {
        visited[i] = false;
        recStack[i] = false;
    }
    for(int i = 0; i < V; i++)
    {
        if(isCyclicUtil(i, visited, recStack))
        {
        	return true;
        }
    }
    return false;
}

bool Graph::isBipartite()
{
	return isCyclic();
}
int main()
{
	int n,src,dst;
	cin>>n;
	Graph graph(n);
	for(int i=0;i<n;i++)
	{
		cin>>src>>dst;
		graph.addEdge(src,dst);
	}
	cout<<"\n+++ n = "<<n<<endl;
	cout<<"+++ Neighbour list : "<<endl;
	graph.printGraph();
	cout<<"+++ Running DFS "<<endl;
	graph.DFS();
	if(graph.isBipartite())
	{
		cout<<"The graph is Bipartite"<<endl;
	}
	else
	{
		cout<<"The graph is not Bipartite"<<endl;
	}
	return 0;
}