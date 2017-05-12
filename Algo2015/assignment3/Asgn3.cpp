/*sample input
10
40 69 50 85 84 26 74 8 3 4
5
18 2 47 79 26
5
*/
#include <bits/stdc++.h>
using namespace std;
#define maxValue 1000
class priorityQueue
{
	int* arr;
	int* minArray;
	int size;
	int capacity;
	void swap(int* a, int* b);
	int getMinBranch(int from);
	int min(int a, int b, int c);
public:
	priorityQueue();
	void insertKey(int num);
    void MaxHeapify(int from);
    int left(int i) { return (2*i + 1); }
    int right(int i) { return (2*i + 2); }
    void initMin();
    void printQueue();
    void deleteMax(int newNumDel);
    /*
    int parent(int i) { return (i-1)/2; }
    int extractMin();
    void decreaseKey(int i, int new_val);
    int getMin() { return arr[0]; }
    void deleteKey(int num);
    */
};
priorityQueue::priorityQueue()
{
	size = 0;
	capacity = maxValue;
	arr = new int[maxValue];
}
void priorityQueue::insertKey(int key)
{
	arr[size++] = key;
}
void priorityQueue::swap(int* a, int* b)
{
	int temp = *a;
	*a = *b;
	*b = temp;
}
void priorityQueue::MaxHeapify(int from)
{
	int largest = from;
	int leftChild = left(from);
	int rightChild = right(from);

	if(leftChild < size && arr[leftChild] > arr[largest])
	{
		largest = leftChild;
	}
	if(rightChild < size && arr[rightChild] > arr[largest])
	{
		largest = rightChild;
	}

	if(largest != from)
	{
		swap(&arr[largest],&arr[from]);
		MaxHeapify(largest);
	}
}
int priorityQueue::min(int a, int b, int c)
{
	int minValue = a;
	if(b < minValue)
	{
		minValue = b;
	}
	if(c < minValue)
	{
		minValue = c;
	}
	return minValue;
}
int priorityQueue::getMinBranch(int from)
{
	if(from >= size)
	{
		return INT_MAX;
	}
	int leftChild = 2*from + 1;
	int rightChild = 2*from + 2;
	int lmin = getMinBranch(leftChild);
	int rmin = getMinBranch(rightChild);
	return min(arr[from],lmin,rmin);
}
void priorityQueue::initMin()
{
	minArray = new int[size];
	for(int i=0;i<size;i++)
	{
		minArray[i] = getMinBranch(i);
	}
}
void priorityQueue::printQueue()
{
	for(int i=0;i<size;i++)
	{
		int leftChild = 2*i + 1;
		int rightChild = 2*i + 2;
		cout<<"    "<<setw(2)<<arr[i]<<", "<<setw(2)<<minArray[i]<<"  ";
		if(leftChild >= size && rightChild >= size)
		{
			cout<<"( "<<"   _   "<<": "<<"   _   "<<")"<<endl;
			continue;
		}
		cout<<"( ";
		if(leftChild < size)
		{
			cout<<setw(2)<<arr[leftChild]<<", "<<setw(2)<<minArray[leftChild];
		}
		else
		{
			cout<<" :    _  ";
		}
		if(rightChild < size)
		{
			cout<<" : "<<setw(2)<<arr[rightChild]<<", "<<setw(2)<<minArray[rightChild];
		}
		else
		{
			cout<<" :    _  ";
		}
		cout<<" )"<<endl;
	}
}
void priorityQueue::deleteMax(int newNumDel)
{
	cout<<"\n+++ Deleting maximum : ";
	for(int i=0;i<newNumDel;i++)
	{
		cout<<arr[0]<<" ";
		swap(&arr[0], &arr[--size]);
		MaxHeapify(0);
	}
}
/*
int main()
{
	int n,num,newNumIns,newNumDel;
	cin>>n;
	int sarr[n];
	cout<<"\nninit = "<<n<<endl;
	cout<<"\n+++ Initializing queue: ";
	for(int i=0;i<n;i++)
	{
		cin>>num;
		cout<<setw(2)<<num<<" ";
		sarr[i] = num;
	}
	cout<<endl;
	priorityQueue myQueue;
	for(int i=0;i<n;i++)
	{
		myQueue.insertKey(sarr[i]);
	}
	for(int i = n/2 - 1;i>=0;i--)
	{
		myQueue.MaxHeapify(i);
	}
	cout<<"\n+++ Queue initialized "<<endl;
	myQueue.initMin();
	myQueue.printQueue();
	cin>>newNumIns;
	cout<<"\nnins : "<<newNumIns<<endl;
	cout<<"+++ Inserting elements: ";
	for(int i=0;i<newNumIns;i++)
	{
		cin>>num;
		cout<<num<<" ";
		myQueue.insertKey(num);
	}
	n = n + newNumIns;
	for(int i = n/2 - 1;i>=0;i--)
	{
		myQueue.MaxHeapify(i);
	}
	myQueue.initMin();
	cout<<"\n+++ Insertions done : "<<endl;
	myQueue.printQueue();
	cin>>newNumDel;
	cout<<"\nndelmax = "<<newNumDel<<endl;
	myQueue.deleteMax(newNumDel);
	myQueue.initMin();
	cout<<"\n+++ Deletions done :  "<<endl;
	myQueue.printQueue();
	return 0;
}
*/
int main()
{
	srand(time(NULL));
	int n,num,newNumIns,newNumDel;
	//cin>>n;
	n = rand()%91 + 10;
	int sarr[n];
	cout<<"\nninit = "<<n<<endl;
	cout<<"\n+++ Initializing queue: ";
	for(int i=0;i<n;i++)
	{
		//cin>>num;
		num = rand()%100 + 1;
		cout<<setw(2)<<num<<" ";
		sarr[i] = num;
	}
	cout<<endl;
	priorityQueue myQueue;
	for(int i=0;i<n;i++)
	{
		myQueue.insertKey(sarr[i]);
	}
	for(int i = n/2 - 1;i>=0;i--)
	{
		myQueue.MaxHeapify(i);
	}
	cout<<"\n+++ Queue initialized "<<endl;
	myQueue.initMin();
	myQueue.printQueue();
	//cin>>newNumIns;
	newNumIns = rand()%n;
	cout<<"\nnins : "<<newNumIns<<endl;
	cout<<"+++ Inserting elements: ";
	for(int i=0;i<newNumIns;i++)
	{
		//cin>>num;
		num = rand()%100 + 1;
		cout<<num<<" ";
		myQueue.insertKey(num);
	}
	n = n + newNumIns;
	for(int i = n/2 - 1;i>=0;i--)
	{
		myQueue.MaxHeapify(i);
	}
	myQueue.initMin();
	cout<<"\n+++ Insertions done : "<<endl;
	myQueue.printQueue();
	//cin>>newNumDel;
	newNumDel = rand()%n;
	cout<<"\nndelmax = "<<newNumDel<<endl;
	myQueue.deleteMax(newNumDel);
	myQueue.initMin();
	cout<<"\n+++ Deletions done :  "<<endl;
	myQueue.printQueue();
	return 0;
}