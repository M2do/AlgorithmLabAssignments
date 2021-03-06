/*sample input n,m, then all edges 

25
36
 2  5  
11  7
 3  4 
13 12 
 8 18 
 8 17
 1  4 
 3  1
 8  0 
 5  6 
15 14 
 0 18 
 8 16 
 9 22
 2  6
14  8
19  5 
 9 21 
21 22 
16 18 
24 12 
14 18 
24 13 
16 14
19 20 
21 23 
 8 15 
23  9 
17 18 
16  0 
16 17 
 2 20
23 22 
20  6 
15 18 
 0 15*/
#include <bits/stdc++.h>
using namespace std;
class Graph
{
	int V;
	bool* visited;
	list<int> *adjList;
	void Crawling(int from,bool visited[]);
public:
	Graph(int V);
	void addEdge(int src,int dst);
	void printGraph();
	bool checkconn();
	bool allVisited();
	int getFirstUnvisited();
	void buildHighway(int from);
};
Graph::Graph(int V)
{
	this->V = V;
	adjList = new list<int>[V];
	visited = new bool[V];
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
		cout<<"    ";
		cout<<"City "<<setw(2)<<i<<" == ";
		for(list<int>::iterator it = adjList[i].begin();it!=adjList[i].end();it++)
		{
			cout<<setw(2)<<*it<<" ";
		}
		cout<<endl;
	}
}
void Graph::Crawling(int from,bool visited[])
{
	visited[from] = true;
	for(list<int >::iterator it = adjList[from].begin();it!=adjList[from].end();it++)
	{
		if(!visited[*it])
		{
			Crawling(*it,visited);
		}
	}
}
bool Graph::checkconn()
{
	for(int i=0;i<V;i++)
	{
		visited[i] = false;
	}
	Crawling(0,visited);
	for(int i=0;i<V;i++)
	{
		if(!visited[i])
		{
			return false;
		}
	}
	return true;

}
bool Graph::allVisited()
{
	for(int i=0;i<V;i++)
	{
		if(!visited[i])
		{
			return false;
		}
	}
	return true;
}
int Graph::getFirstUnvisited()
{
	for(int i=0;i<V;i++)
	{
		if(!visited[i])
		{
			return i;
		}
	}
	return V-1;
}
void Graph::buildHighway(int from)
{
	addEdge(0,from);
	Crawling(from,visited);
	return;
}
int main()
{
	int n,m,src,dst,count = 0;
	cin>>n>>m;
	cout<<endl;
	cout<<"\n+++ n = "<<setw(2)<<n<<endl;
	cout<<"+++ m = "<<setw(2)<<m<<endl;
	Graph graph(n);
	cout<<"+++ Adding edges :"<<endl;
	cout<<"    ";
	for(int i=0;i<m;i++)
	{
		cin>>src>>dst;
		if(count == 8)
		{
			count = 0;
			cout<<endl;
			cout<<"    ";
		}
		cout<<" ("<<setw(2)<<src<<","<<setw(2)<<dst<<") ";
		graph.addEdge(src,dst);
		count++;
	}
	cout<<"\n+++ The graph is : "<<endl;
	graph.printGraph();
	if(graph.checkconn())
	{
		cout<<"\n+++ All cities are connected by highways "<<endl; 
	}
	else
	{
		cout<<"\n+++ All cities are not connected by highways "<<endl;
	}
	cout<<"\n+++ Building highways "<<endl;
	while(!graph.allVisited())
	{
		int i = graph.getFirstUnvisited();
		cout<<"    ";
		cout<<"Between City 0 and City "<<setw(2)<<i<<endl;
		graph.buildHighway(i);
	}
	cout<<"\n+++ All cities are connected by highways "<<endl;
	return 0;
}
