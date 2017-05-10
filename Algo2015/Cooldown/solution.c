/* Program for Labeled-Tree-to-Prüfer-Code and Prüfer-Code-to-Labeled-Tree
   constructions. Both implementations run in O(n^2) time. Using min-heaps,
   these running times can be improved. The improvement possibilities are
   mentioned at appropriate places in the code. */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define DUMMY_NBR -1

typedef struct _node {
   int nbr;
   struct _node *next;
} node;

typedef node **graph;

/* Generate a random array A of size n - 2 */
int *gencode ( int n )
{
   int *A, i;

   A = (int *)malloc((n-2) * sizeof(int));
   for (i=0; i<n-2; ++i) A[i]= rand() % n;
   return A;
}

void printcode ( int *A, int n )
{
   int i;

   for (i=0; i<n-2; ++i) {
      if (i % 15 == 0) printf("\n    ");
      printf("%-4d ", A[i]);
   }
   printf("\n");
}

graph codetotree ( int *A, int n )
{
   graph G;
   node *p;
   int i, u, v, *incode;

   G = (node **)malloc(n * sizeof(node *));
   for (i=0; i<n; ++i) {
      /* Create the dummy list headers */
      G[i] = (node *)malloc(sizeof(node));
      G[i] -> nbr = DUMMY_NBR;
      G[i] -> next = NULL;
   }

   /* Store in incode[i] how many times i appears in the code A */
   incode = (int *)malloc(n * sizeof(int));
   for (i=0; i<n; ++i) incode[i] = 0;
   for (i=0; i<n-2; ++i) ++(incode[A[i]]);

   for (i=0; i<n-2; ++i) {

      u = A[i];                      /* Next element from A */
      v = 0; while (incode[v]) ++v;  /* Smallest vertex not in A */

      /* Note: This implementation of the determination of v takes O(n) time.
         If the vertices not present in A are organized in the form of a
         min-heap, then the minimum can be found in O(1) time. When the
         list A is updated at the end of the iteration, insert and
         delete operations are performed which require O(log n) time. */

      /* Add the edge (u,v) to the tree */
      p = (node *)malloc(sizeof(node));
      p -> nbr = v; p -> next = G[u] -> next; G[u] -> next = p;
      p = (node *)malloc(sizeof(node));
      p -> nbr = u; p -> next = G[v] -> next; G[v] -> next = p;

      /* Delete (the first occurrence of) u from A, and append v to A.
         The elements in the array A are not relocated. These changes
         are effected by updating the u-th and v-th entries of incode[]. */
      --(incode[u]); /* if incode[u] becomes 0, insert u to the min-heap */
      ++(incode[v]); /* locate and delete v from the min-heap */
   }

   /* Locate the two vertices u and v not in A */
   u = 0; while (incode[u]) ++u;
   v = u + 1; while (incode[v]) ++v;

   /* Add the last edge (u,v) to the tree */
   p = (node *)malloc(sizeof(node));
   p -> nbr = v; p -> next = G[u] -> next; G[u] -> next = p;
   p = (node *)malloc(sizeof(node));
   p -> nbr = u; p -> next = G[v] -> next; G[v] -> next = p;

   free(incode);

   return G;
}

void printgraph ( graph G, int n )
{
   int i;
   node *p;

   for (i=0; i<n; ++i) {
      printf("    %-4d -> ", i);
      p = G[i] -> next;
      while (p) {
         printf("%4d ", p -> nbr);
         p = p -> next;
      }
      printf("\n");
   }
}

int isleaf ( graph G, int u )
{
   if (G[u] -> next == NULL) return 0;          /* Isolated/Deleted vertex */
   if (G[u] -> next -> next == NULL) return 1;  /* Leaf */
   return 0;                                    /* Node of degree >= 2 */
}

int *treetocode ( graph G, int n )
{
   int *A, i, u, v;
   node *p, *q;

   A = (int *)malloc((n-2) * sizeof(int));

   for (i=0; i<n-2; ++i) {
      u = 0; while (!isleaf(G,u)) ++u;  /* The smallest leaf */
      v = G[u] -> next -> nbr;          /* Its neighbor */
      A[i] = v;                         /* Next element in the output code */

      /* Note: Detection of the vertex u can be done in O(1) time using a
         min-heap of leaves. This should be followed by a deletemin in O(log n)
         time for the removal of the vertex u from G. Moreover, the deletion of
         the edge (u,v) reduces the degree of v by one. If this exxposes v
         as a (new) leaf, v should be inserted in the min-heap. The current
         implementation runs in O(n) time. */

      /* Delete the edge (u,v) from T */
      free(G[u] -> next); G[u] -> next = NULL;
      p = G[v];
      while (p -> next -> nbr != u) p = p -> next; /* O(n) iterations possible */
      q = p -> next;
      p -> next = p -> next -> next;
      free(q);
   }

   return A;
}

int main ( int argc, char *argv[] )
{
   graph T;
   int n, i, *A;

   srand((unsigned int)time(NULL));
   if (argc > 1) n = atoi(argv[1]); else scanf("%d", &n);

   printf("\n+++ Prufer code to tree construction\n    Input code:");
   A = gencode(n); printcode(A,n);
   T = codetotree(A,n);
   printf("    Output tree:\n");
   printgraph(T,n);

   free(A); A = NULL;

   printf("\n+++ Tree to Prufer code construction\n");
   A = treetocode(T,n);
   printf("    Reconstructed code:");
   printcode(A,n);

   for (i=0; i<n; ++i) free(T[i]);
   free(T); free(A);

   exit(0);
}