/*** Implementation of the Day–Stout–Warren (DSW) algorithm (CACM 1986) ***/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct _node {
   int key;
   struct _node *L, *R;
} node;

typedef node *BST;

/* Standard BST insertion procedure */
BST insert ( BST T, int x, int *mod )
{
   node *p, *q;

   printf("%d ", x);

   p = T; q = NULL; *mod = 0;
   while (p) {
      if (x == p -> key) return T;
      q = p;
      if (x < p -> key) p = p -> L; else p = p -> R;
   }

   *mod = 1;

   p = (node *)malloc(sizeof(node));
   p -> key = x;
   p -> L = p -> R = NULL;

   if (q == NULL) return p;

   if (x < q -> key) q -> L = p; else q -> R = p;

   return T;
}

void preorder ( BST T )
{
   if (T) {
      printf("%d ", T -> key);
      preorder(T -> L);
      preorder(T -> R);
   }
}

void inorder ( BST T )
{
   if (T) {
      inorder(T -> L);
      printf("%d ", T -> key);
      inorder(T -> R);
   }
}

/* Standard height-computation function. */
int height ( BST T )
{
   int lht, rht;

   if (T == NULL) return -1;
   lht = height(T->L);
   rht = height(T->R);
   return 1 + ((lht >= rht) ? lht : rht);
}

void printBST ( BST T )
{
   printf("    Preorder    : "); preorder(T); printf("\n");
   printf("    Inorder     : "); inorder(T); printf("\n");
   printf("    Height      : %d\n\n", height(T));
}

BST rightRotate ( BST T )
{
   node *L, *LR;

   if ((T == NULL) ||(T -> L == NULL)) return T;
   L = T -> L; LR = L -> R;
   L -> R = T; T -> L = LR;
   return L;
}

BST leftRotate ( BST T )
{
   node *R, *RL;

   if ((T == NULL) ||(T -> R == NULL)) return T;
   R = T -> R; RL = R -> L;
   R -> L = T; T -> R = RL;
   return R;
}

BST makeSkew ( BST T )
{
   node *p, *r;

   /* Create a dummy root to make the rest of the implementation clean */
   r = (node *)malloc(sizeof(node));
   r -> key = -1; /* This value is not necessary */
   r -> L = NULL; /* The dummy root has no left subtree */
   r -> R = T;    /* The original tree T is the right subtree of the dummy root */

   p = r;
   while (p -> R) {
      /* Let q = p -> L. If q has left child, right rotate at q, else
         advance down the linked list. */
      if (p -> R -> L == NULL) p = p -> R;
      else p -> R = rightRotate(p -> R);
   }

   return r;
}

/* This function keeps on left rotating alternate nodes on the rightmost
   path from the root. The total number of rotations is supplied as count. */
BST compress ( BST T, int count )
{
   node *p;

   p = T;
   while (count) {
      p -> R = leftRotate(p -> R);
      p = p -> R;
      --count;
   }

   return T;
}

BST rebalance ( BST T )
{
   int n, t, l, lc;

   /* n is the number of nodes in the original tree */
   n = height(T);

   /* Compute l = log2(n+1) */
   t = n + 1; l = 0;
   while (t > 1) { ++l; t /= 2; }

   /* Create the deepest leaves */
   lc = n + 1 - (1 << l); if (lc == 0) lc = (1 << (l - 1));
   T = compress(T,lc);

   n -= lc;
   while (n > 1) { /* Sequence of left rotations */
      n /= 2;
      T = compress(T,n);
   }

   return T;
}

int main ( int argc, char *argv[] )
{
   BST T = NULL;
   int n, i, mod;

   srand((unsigned int)time(NULL));
   if (argc >= 2) n = atoi(argv[1]); else { printf("n = "); scanf("%d", &n); }

   printf("+++ Insert      : ");
   for (i=0; i<n; ++i)
      do (T = insert(T,rand()%100,&mod)); while (!mod);
   printf("\n");
   printf("\n+++ Initial BST created:\n");
   printBST(T);

   T = makeSkew(T);
   printf("+++ The tree is now fully right-skew:\n");
   printBST(T->R);

   T = rebalance(T);
   printf("+++ Balance restored in the tree:\n");
   printBST(T->R);

   exit(0);
}