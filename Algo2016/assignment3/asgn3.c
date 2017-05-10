#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define NSML_DFT 25
#define NINS_DFT 1000
#define NDEL_DFT 900

/* Change the following probability to 0 to get random input sequence for
   insertion, and to 1 to get an increasing sequence */
#define INCR_SEQ_PROB 0.75

typedef struct _node {
   int key;
   struct _node *L, *R, *P;
} tnode;

typedef tnode *BST;

void destroy ( BST T )
{
   if (T == NULL) return;
   destroy(T -> L);
   destroy(T -> R);
   free(T);
}

int size ( BST T )
{
   if (T == NULL) return 0;
   return size(T -> L) + 1 + size(T -> R);
}

int height ( BST T )
{
   int lh, rh;

   if (T == NULL) return -1;
   lh = height(T -> L);
   rh = height(T -> R);
   return 1 + ((lh >= rh) ? lh : rh);
}

void inorder ( BST T )
{
   if (T == NULL) return;
   inorder(T -> L);
   printf(" %2d", T -> key);
   inorder(T -> R);
}

/* Function to convert a subtree to a sorted array. Assumes that A[] has
   enough pre-allocated space. */
int BST2array ( BST T, int *A )
{
   int ls, rs;

   if (T == NULL) return 0;
   ls = BST2array(T->L,A);
   A[ls] = T -> key;
   rs = BST2array(T->R,A+ls+1);
   return ls + 1 + rs;
}

/* Function to convert a sorted array to a perfectly balanced BST */
BST array2BST ( int *A, int n )
{
   tnode *T;
   int r;

   if (n == 0) return NULL;

   /* Place median at root */
   r = n / 2;
   T = (tnode *)malloc(sizeof(tnode));
   T -> key = A[r]; T -> P = NULL;

   /* Recursively build left and right subtrees */
   if ((T -> L = array2BST(A,r))) T -> L -> P = T;
   if ((T -> R = array2BST(A+r+1,n-(r+1)))) T -> R -> P = T;
   return T;
}

BST rebuild ( BST T, int n )
{
   int *A;

   A = (int *)malloc(n * sizeof(int));
   BST2array(T,A);
   destroy(T);
   T = array2BST(A,n);
   free(A);
   return T;
}

BST insert ( BST T, int x, int *n, int *m, int prnflag )
{
   tnode *p, *q;
   int dp, sz, lsz, rsz;

   /* Try to locate x in T */
   p = T; q = NULL; dp = 0;
   while (p) {
      if (x == p -> key) {
         if (prnflag) printf("       ");
         return T;
      }
      q = p;
      p = (x < p -> key) ? p -> L : p -> R;
      ++dp;
   }

   /* Search failed. Do standard BST insertion at a new leaf node. */
   p = (tnode *)malloc(sizeof(tnode));
   p -> key = x; p -> L = p -> R = NULL; p -> P = q;
   if (T == NULL) T = p;
   else if (x < q -> key) q -> L = p;
   else q -> R = p;

   /* Tree size increases by one */
   ++(*n); if (*n > *m) *m = *n;

   if (dp > 1 + (int)(log(*n)/log(1.5))) {    /* Rebalancing necessary */
      sz = 1;
      while (q) {  /* Upward journey using parent pointers */
         if (p == q -> L) {
            lsz = sz;
            rsz = size(q -> R);
         } else {
            lsz = size(q -> L);
            rsz = sz;
         }
         sz = lsz + 1 + rsz;

         p = q; q = q -> P;

         if ((lsz > (2.0 / 3.0) * sz) || (rsz > (2.0 / 3.0) * sz)) {
            /* Scapegoat node found */

            if (prnflag) printf(" [R %2d]", sz);
            p = rebuild(p, sz);
            p -> P = q;
            if (q == NULL) return p;
            if (p -> key < q -> key) q -> L = p; else q -> R = p;
            return T;
         }
      }
   } else {
      if (prnflag) printf("       ");
   }

   return T;
}

BST delete ( BST T, int x, int *n, int *m, int prnflag )
{
   tnode *p, *q, *r, *s;

   /* Search for x in T */
   p = T; q = NULL;
   while (p) {
      if (p -> key == x) break;
      q = p;
      if (x < p -> key) p = p -> L; else p = p -> R;
   }

   /* Search fails */
   if (p == NULL) {
      if (prnflag) printf("       ");
      return T;
   }

   /* Search succeeds. Do standard BST deletion. */
   if ((p -> L == NULL) || (p -> R == NULL)) {  /* One child empty */
      r = (p -> L == NULL) ? p -> R : p -> L;
      if (r) r -> P = q;
      if (q == NULL) {
         free(T);
         T = r;
      } else {
         free(p);
         if (x < q -> key) q -> L = r; else q -> R = r;
      }
   } else {                                     /* Both children are present */
      q = p; s = p -> L;
      while (s -> R) { q = s; s = s -> R; }
      p -> key = s -> key;                      /* Copy immediate predecessor */
      r = s -> L;
      free(s);
      if (q == p) q -> L = r; else q -> R = r;
      if (r) r -> P = q;
   }

   --(*n);
   if ((*n > 0) && (*n < (2.0 / 3.0) * (*m))) {  /* if rebuilding is needed */
      if (prnflag) printf(" [R %2d]", *n);
      T = rebuild(T,*n);
      *m = *n;
   } else {
      if (prnflag) printf("       ");
   }

   return T;
}

int main ( int argc, char *argv[] )
{
   int nsml, nins, ndel, last;
   int n = 0, m = 0;
   int i, j, *A;
   BST T = NULL;

   srand((unsigned int)time(NULL));
   if (argc >= 4) {
      nsml = atoi(argv[1]);
      nins = atoi(argv[2]);
      ndel = atoi(argv[3]);
   } else {
      nsml = NSML_DFT;
      nins = NINS_DFT;
      ndel = NDEL_DFT;
   }

   printf("\nnsml = %d\n\n", nsml);

   A = (int *)malloc(nsml * sizeof(int));

   A[0] = 1 + rand() % 20; last = 0;
   printf("+++ insert(%2d):", A[0]);
   T = insert(T,A[0],&n,&m,1);
   printf(" Height = %2d: ", height(T)); inorder(T); printf("\n");

   for (i=1; i<nsml; ++i) {
      if ((double)rand()/(double)RAND_MAX > INCR_SEQ_PROB) {
         A[i] = 1 + rand() % 99;
      } else {
         A[i] = A[last] + 1 + rand() % 5;
         last = i;
      }
      printf("+++ insert(%2d):", A[i]);
      T = insert(T,A[i],&n,&m,1);
      printf(" Height = %2d: ", height(T)); inorder(T); printf("\n");
   }

   printf("\n");

   for (i=0; i<nsml; ++i) {
      j = rand() % nsml; while (A[j] == 0) { ++j; if (j == nsml) j = 0; }
      printf("+++ delete(%2d):", A[j]);
      T = delete(T,A[j],&n,&m,1);
      A[j] = 0;
      printf(" Height = %2d: ", height(T)); inorder(T); printf("\n");
   }

   free(A);

   printf("\nnins = %d\n", nins);
   printf("\n+++ Inserting increasing sequence of %d keys\n", nins);
   for (i=1; i<=nins; ++i) {
      T = insert(T,i,&n,&m,0);
      if (i % 100 == 0)
         printf("    Height after %4d iterations: %d\n", n, height(T));
   }

   printf("\nndel = %d\n", ndel);
   printf("\n+++ Deleting increasing sequence of %2d keys\n", ndel);
   for (i=1; i<=ndel; ++i) {
      T = delete(T,i,&n,&m,0);
      if (i % 100 == 0)
         printf("    Height after %4d deletions: %2d\n", i, height(T));
   }

   destroy(T);

   exit(0);
}