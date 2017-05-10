#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_LEN 10
#define DFT_LIM 99
#define DFT_NINS 20
#define DFT_NDEL 20

typedef unsigned int uint;

typedef struct _tnode {
   uint EOS;
   struct _tnode *L, *R;
} tnode;

typedef tnode *bintree;

/* Function to store the binary representation of a in S[] without the
   leading 1 (msb). The function assumes that a > 0. */
void binaryRep ( char *S, uint a )
{
   int i, len;
   char T[MAX_LEN];

   i = 0;
   while (a) {
      T[i] = '0' + (a & 1); a >>= 1; ++i;
   }
   --i; S[i] = '\0';    /* Discard the msb */
   --i; len = i;        /* len is the number of bits to copy */
   while (i >= 0) {
      S[len-i] = T[i];  /* copy from msb to lsb */
      --i;
   }
}

bintree insert ( bintree T, uint a )
{
   tnode *p;
   int i, l;
   char S[MAX_LEN+1];

   /* 0 cannot be stored */
   if (a == 0) return T;

   /* If T is empty, create a new root */
   if (T == NULL) {
      T = (tnode *)malloc(sizeof(tnode));
      T -> L = T -> R = NULL; T -> EOS = 0;
   }

   /* Convert n to a binary string */
   binaryRep(S,a); l = strlen(S);

   /* Track down the tree following the bits of n. If the walk cannot
      proceed, new nodes are created. */
   p = T;
   for (i=0; i<l; ++i) {
      if (S[i] == '0') {
         if (p -> L == NULL) {
            p -> L = (tnode *)malloc(sizeof(tnode));
            p -> L -> L = p -> L -> R = NULL; p -> L -> EOS = 0;
         }
         p = p -> L;
      } else {
         if (p -> R == NULL) {
            p -> R = (tnode *)malloc(sizeof(tnode));
            p -> R -> L = p -> R -> R = NULL; p -> R -> EOS = 0;
         }
         p = p -> R;
      }
   }

   /* Set the EOS marker (works even when a was already present) */
   p -> EOS = 1;

   return T;
}

/* Recursive function to delete all nodes having no strings stored in
   their subtrees. S[] stores the binary representation of the number
   deleted, l is the number of bits in S[], and i is an index in S[]. */
bintree postDeleteAdj ( bintree T, char *S, int l, int i )
{
   /* Recursively handle the appropriate subtree */
   if (i <= l-1) {
      if (S[i] == '0') T -> L = postDeleteAdj(T->L,S,l,i+1);
      else T -> R = postDeleteAdj(T->R,S,l,i+1);
   }

   /* Now check whether this node needs to be deleted */
   if ((T -> EOS == 0) && (T -> L == NULL) && (T -> R == NULL)) {
      free(T); return NULL;
   }

   return T;
}

bintree delete ( bintree T, uint a )
{
   char S[MAX_LEN+1];
   int i, l;
   tnode *p;

   if ((a == 0) || (T == NULL)) return T;

   /* Get the binary representation of n */
   binaryRep(S,a); l = strlen(S);

   /* Search for the presence of n in T */
   p = T;
   for (i=0; i<l; ++i) {
      if (S[i] == '0') {
         if (p -> L == NULL) return T;  /* Search fails */
         p = p -> L;
      } else {
         if (p -> R == NULL) return T;  /* Search fails */
         p = p -> R;
      }
   }

   /* Here the search is successful, that is, a is located in T */

   if (p -> EOS == 0) return T;        /* Item not present */

   p -> EOS = 0;                       /* Delete the EOS marker */

   /* Recursively delete the unmarked leaves */
   if ((p -> L == NULL) && (p -> R == NULL)) T = postDeleteAdj(T,S,l,0);

   return T;
}

/* Preorder printing */
int printTreeHelper ( bintree T, uint a, int npr )
{
   char S[MAX_LEN+1];

   if (T == NULL) return npr;
   if (T -> EOS) {
      binaryRep(S,a);
      printf("%10s", S);
      ++npr;
      if (npr % 8 == 0) printf("\n");
   }
   npr = printTreeHelper(T->L,(a<<1),npr);      /* Left child stores 2a */
   npr = printTreeHelper(T->R,((a<<1)|1),npr);  /* Right child stores 2a+1 */
   return npr;
}

void printTree ( bintree T )
{
   int npr;

   npr = printTreeHelper(T,1,0);
   if (npr % 8) printf("\n");
}

/* Returns the number of nodes in T */
int countNodes ( bintree T )
{
   if (T == NULL) return 0;
   return 1 + countNodes(T->L) + countNodes(T->R);
}

/* This is level-wise left-to-right printing. This needs BFS. We use a
   queue Q[] for the traversal. The numbers corresponding to the nodes
   are stored in A[]. */
void printInts ( bintree T )
{
   tnode **Q;
   int *A, n;
   int F, B;

   if (T == NULL) return;

   /* Create a queue to store all nodes in T */
   n = countNodes(T);
   Q = (tnode **)malloc(n * sizeof(tnode *));
   A = (int *)malloc(n * sizeof(int));

   /* Enqueue the root */
   F = B = 0; Q[0] = T; A[0] = 1;

   /* Repeat so long as the queue of nodes is not empty */
   while (F <= B) {
      /* Process front */
      if (Q[F] -> EOS) { printf(" %2d", A[F]); fflush(stdout); }

      /* Enqueue left child if non-empty */
      if (Q[F] -> L) {
         ++B; Q[B] = Q[F] -> L; A[B] = (A[F] << 1);
      }

      /* Enqueue right child if non-empty */
      if (Q[F] -> R) {
         ++B; Q[B] = Q[F] -> R; A[B] = (A[F] << 1) + 1;
      }

      /* Dequeue */
      ++F;
   }

   free(Q); free(A);
}

void freeTree ( bintree T )
{
   if (T == NULL) return;
   freeTree(T->L);
   freeTree(T->R);
   free(T);
}

int main ( int argc, char *argv[] )
{
   uint i, j, nins, ndel, lim;
   bintree T = NULL;
   uint *A;

   srand((unsigned int)time(NULL));
   if (argc >= 4) {
      lim = atoi(argv[1]);
      nins = atoi(argv[2]);
      ndel = atoi(argv[3]);
   } else {
      lim = DFT_LIM;
      nins = DFT_NINS;
      ndel = DFT_NDEL;
   }

   A = (uint *)malloc(nins * sizeof(uint));
   printf("\n+++ Insert:");
   for (i=0; i<nins; ++i) {
      A[i] = 1 + rand() % lim;
      printf(" %d", A[i]); fflush(stdout);
      T = insert(T,A[i]);
   }

   printf("\n\n+++ After insertion:\n");
   printTree(T);

   printf("\n+++ Delete:");
   for (i=0; i<ndel; ++i) {
      j = rand() % nins;
      printf(" %d", A[j]); fflush(stdout);
      T = delete(T,A[j]);
   }

   printf("\n\n+++ After deletion:\n");
   printTree(T);

   freeTree(T);
   T = NULL;
   printf("\n+++ Old tree destroyed\n\n");

   for (i=0; i<nins; ++i) {
      A[i] = 1 + rand() % lim;
      printf("+++ Insert(%2d):", A[i]); fflush(stdout);
      T = insert(T,A[i]);
      printInts(T);
      printf("\n");
   }

   printf("\n");
   for (i=0; i<ndel; ++i) {
      do { j = rand() % nins; } while (A[j] == 0);
      printf("+++ Delete(%2d):", A[j]); fflush(stdout);
      T = delete(T,A[j]);
      printInts(T);
      printf("\n");
      A[j] = 0;
   }

   freeTree(T);

   free(A);

   exit(0);
}