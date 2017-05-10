#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define C1_DFT 5
#define C2_DFT 6
#define C3_DFT 7

typedef struct _node {
   int nbr;
   struct _node *next;
} node;

typedef struct {
   int u1, u2, u3;
   node *head;
} header;

typedef struct {
   int n;
   header *H;
} graph;

void geninput ( int *a1, int *a2, int *a3, int C1, int C2, int C3)
{
   int T;

   do {
      *a1 = rand() % (1 + C1);
      *a2 = rand() % (1 + C2);
      *a3 = rand() % (1 + C3);
      T = (*a1) + (*a2) + (*a3);
   } while ((T <= C1) || (T <= C2) || (T <= C3));
}

void genoutput ( int *b1, int *b2, int *b3, int a1, int a2, int a3, int C1, int C2, int C3)
{
   int i, T, ntrials = 0;

   do {
      T = a1 + a2 + a3;
      i = rand() % 3;
      if (i == 0) {
         *b1 = (T <= (C1 + C2 + C3) / 2) ? 0 : C1;
         T -= *b1;
         do {
            i = rand() % 2;
            if (i == 0) { *b2 = rand() % (C2 + 1); *b3 = T - (*b2); }
            else { *b3 = rand() % (C3 + 1); *b2 = T - (*b3); }
         } while ((*b2 < 0) || (*b2 > C2) || (*b3 < 0) || (*b3 > C3));
      } else if (i == 1) {
         *b2 = (T <= (C1 + C2 + C3) / 2) ? 0 : C2;
         T -= *b2;
         do {
            i = rand() % 2;
            if (i == 0) { *b1 = rand() % (C1 + 1); *b3 = T - (*b1); }
            else { *b3 = rand() % (C3 + 1); *b1 = T - (*b3); }
         } while ((*b1 < 0) || (*b1 > C1) || (*b3 < 0) || (*b3 > C3));
      } else {
         *b3 = (T <= (C1 + C2 + C3) / 2) ? 0 : C3;
         T -= *b3;
         do {
            i = rand() % 2;
            if (i == 0) { *b1 = rand() % (C1 + 1); *b2 = T - (*b1); }
            else { *b2 = rand() % (C2 + 1); *b1 = T - (*b2); }
         } while ((*b1 < 0) || (*b1 > C1) || (*b2 < 0) || (*b2 > C2));
      }
      ++ntrials;
   } while ((ntrials < 10) && ((*b1 == a1) || (*b2 == a2) || (*b3 == a3)));
}

void addnode ( graph G, int i, int u1, int u2, int u3 )
{
   G.H[i].u1 = u1;
   G.H[i].u2 = u2;
   G.H[i].u3 = u3;
   G.H[i].head = NULL;
}

/* Lexicographic ordering of triples */
int cmpheader ( header H1, header H2 )
{
   if (H1.u1 > H2.u1) return 1;
   if (H1.u1 < H2.u1) return -1;
   if (H1.u2 > H2.u2) return 1;
   if (H1.u2 < H2.u2) return -1;
   if (H1.u3 > H2.u3) return 1;
   if (H1.u3 < H2.u3) return -1;
   return 0;
}

/* Binary search */
int findtriple ( graph G, int u1, int u2, int u3 )
{
   int L, R, M;
   header H;

   H.u1 = u1; H.u2 = u2; H.u3 = u3; H.head = NULL;
   L = 0; R = G.n - 1;
   while (L < R) {
      M = (L + R) / 2;
      if (cmpheader(H,G.H[M]) <= 0) R = M; else L = M+1;
   }
   if (!cmpheader(H,G.H[L])) return L;
   return -1;
}

void addedge ( graph G, int i, int v1, int v2, int v3 )
{
   int j;
   node *p;

   j = findtriple(G,v1,v2,v3);
   p = (node *)malloc(sizeof(node));
   p -> nbr = j;
   p -> next = G.H[i].head;
   G.H[i].head = p;
}

void addalledges ( graph G, int C1, int C2, int C3 )
{
   int u1, u2, u3, t1, t2, t, tt, i;

   /* Determine all possibilities of pouring water from one glass to
      another. The sequence of edge addition ensures that the adjacency
      list of each vertex is sorted with respect to the indices of the
       neighbors. */
   for (i=0; i<G.n; ++i) {
      u1 = G.H[i].u1; u2 = G.H[i].u2; u3 = G.H[i].u3;
      
      /* Glass 3 -> Glass 1 */
      t1 = C1 - u1; t2 = u3; t = (t1 <= t2) ? t1 : t2;

      /* Glass 2 -> Glass 1 */
      t1 = C1 - u1; t2 = u2; tt = (t1 <= t2) ? t1 : t2;

      if (t >= tt) {
         if (t > 0) addedge(G, i, u1+t, u2, u3-t);
         if (tt > 0) addedge(G, i, u1+tt, u2-tt, u3);
      } else {
         if (tt > 0) addedge(G, i, u1+tt, u2-tt, u3);
         if (t > 0) addedge(G, i, u1+t, u2, u3-t);
      }

      /* Glass 3 -> Glass 2 */
      t1 = C2 - u2; t2 = u3; t = (t1 <= t2) ? t1 : t2;
      if (t > 0) addedge(G, i, u1, u2+t, u3-t);

      /* Glass 2 -> Glass 3 */
      t1 = C3 - u3; t2 = u2; t = (t1 <= t2) ? t1 : t2;
      if (t > 0) addedge(G, i, u1, u2-t, u3+t);

      /* Glass 1 -> Glass 2 */
      t1 = C2 - u2; t2 = u1; t = (t1 <= t2) ? t1 : t2;

      /* Glass 1 -> Glass 3 */
      t1 = C3 - u3; t2 = u1; tt = (t1 <= t2) ? t1 : t2;

      if (tt >= t) {
         if (t > 0) addedge(G, i, u1-t, u2+t, u3);
         if (tt > 0) addedge(G, i, u1-tt, u2, u3+tt);
      } else {
         if (tt > 0) addedge(G, i, u1-tt, u2, u3+tt);
         if (t > 0) addedge(G, i, u1-t, u2+t, u3);
      }
   }
}

graph gengraph ( int a1, int a2, int a3, int b1, int b2, int b3, int C1, int C2, int C3 )
{
   graph G;
   int i, j, u1, u2, u3, T;
   header H;

   /* Total quantity of liquid */
   T = a1 + a2 + a3;


   /* First count the number of nodes. We can also start with a (loose) upper
      bound like (T+1) + 4(C1-1) + (T-C1+1) + 1 (assumes T >= C1). This is
      needed for the allocation of memory to the header array. */
   G.n = 0;

   /* First take u1 = 0. Since Glass 1 is empty, the other two glasses
      may contain any amounts adding up to T. Only the capacity constraints
      must be satisfied. */
   for (u2=0; (u2<=T) && (u2<=C2); ++u2) {
      u3 = T - u2;
      if (u3 <= C3) ++G.n;
   }

   /* Now take 0 < u1 < C1. Since Glass 1 is neither empty nor full,
      we need (at least) one of the two other glasses to be either
      full or empty. */
   for (u1=1; (u1<C1) && (u1<=T); ++u1) {
      u2 = 0; u3 = T-u1; if ((u3 <= C3) && (u3 >= 0)) ++G.n;
      u3 = C3; u2 = T-u1-C3; if ((u2 < C2) && (u2 > 0)) ++G.n;
      u3 = 0; u2 = T-u1; if ((u2 < C2) && (u2 > 0)) ++G.n;
      u2 = C2; u3 = T-u1-C2; if ((u3 <= C3) && (u3 >= 0)) ++G.n;
   }

   /* Finally take u1 = C1. Since Glass 1 is full, the other two glasses
      may contain any amounts adding up to T-C1. Only the capacity constraints
      must be satisfied. */
   for (u2=0; (u2<=T-C1) && (u2<=C2); ++u2) {
      u3 = T - C1 - u2;
      if (u3 <= C3) ++G.n;
   }

   /* Create the nodes (headers) */
   G.H = (header *)malloc((G.n + 1) * sizeof(header));

   i = 0;

   /* Actually add the nodes corresponding to the three cases discussed
      during the counting of vertices */

   /* Case 1: u1 = 0 */
   u1 = 0;
   for (u2=0; (u2<=T) && (u2<=C2); ++u2) {
      u3 = T - u2;
      if (u3 <= C3) addnode(G,i++,u1,u2,u3);
   }

   /* Case 2: 0 < u1 < C1 */
   for (u1=1; (u1<C1) && (u1<=T); ++u1) {
      u2 = 0; u3 = T-u1; if ((u3 <= C3) && (u3 >= 0)) addnode(G,i++,u1,u2,u3);
      u3 = C3; u2 = T-u1-C3; if ((u2 < C2) && (u2 > 0)) addnode(G,i++,u1,u2,u3);
      u3 = 0; u2 = T-u1; if ((u2 < C2) && (u2 > 0)) addnode(G,i++,u1,u2,u3);
      u2 = C2; u3 = T-u1-C2; if ((u3 <= C3) && (u3 >= 0)) addnode(G,i++,u1,u2,u3);
   }

   /* Case 3: u1 = C1 */
   u1 = C1;
   for (u2=0; (u2<=T-C1) && (u2<=C2); ++u2) {
      u3 = T - C1 - u2;
      if (u3 <= C3) addnode(G,i++,u1,u2,u3);
   }

   /* If the initial configuration (a1,a2,a3) is not valid, we need to add
      a new vertex for it. We first compute the insertion index. We then
      shift the rest of the array by one cell, and finally insert
      (a1,a2,a3) in the empty cell so produced. */
   if (findtriple(G,a1,a2,a3) < 0) {
      H.u1 = a1; H.u2 = a2; H.u3 = a3; H.head = NULL;
      for (i=0; i<G.n; ++i) {
         if (cmpheader(H,G.H[i]) < 0) break;
      }
      for (j=G.n; j>i; --j) G.H[j] = G.H[j-1];
      G.H[i] = H;
      ++G.n;
   }

   /* Create the edges */
   addalledges(G,C1,C2,C3);

   return G;
}

void prngraph ( graph G )
{
   int i;
   node *p;

   printf("    n = %d\n", G.n);
   for (i=0; i<G.n; ++i) {
      printf("    Node %2d : (%2d,%2d,%2d) ->", i, G.H[i].u1, G.H[i].u2, G.H[i].u3);
      p = G.H[i].head;
      while (p) {
         printf(" %2d", p->nbr);
         p = p -> next;
      }
      printf("\n");
   }
}

/* This is the basic backbone for reachable and prnpath. The array path[]
   stores the path from the source s to the current node u in the DFS
   tree. If this array is NULL, the path is not computed or printed. */
int DFS ( graph G, int u, int t, int *visited, int *path, int l )
{
   node *p;
   int i, j;

   visited[u] = 1;
   if (path) { path[l] = u; ++l; }   /* Only for prnpath */
   if (u == t) {
      if (path) {                    /* Only for prnpath */
         printf("\n+++ A sequence is:\n   ");
         for (i=0; i<l; ++i) {
            j = path[i];
            printf("%s", (i == 0) ? " " : " -> ");
            printf("(%d,%d,%d)", G.H[j].u1, G.H[j].u2, G.H[j].u3);
         }
         printf("\n");
      }
      return 1;
   }
   p = G.H[u].head;
   while (p) {
      if (!visited[p->nbr]) if (DFS(G,p->nbr,t,visited,path,l)) return 1;
      p = p -> next; printf(".");
   }
   return 0;
}

int reachable ( graph G, int s, int t )
{
   int *visited, i;

   visited = (int *)malloc(G.n * sizeof(int));
   for (i=0; i<G.n; ++i) visited[i] = 0;
   if ((i=DFS(G,s,t,visited,NULL,0)))
      printf("\n+++ Sequence of moves exists\n");
   else
      printf("\n+++ Sequence of moves does not exist\n");
   free(visited);
   return i;
}

void prnmoves ( graph G, int s, int t )
{
   int *visited, *path, i;

   visited = (int *)malloc(G.n * sizeof(int));
   path = (int *)malloc(G.n * sizeof(int));
   for (i=0; i<G.n; ++i) visited[i] = 0;
   DFS(G,s,t,visited,path,0);
   free(visited);
   free(path);
}

void freelist ( node *p )
{
   if (p == NULL) return;
   freelist(p -> next);
   free(p);
}

void freegraph ( graph G )
{
   int i;

   for (i=0; i<G.n; ++i) freelist(G.H[i].head);
   free(G.H);
}

int main ( int argc, char *argv[] )
{
   int a1, a2, a3, b1, b2, b3, C1, C2, C3;
   int s, t, solvable;
   graph G;

   srand((unsigned int)time(NULL));

   if (argc >= 4) {
      C1 = atoi(argv[1]); C2 = atoi(argv[2]); C3 = atoi(argv[3]);
   } else {
      C1 = C1_DFT; C2 = C2_DFT; C3 = C3_DFT;
   }

   geninput(&a1,&a2,&a3,C1,C2,C3);
   genoutput(&b1,&b2,&b3,a1,a2,a3,C1,C2,C3);

   printf("+++ Capacities : (%2d,%2d,%2d)\n", C1, C2, C3);
   printf("+++ Start      : (%2d,%2d,%2d)\n", a1, a2, a3);
   printf("+++ End        : (%2d,%2d,%2d)\n", b1, b2, b3);

   G = gengraph(a1,a2,a3,b1,b2,b3,C1,C2,C3);
   printf("\n+++ Graph created\n"); prngraph(G);

   s = findtriple(G,a1,a2,a3); printf("\n+++ Source (s) : %d\n", s);
   t = findtriple(G,b1,b2,b3); printf("+++ Target (t) : %d\n", t);

   solvable = reachable(G,s,t);

   if (solvable) prnmoves(G,s,t);

   freegraph(G);   /* Courtesy */

   exit(0);
}