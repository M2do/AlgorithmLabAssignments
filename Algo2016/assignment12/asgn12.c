#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N_DFT 25

typedef struct _node {
   int nbr;
   struct _node *next;
} node;

typedef struct {
   int n;
   node **H;
} graph;

int existsedge ( graph G, int u, int v )
{
   node *p;

   p = G.H[u];
   while (p) {
      if (p -> nbr == v) return 1;
      p = p -> next;
   }
   return 0;
}

void addnbr ( graph G, int u, int v )
{
   node *p, *q;

   q = (node *)malloc(sizeof(node));
   q -> nbr = v; q -> next = NULL;
   if ((G.H[u] == NULL) || (v < G.H[u] -> nbr)) {
      q -> next = G.H[u];
      G.H[u] = q;
      return;
   }
   p = G.H[u];
   while (p -> next) {
      if (v < p -> next -> nbr) break;
      p = p -> next;
   }
   q -> next = p -> next;
   p -> next = q;
}

graph gengraph ( int n )
{
   graph G;
   int u, v, m, ne, nc, c, *cs, **C, nattempt;

   G.n = n;
   G.H = (node **)malloc(n * sizeof(node *));
   for (u=0; u<n; ++u) G.H[u] = NULL;

   m = n + rand() % n;
   printf("+++ n = %d\n+++ m = %d\n", n, m);
   nc = 5 + rand() % 6;
   cs = (int *)malloc(nc * sizeof(int));
   C = (int **)malloc(nc * sizeof(int *));
   for (u=0; u<nc; ++u) {
      cs[u] = 0;
      C[u] = (int *)malloc(n * sizeof(int));
   }
   for (u=0; u<n; ++u) {
      v = rand() % nc;
      C[v][cs[v]] = u;
      ++cs[v];
   }
   for (c=0; c<nc; ++c) {
      printf("--- Component %d: Size = %2d:", c, cs[c]);
      for (u=0; u<cs[c]; ++u) printf(" %2d", C[c][u]);
      printf("\n");
   }
   printf("+++ Adding edges:");
   ne = 0; nattempt = 0;
   while (ne < m) {
      if (++nattempt >= 100 * m) break;
      c = rand() % nc;
      if (cs[c] <= 1) continue;
      u = rand() % cs[c];
      v = rand() % cs[c];
      if (u == v) continue;
      if (existsedge(G,C[c][u],C[c][v])) continue;
      addnbr(G,C[c][u],C[c][v]);
      addnbr(G,C[c][v],C[c][u]);
      if (ne % 8 == 0) printf("\n   ");
      ++ne;
      printf(" (%2d,%2d)", C[c][u],C[c][v]); fflush(stdout);
   }
   if (m % 8) printf("\n");
   if (ne < m) printf("\n--- Only %d edges added...\n", ne);
   for (u=0; u<nc; ++u) free(C[u]);
   free(C); free(cs);

   return G;
}

void prngraph ( graph G )
{
   int u;
   node *p;

   for (u=0; u<G.n; ++u) {
      printf("    City %2d --", u);
      p = G.H[u];
      while (p) {
         printf(" %2d", p -> nbr);
         p = p -> next;
      }
      printf("\n");
   }
}

void BFS ( graph G, int r, int *visited )
{
   int *Q, F, B, u, v;
   node *p;

   Q = (int *)malloc(G.n * sizeof(int));
   Q[0] = r; F = B = 0; visited[r] = 1;
   while (F <= B) {
      u = Q[F]; ++F;
      p = G.H[u];
      while (p) {
         v = p -> nbr;
         if (visited[v] == 0) {
            Q[++B] = v;
            visited[v] = 1;
         }
         p = p -> next;
      }
   }
   free(Q);
}

void checkconn ( graph G )
{
   int u, *visited;

   visited = (int *)malloc(G.n * sizeof(int));
   for (u=0; u<G.n; ++u) visited[u] = 0;
   BFS(G,0,visited);
   for (u=0; u<G.n; ++u) if (!visited[u]) break;
   if (u == G.n) printf("\n+++ All cities are connected by highways\n");
   else printf("\n+++ Not all cities are connected by highways\n");
   free(visited);
}

void buildhighways ( graph G )
{
   int u, *visited;

   visited = (int *)malloc(G.n * sizeof(int));
   for (u=0; u<G.n; ++u) visited[u] = 0;

   u = 0;
   while (1) {
      while ((u < G.n) && (visited[u])) ++u;
      if (u == G.n) break;
      addnbr(G,0,u); addnbr(G,u,0);
      BFS(G,u,visited);
      if (u) printf("    Between City 0 and City %d\n", u);
   }

   free(visited);
}

int main ( int argc, char *argv[] )
{
   int n;
   graph G;

   srand((unsigned int)time(NULL));
   n = (argc >= 2) ? atoi(argv[1]) : N_DFT;

   G = gengraph(n);
   printf("\n+++ The graph is:\n"); prngraph(G);

   checkconn(G);

   printf("\n+++ Building highways\n");
   buildhighways(G);

   checkconn(G);

   exit(0);
}