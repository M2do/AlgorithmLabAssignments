/* Implementation of Tarjan's Bridge-Finding Algorithm (Information
   Processing Letters 2 (1974), 160—161). The paper handles any
   spanning tree of a connected graph. Here, we deal with DFS trees
   which do not contain cross edges. Therefore the condition on
   dfshi can be ignored. */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define EDGE_PROBABILITY 0.25
#define DUMMY_NBR -1

typedef struct _node {
   int nbr;
   struct _node *next;
} node;

typedef node **graph;

int **readAdjMat ( int n )
{
   int **M, i, j;
   double r;

   printf("+++ Reading adjacency matrix\n");
   M = (int **)malloc(n * sizeof(int *));
   for (i=0; i<n; ++i) {
      M[i] = (int *)malloc(n * sizeof(int));
      M[i][i] = 0;
   }
   for (i=0; i<=n-2; ++i) {
      printf("    ");
      for (j=0; j<i; ++j) printf("  ");
      for (j=i+1; j<n; ++j) {
         r = (double)rand() / (double)RAND_MAX;
         if (r <= EDGE_PROBABILITY) {
            printf(" 1");
            M[i][j] = M[j][i] = 1;
         } else {
            printf(" 0");
            M[i][j] = M[j][i] = 0;
         }
      }
      printf("\n");
   }

   return M;
}

void freeAdjMat ( int **M, int n )
{
   int i;

   for (i=0; i<n; ++i) free(M[i]);
   free(M);
}

graph getAdjList ( int **M, int n )
{
   graph G;
   node *p;
   int i, j;

   printf("\n+++ Converting adjacency matrix to adjacency list\n");
   G = (node **)malloc(n * sizeof(node *));
   for (i=0; i<n; ++i) {
      G[i] = (node *)malloc(sizeof(node));
      G[i] -> nbr = DUMMY_NBR;
      G[i] -> next = NULL;
      for (j=n-1; j>=0; --j) if (M[i][j]) {
         p = (node *)malloc(sizeof(node));
         p -> nbr = j;
         p -> next = G[i] -> next;
         G[i] -> next = p;
      }
   }
   return G;
}

void freeAdjList ( graph G, int n )
{
   int i;
   node *p, *q;

   for (i=0; i<n; ++i) {
      p = G[i];
      while (p) {
         q = p -> next;
         free(p);
         p = q;
      }
   }
   free(G);
}

void prnGraph ( graph G, int n )
{
   int i;
   node *p;

   printf("\n+++ Printing graph from adjacency list\n");
   for (i=0; i<n; ++i) {
      printf("    Neighbors of %3d:", i);
      p = G[i] -> next;
      while (p) {
         printf("%3d", p -> nbr);
         p = p -> next;
      }
      printf("\n");
   }
}

void dfs ( graph G, int i, int visited[], int *tcs, int verbose )
{
   node *p;

   if (verbose) printf(" %3d", i);
   visited[i] = 1;
   ++(*tcs);
   p = G[i] -> next;
   while (p) {
      if (!visited[p -> nbr]) dfs(G,p->nbr,visited,tcs,verbose);
      p = p -> next;
   }
}

int lcsize ( graph G, int n, int verbose )
{
   int *visited, i, lcs = 0, tcs, nc;

   if (verbose) printf("\n+++ Finding the largest component size\n");
   visited = (int *)malloc(n * sizeof(int));
   for (i=0; i<n; ++i) visited[i] = 0;
   i = nc = 0;
   while (i < n) {
      if (!visited[i]) {
         tcs = 0; if (verbose) printf("    Component %3d:", ++nc);
         dfs(G,i,visited,&tcs,verbose);
         if (verbose) printf("\n");
         if (tcs > lcs) lcs = tcs;
      }
      ++i;
   }
   free(visited);
   if (verbose) printf("    The largest component has %d nodes\n", lcs);
   return lcs;
}

void cutedges3 ( graph G, int n )
{
   int i, j, lcs;
   node *p, *q, *u, *v;

   for (i=0; i<n; ++i) {
      p = G[i];
      while (p -> next) {
         q = p -> next; j = q -> nbr;
         if (i < j) {
            p -> next = q -> next;
            u = G[j];
            while (u -> next -> nbr != i) u = u -> next;
            v = u -> next;
            u -> next = v -> next;
            lcs = lcsize(G,n,0);
            if (lcs < n) printf("    (%3d , %-3d) is a cut edge\n", i, j);
            p -> next = q; u -> next = v;
         }
         p = q;
      }
   }
}

int DFS ( graph G, int i, int *dfslvl, int *dfsnum, int *dfslo, int num )
{
   node *p;
   int j;

   dfsnum[i] = dfslo[i] = num++;

   p = G[i] -> next;
   while (p) {
      j = p -> nbr;
      if (dfslvl[j] == -1) {
         dfslvl[j] = dfslvl[i] + 1;
         num = DFS(G,j,dfslvl,dfsnum,dfslo,num);
         if (dfslo[j] < dfslo[i]) dfslo[i] = dfslo[j];

         if (dfslo[j] == dfsnum[j])
            printf("    (%3d , %-3d) is a cut edge\n", i, j);
         
      } else if (dfslvl[i] >= dfslvl[j]+2) {
         if (dfsnum[j] < dfslo[i]) dfslo[i] = dfsnum[j];
      }
      p = p -> next;
   }

   return num;
}

void cutedges4 ( graph G, int n )
{
   int *dfslvl, *dfsnum, *dfslo, i, num;

   dfslvl = (int *)malloc(n * sizeof(int));
   dfsnum = (int *)malloc(n * sizeof(int));
   dfslo = (int *)malloc(n * sizeof(int));

   for (i=0; i<n; ++i) dfslvl[i] = dfsnum[i] = -1;
   num = 0; dfslvl[0] = 0;
   num = DFS(G,0,dfslvl,dfsnum,dfslo,num);

   free(dfslvl); free(dfsnum); free(dfslo);
}

int main ( int argc, char *argv[] )
{
   int n, lcs;
   int **M;
   graph G;

   if (argc > 1) n = atoi(argv[1]); else scanf("%d", &n);
   srand((unsigned int)time(NULL));

   M = readAdjMat(n);
   G = getAdjList(M,n);
   freeAdjMat(M,n);
   prnGraph(G,n);
   lcs = lcsize(G,n,1);
   if (lcs < n) printf("\n+++ The graph is disconnected\n");
   else {
      printf("\n+++ Finding cut edges (Inefficient)\n");
      cutedges3(G,n);
      printf("\n+++ Finding cut edges (Efficient)\n");
      cutedges4(G,n);
   }

   exit(0);
}