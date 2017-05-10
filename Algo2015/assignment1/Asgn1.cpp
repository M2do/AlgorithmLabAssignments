/*sample input to be applied after uncommenting the first main function and commenting out second one
20
86 58 82 78 48 85 28 18 14 69 11 3 37 50 17 96 77 11 43 56
30
70
*/
#include <bits/stdc++.h>
using namespace std;
struct node
{
    int data;
    node* left;
    node* right;
};
node* getNode(int idata)
{
    node* temp = new node;
    temp->data = idata;
    temp->left = NULL;
    temp->right = NULL;
    return temp;
}
node* insert(node* root,int idata)
{
    if(root == NULL)
    {
        node* temp = getNode(idata);
        return temp;
    }
    if(idata < root->data)
    {
        root->left = insert(root->left,idata);
    }
    else if(idata > root->data)
    {
        root->right = insert(root->right,idata);
    }
    else
    {
        return root;
    }
    return root;
}
void preOrder(node* root)
{
    if(root == NULL)
    {
        return;
    }
    cout<<root->data<<" ";
    preOrder(root->left);
    preOrder(root->right);
}
void inOrder(node* root)
{
    if(root == NULL)
    {
        return;
    }
    inOrder(root->left);
    cout<<root->data<<" ";
    inOrder(root->right);
}
int geTotal(node* root)
{
    if(root == NULL)
    {
        return 0;
    }
    int lc = geTotal(root->left);
    int rc = geTotal(root->right);
    return lc + rc + 1;
}
void putArray(node* root, int arr[], int* len)
{
    if(root == NULL)
    {
        return;
    }
    putArray(root->left,arr,len);
    arr[(*len)++] = root->data;
    putArray(root->right,arr,len);
}
node* getCeil(node* root, int num)
{
    if(root == NULL)
    {
        return NULL;
    }
    if(root->data == num)
    {
        return root;
    }
    node* lnode = getCeil(root->left, num);
    node* rnode = getCeil(root->right, num);
    if(lnode)
    {
        return lnode;
    }
    return rnode;
}
node* search1(node* root,int x)
{
    int i;
    int size = geTotal(root);
    int arr[size];
    int len = 0;
    putArray(root,arr,&len);
    int num;
    for(i=0;i<size;i++)
    {
        if(arr[i] >= x)
        {
            num = arr[i];
            break;
        }
    }
    if(i == size)
    {
        return NULL;
    }
    return getCeil(root,num);
}
node* search2(node* root,int y)
{
    int i;
    int size = geTotal(root);
    int arr[size];
    int len = 0;
    putArray(root,arr,&len);
    int num;
    for(i=size-1;i>=0;i--)
    {
        if(arr[i] <= y)
        {
            num = arr[i];
            break;
        }
    }
    if(i == 0)
    {
        return NULL;
    }
    return getCeil(root,num);
}
void printValues(node* root,int x,int y)
{
    int i;
    int size = geTotal(root);
    int arr[size];
    int len = 0;
    putArray(root,arr,&len);
    int num;
    for(i=0;i<size;i++)
    {
        if((arr[i] >= x) && (arr[i] <= y))
        {
            cout<<arr[i]<<" ";
        }
    }
}
node *lca(node* root, int n1, int n2)
{
    if (root == NULL) return NULL;
    if (root->data > n1 && root->data > n2)
        return lca(root->left, n1, n2);
    if (root->data < n1 && root->data < n2)
        return lca(root->right, n1, n2);
    return root;
}
/*
int main()
{
    int n,num,x,y;
    node* root = NULL;
    cin>>n;
    int arr[n];
    cout<<"\nn = "<<n<<endl;
    cout<<"+++ insert   : ";
    for(int i=0;i<n;i++)
    {
        cin>>num;
        cout<<num<<" ";
        arr[i] = num;
    }
    for(int i=0;i<n;i++)
    {
        root = insert(root,arr[i]);
    }
    cout<<"\n+++ The BST created has the following listings : "<<endl;
    cout<<"    Preorder : ";
    preOrder(root);
    cout<<endl;
    cout<<"    Inorder  : ";
    inOrder(root);
    cout<<endl;
    cin>>x>>y;
    cout<<"x = "<<x<<endl;
    cout<<"y = "<<y<<endl;
    node* searchResult1 = search1(root,x);
    int lcaNum1 = 0;
    if(searchResult1)
    {
        cout<<"+++ search1 ("<<setw(2)<<x<<")"<<" : "<<searchResult1->data<<endl;
        lcaNum1 = searchResult1->data;
    }
    else
    {
        cout<<"+++ all keys are smaller than "<<x<<endl;
    }
    node* searchResult2 = search2(root,y);
    int lcaNum2 = 0;
    if(searchResult2)
    {
        cout<<"+++ search2 ("<<setw(2)<<y<<")"<<" : "<<searchResult2->data<<endl;
        lcaNum2 = searchResult2->data;
    }
    else
    {
        cout<<"+++ all keys are greater than "<<y<<endl;
    }
    node* lcaNode = lca(root,lcaNum1,lcaNum2);
    cout<<"+++ lca ("<<setw(2)<<lcaNum1<<", "<<setw(2)<<lcaNum2<<") : "<<lcaNode->data<<endl;
    cout<<"+++ values of T between "<<setw(2)<<x<<" and "<<setw(2)<<y<<" : ";
    printValues(root,x,y);
    cout<<endl;
    return 0;
}*/
int main()
{
    srand(time(NULL));
    int n,num;
    node* root = NULL;
    n = rand()%15 + 35;
    int arr[n];
    cout<<"\nn = "<<n<<endl;
    cout<<"+++ insert   : ";
    for(int i=0;i<n;i++)
    {
        num = rand()%80 + 20;
        cout<<num<<" ";
        arr[i] = num;
    }
    for(int i=0;i<n;i++)
    {
        root = insert(root,arr[i]);
    }
    cout<<"\n+++ The BST created has the following listings : "<<endl;
    cout<<"    Preorder : ";
    preOrder(root);
    cout<<endl;
    cout<<"    Inorder  : ";
    inOrder(root);
    cout<<endl;
    int x = rand()%50 + 20;
    int y = rand()%50 + 20 + (rand()%30);
    cout<<"x = "<<x<<endl;
    cout<<"y = "<<y<<endl;
    node* searchResult1 = search1(root,x);
    int lcaNum1 = 0;
    if(searchResult1)
    {
        cout<<"+++ search1 ("<<setw(2)<<x<<")"<<" : "<<searchResult1->data<<endl;
        lcaNum1 = searchResult1->data;
    }
    else
    {
        cout<<"+++ all keys are smaller than "<<x<<endl;
    }
    node* searchResult2 = search2(root,y);
    int lcaNum2 = 0;
    if(searchResult2)
    {
        cout<<"+++ search2 ("<<setw(2)<<y<<")"<<" : "<<searchResult2->data<<endl;
        lcaNum2 = searchResult2->data;
    }
    else
    {
        cout<<"+++ all keys are greater than "<<y<<endl;
    }
    node* lcaNode = lca(root,lcaNum1,lcaNum2);
    cout<<"+++ lca ("<<setw(2)<<lcaNum1<<", "<<setw(2)<<lcaNum2<<") : "<<lcaNode->data<<endl;
    cout<<"+++ values of T between "<<setw(2)<<x<<" and "<<setw(2)<<y<<" : ";
    printValues(root,x,y);
    cout<<endl;
    return 0;
}