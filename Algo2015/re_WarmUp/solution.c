#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_SIZE 100
#define INFINITY 1000000000

void gencosts ( int C[][MAX_SIZE], int n )
{
   int i, j;

   for (i=0; i<n; ++i) {
      C[i][i] = 0;
      for (j=i+1; j<n; ++j) C[i][j] = C[j][i] = 1 + rand() % 99;
   }

   printf("\n+++ The cost matrix:\n");
   for (i=0; i<n; ++i) {
      printf("    ");
      for (j=0;j<n; ++j) printf("%3d ", C[i][j]);
      printf("\n");
   }
}

void handleleaf ( int C[][MAX_SIZE], int n, int A[], int cost, int *bestcost )
{
   cost += C[A[n-2]][A[n-1]];
   cost += C[A[n-1]][A[0]];
   if (cost < *bestcost) {
#ifdef __VERBOSE
      int i;

      printf("    New tour found: ");
      for (i=0; i<n; ++i) printf("%2d ", A[i]);
      printf("Cost: %4d\n", cost);
#endif
      *bestcost = cost;
   }
}

void mincostexh ( int C[][MAX_SIZE], int n, int A[], int k, int cost, int *bestcost, int *nleaves, int *ncalls )
{
   int j, l, t;

   ++(*ncalls);

   if (k == n-1) {
      handleleaf(C,n,A,cost,bestcost);
      ++(*nleaves);
      return;
   }

   for (j=k; j<n; ++j) {
      t = A[k]; A[k] = A[j]; A[j] = t; cost += C[A[k-1]][A[k]];
      for (l=k+1; l<n; ++l) if (A[l] > A[1]) break;
      if (l < n) mincostexh(C,n,A,k+1,cost,bestcost,nleaves,ncalls);
      cost -= C[A[k-1]][A[k]]; t = A[k]; A[k] = A[j]; A[j] = t;
   }
}

void mincostgp ( int C[][MAX_SIZE], int n, int A[], int k, int cost, int m, int *bestcost, int *nleaves, int *ncalls )
{
   int j, l, t;
   int estimate;

   ++(*ncalls);

   if (k == n-1) {
      handleleaf(C,n,A,cost,bestcost);
      ++(*nleaves);
      return;
   }

   for (j=k; j<n; ++j) {
      t = A[k]; A[k] = A[j]; A[j] = t; cost += C[A[k-1]][A[k]];
      for (l=k+1; l<n; ++l) if (A[l] > A[1]) break;
      if (l < n) {
         estimate = cost + m * (n-k);
         if (estimate < *bestcost) mincostgp(C,n,A,k+1,cost,m,bestcost,nleaves,ncalls);
      }
      cost -= C[A[k-1]][A[k]]; t = A[k]; A[k] = A[j]; A[j] = t;
   }
}

void mincostlp1 ( int C[][MAX_SIZE], int n, int A[], int k, int cost, int *bestcost, int *nleaves, int *ncalls )
{
   int j, l, t, m, r, s;
   int estimate;

   ++(*ncalls);

   if (k == n-1) {
      handleleaf(C,n,A,cost,bestcost);
      ++(*nleaves);
      return;
   }

   for (j=k; j<n; ++j) {
      t = A[k]; A[k] = A[j]; A[j] = t; cost += C[A[k-1]][A[k]];
      for (l=k+1; l<n; ++l) if (A[l] > A[1]) break;
      if (l < n) {
         estimate = cost;
         m = INFINITY;
         for (s=k+1; s<n; ++s) if (C[A[k]][A[s]] < m) m = C[A[k]][A[s]];
         estimate += m;
         for (r=k+1; r<n; ++r) {
            m = C[A[r]][A[0]];
            for (s=k+1; s<n; ++s)
               if ((s != r) && (C[A[s]][A[r]] < m)) m = C[A[s]][A[r]];
            estimate += m;
         }
         if (estimate < *bestcost) mincostlp1(C,n,A,k+1,cost,bestcost,nleaves,ncalls);
      }
      cost -= C[A[k-1]][A[k]]; t = A[k]; A[k] = A[j]; A[j] = t;
   }
}

void mincostlp2 ( int C[][MAX_SIZE], int n, int A[], int k, int cost, int *bestcost, int *nleaves, int *ncalls )
{
   int j, l, t, m, m2, r, s;
   int estimate;

   ++(*ncalls);

   if (k == n-1) {
      handleleaf(C,n,A,cost,bestcost);
      ++(*nleaves);
      return;
   }

   for (j=k; j<n; ++j) {
      t = A[k]; A[k] = A[j]; A[j] = t; cost += C[A[k-1]][A[k]];
      for (l=k+1; l<n; ++l) if (A[l] > A[1]) break;
      if (l < n) {
         estimate = 2 * cost;

         /* You only leave A[k] */
         m = INFINITY;
         for (s=k+1; s<n; ++s) if (C[A[k]][A[s]] < m) m = C[A[k]][A[s]];
         estimate += m;

         /* You only enter A[0] */
         m = INFINITY;
         for (s=k+1; s<n; ++s) if (C[A[s]][A[0]] < m) m = C[A[s]][A[0]];
         estimate += m;

         /* For other unvisited nodes, consider two minimum-cost links: one
            for going in, the other for going out */
         for (r=k+1; r<n; ++r) {
            /* The city may be the next or last one to be visited */
            if (C[A[k]][A[r]] <= C[A[r]][A[0]]) {
               m = C[A[k]][A[r]]; m2 = C[A[r]][A[0]];
            } else {
               m = C[A[r]][A[0]]; m2 = C[A[k]][A[r]];
            }
            /* Compare with distances to other unvisited cities */
            for (s=k+1; s<n; ++s) if (s != r) {
               if (C[A[r]][A[s]] <= m) { m2 = m; m = C[A[r]][A[s]]; }
               else if (C[A[r]][A[s]] < m2) { m2 = C[A[r]][A[s]]; }
            }
            estimate += m + m2;
         }

         if (estimate < 2 * (*bestcost))
            mincostlp2(C,n,A,k+1,cost,bestcost,nleaves,ncalls);
      }
      cost -= C[A[k-1]][A[k]]; t = A[k]; A[k] = A[j]; A[j] = t;
   }
}

int main ( int argc, char *argv[] )
{
   int n, m, i, j;
   int C[MAX_SIZE][MAX_SIZE];
   int A[MAX_SIZE];
   int bestcost;
   int nleaves, ncalls;

   if (argc > 1) n = atoi(argv[1]); else scanf("%d", &n);
   if ((n < 3) || (n > MAX_SIZE)) {
      fprintf(stderr, "Invalid number of cities: %d\n", n);
      exit(1);
   }
   srand((unsigned int)time(NULL));

   gencosts(C,n);

   for (i=0;i<n; ++i) A[i] = i;

   printf("\n+++ Exhaustive search:");
   if (n > 12) printf(" Not done\n");
   else {
      printf("\n");
      bestcost = INFINITY; nleaves = ncalls = 0;
      mincostexh(C,n,A,1,0,&bestcost,&nleaves,&ncalls);
      printf("    Minimum cost computed     = %d\n", bestcost);
      printf("    Number of leaves explored = %d\n", nleaves);
      printf("    Number of calls made      = %d\n", ncalls);
   }

   printf("\n+++ Search with global pruning strategy:\n");
   m = INFINITY;
   for (i=0; i<n; ++i) for (j=i+1; j<n; ++j) if (C[i][j] < m) m = C[i][j];
   bestcost = INFINITY; nleaves = ncalls = 0;
   mincostgp(C,n,A,1,0,m,&bestcost,&nleaves,&ncalls);
   printf("    Minimum cost computed     = %d\n", bestcost);
   printf("    Number of leaves explored = %d\n", nleaves);
   printf("    Number of calls made      = %d\n", ncalls);

   printf("\n+++ Search with local pruning strategy 1:\n");
   bestcost = INFINITY; nleaves = ncalls = 0;
   mincostlp1(C,n,A,1,0,&bestcost,&nleaves,&ncalls);
   printf("    Minimum cost computed     = %d\n", bestcost);
   printf("    Number of leaves explored = %d\n", nleaves);
   printf("    Number of calls made      = %d\n", ncalls);

   printf("\n+++ Search with local pruning strategy 2:\n");
   bestcost = INFINITY; nleaves = ncalls = 0;
   mincostlp2(C,n,A,1,0,&bestcost,&nleaves,&ncalls);
   printf("    Minimum cost computed     = %d\n", bestcost);
   printf("    Number of leaves explored = %d\n", nleaves);
   printf("    Number of calls made      = %d\n", ncalls);

   exit(0);
}