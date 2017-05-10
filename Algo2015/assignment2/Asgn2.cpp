/*
15
7 14 27 77 13 10 86 70 80 69 61 63 67 11 85     //first input
16
91 74 75 39 6 64 24 89 18 57 17 66 54 63 1 13   //second input
*/
#include <bits/stdc++.h>
using namespace std;
struct node
{
	int  data;
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
	if(idata > root->data)
	{
		root->right = insert(root->right,idata);
	}
	return root;
}
void preOrder(node* root)
{
	if(root ==  NULL)
	{
		return;
	}
	cout<<setw(2)<<root->data<<" ";
	preOrder(root->left);
	preOrder(root->right);
}
void inOrder(node* root)
{
	if(root ==  NULL)
	{
		return;
	}
	inOrder(root->left);
	cout<<setw(2)<<root->data<<" ";
	inOrder(root->right);
}
int main()
{
	int n, num;
	cin>>n;
	int arr[n];
	cout<<"\n+++ n = "<<n<<endl;
	cout<<"\n+++ Insert : ";
	for(int i=0;i<n;i++)
	{
		cin>>num;
		cout<<setw(2)<<num<<" ";
		arr[i] = num;
	}
	node* root = NULL;
	for(int i=0;i<n;i++)
	{
		root = insert(root,arr[i]);
	}
	cout<<"\n+++ Initial BST created : "<<endl;
	cout<<"\n    Preorder : ";
	preOrder(root);
	cout<<endl;
	cout<<"\n    Inorder  : ";
	inOrder(root);
	cout<<endl;
	return 0;
}