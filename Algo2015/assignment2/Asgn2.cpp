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
int getHeight(node* root)
{
	if(root == NULL)
	{
		return -1;
	}
	int lh = getHeight(root->left);
	int rh = getHeight(root->right);
	return max(lh, rh) + 1;
}
node* rotateRight(node* root)
{
	node* temp1 = root->left;
	node* temp2 = temp1->right;

	temp1->right = root;
	root->left = temp2;

	return temp1;

}
node* makeSkew(node* root)
{
	while(root->left != NULL)
	{
		root = rotateRight(root);
	}
	if(root->right != NULL)
	{
		root->right = makeSkew(root->right);
	}
	return root;
}
node* rebalance(node* root)
{
	
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
	cout<<"    Preorder : ";
	preOrder(root);
	cout<<endl;
	cout<<"    Inorder  : ";
	inOrder(root);
	cout<<endl;
	cout<<"    Height   : "<<setw(2)<<getHeight(root)<<endl;
	root = makeSkew(root);
	cout<<"\n+++ The tree is now completely right skewed : "<<endl;
	cout<<"    Preorder : ";
	preOrder(root);
	cout<<endl;
	cout<<"    Inorder  : ";
	inOrder(root);
	cout<<endl;
	cout<<"    Height   : "<<setw(2)<<getHeight(root)<<endl;
	root = makeSkew(root);
	root = rebalance(root);
	cout<<"\n+++ Balance restored in the tree : "<<endl;
	cout<<"    Preorder : ";
	preOrder(root);
	cout<<endl;
	cout<<"    Inorder  : ";
	inOrder(root);
	cout<<endl;
	cout<<"    Height   : "<<setw(2)<<getHeight(root)<<endl;
	root = makeSkew(root);
	return 0;
}