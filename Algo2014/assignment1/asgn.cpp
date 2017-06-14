/* 
128
941 999 153 295 461 734 175 738 235 405 966 151 815 391 504 12 537 688 985 390
139 981 257 693 338 78 715 269 643 598 569 584 597 721 598 776 455 772 514 408
177 199 278 991 590 781 722 127 188 426 235 327 408 491 738 464 288 173 732 930
770 20 233 86 740 830 861 913 322 95 41 217 293 318 209 882 99 649 727 287
75 680 332 482 890 70 664 178 242 396 826 12 134 59 815 592 608 677 225 929
771 265 146 782 582 73 665 680 721 111 685 796 791 17 996 681 805 661 858 47
775 403 777 628 461 592 220 787
*/
//#include <bits/stdc++.h>
using namespace std;
struct node
{
    int data;
    node* left;
    node* right;
};
void printArray(int arr[],int size)
{
    int count = 0;
    for(int i=0;i<size;i++)
    {
        cout<<setw(3)<<arr[i]<<" ";
        count++;
        if(count == 20)
        {
            count = 0;
            cout<<endl;
        }
    }
}
node* getNode(int idata)
{
    node* temp = new node;
    temp->data = idata;
    temp->left = NULL;
    temp->right = NULL;
    return temp;
}
void createTree(node* root,int arr[],int l,int h)
{
    if(l > h)
    {
        return;
    }
    if(rand()%3 == 2)
    {
        root->right = getNode(arr[l]);
        createTree(root->right,arr,l+1,h);
    }
    else if(rand()%3 == 2)
    {
        root->left = getNode(arr[l]);
        createTree(root->left,arr,l+1,h);
    }
    else
    {
        int mid = (l+h)/2;
        root->left = getNode(arr[l]);
        createTree(root->left,arr,l+1,mid);
        if(mid < h)
        {
            root->right = getNode(arr[mid+1]);
            createTree(root->right,arr,mid+2,h);
        }
    }
}
void printTree(node* root)
{
    static int count = 0;
    if(count == 20)
    {
        cout<<endl;
        count = 0;
    }
    if(root == NULL)
    {
        return;
    }
    cout<<setw(3)<<root->data<<" ";
    count++;
    printTree(root->left);
    printTree(root->right);
}
int max(int a,int b)
{
    return (a >= b) ? a : b;
}
void filterMax(int sarr[],int index,int ssarr[],int* size)
{   static int value = 0;
    int sum = 0;
    cout<<"\nCurrent Path : ";
    for(int i=0;i<index;i++)
    {
        cout<<setw(3)<<sarr[i]<<" ";
        sum += sarr[i];
    }
    cout<<" Sum : "<<sum<<endl;
    if(sum > value)
    {
        value = sum;
        *size = index;
        for(int i=0;i<index;i++)
        {
            ssarr[i] = sarr[i];
        }
    }
}
void maxWeight(node* root,int sarr[],int index,int ssarr[],int* size)
{
    if(root == NULL)
    {
        filterMax(sarr,index,ssarr,size);
        return ;
    }
    sarr[index] = root->data;
    maxWeight(root->left,sarr,index + 1,ssarr,size);
    maxWeight(root->right,sarr,index + 1,ssarr,size);
}
int getMaxWeight(int ssarr[],int size)
{
    int sum = 0;
    for(int i=0;i<size;i++)
    {
        sum += ssarr[i];
    }
    return sum;
}
void printSupportArray(int ssarr[],int size)
{
    for(int i=0;i<size;i++)
    {
        if(i == size-1)
        {
            cout<<setw(3)<<ssarr[i]<<endl;
            continue;
        }
        cout<<setw(3)<<ssarr[i]<<" + ";
    }
    cout<<endl;
}
int main()
{
    srand(time(NULL));
    int n;
    cin>>n;
    int arr[n];
    for(int i=0;i<n;i++)
    {
        cin>>arr[i];
    }
    cout<<"\n+++ n = "<<n<<endl;
    cout<<"\n+++ Input Array : "<<endl;
    printArray(arr,n);
    cout<<endl;
    node* root = getNode(arr[0]);
    createTree(root,arr,1,n-1);
    cout<<"\n+++ Tree Traversal Listing : "<<endl;
    printTree(root);
    cout<<endl;
    int sarr[1000];
    int ssarr[1000];
    int size = 0;
    int index = 0;
    maxWeight(root,sarr,index,ssarr,&size);
    cout<<"\nMaximum Weight : "<<getMaxWeight(ssarr,size);
    cout<<"\nValues on the max-path ways : "<<endl;
    printSupportArray(ssarr,size);
    return 0;
}
