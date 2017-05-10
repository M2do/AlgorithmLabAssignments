#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N_DFLT 8
#define B_DFLT 2000
#define MIN_VAL 10
#define MAX_VAL 15
#define INFINITY 1000000000

void genarrays ( int *w, int *c, int n, int min_val, int max_val )
{
   int i, r, W, C;

   W = min_val + rand() % (max_val - min_val + 1);
   C = min_val + rand() % (max_val - min_val + 1);
   for (i=0; i<n; ++i) {
      r = 1 + rand() % 5;
      w[i] = 30 + r * W + rand() % 5;
      c[i] = 30 + r * C + rand() % 5;
   }
   printf("+++ Weights : ");
   for (i=0; i<n; ++i) printf("%3d ", w[i]);
   printf("\n");
   printf("+++ Costs   : ");
   for (i=0; i<n; ++i) printf("%3d ", c[i]);
   printf("\n");
}

void printcounts (int *A, int **CNT, int a, int i)
{
   if (i < 0) return;
   printcounts(A,CNT,a-CNT[i][a]*A[i],i-1);
   printf("%3d ", CNT[i][a]);
}

/* Implementation 1: T[i,j] stores the weight of the lightest multi-collection
   of objects taken from objects 0,1,2,...,i, achieving an exact cost of j */
void maxpack1 ( int *w, int *c, int n, int B )
{
   int C, cost, i, j, k, weight;
   int **T, **CNT;
   double max, cperw;

   printf("\n+++ Method 1\n");
   /* In this method, we need a bound on the cost. Locate the object with
      the maximum cost/weight ratio. Fill the knapsack with objects of this
      type. */
   max = (double)c[0] / (double)w[0]; j = 0;
   for (i=1; i<n; ++i) {
      cperw = (double)c[i] / (double)w[i];
      if (cperw > max) {
         max = cperw; j = i;
      }
   }
   C = 0.5 + (double)c[j] * (double)B / (double)w[j];
   T = (int **)malloc(n * sizeof(int *));
   CNT = (int **)malloc(n * sizeof(int *));
   for (i=0; i<n; ++i) {
      T[i] = (int *)malloc((C+1) * sizeof(int));
      CNT[i] = (int *)malloc((C+1) * sizeof(int));
      for (j=0; j<=C; ++j) {
         T[i][j] = INFINITY;
         CNT[i][j] = 0;
      }
   }
   j = 0; cost = 0;
   while ((cost <= C) && (j * w[0] <= B)) {
      T[0][cost] = j * w[0];
      CNT[0][cost] = j;
      ++j; cost += c[0];
   }
   for (i=1; i<n; ++i) {
      for (j=0; j<=C; ++j) {
         cost = j; weight = T[i-1][j]; k = 0;
         while ((weight <= B) && (cost <= C)) {
            if (weight < T[i][cost]) {
               T[i][cost] = weight;
               CNT[i][cost] = k;
            }
            weight += w[i]; cost += c[i]; ++k;
         }
      }
   }
   cost = C;
   while (T[n-1][cost] == INFINITY) --cost;
   printf("    Maximum cost = %d\n    Weight = %d\n", cost, T[n-1][cost]);
   printf("    Counts  : "); printcounts(c,CNT,cost,n-1);
   printf("\n");
}

/* Implementation 2: T[i,j] stores the cost of the costliest multi-collection
   of objects taken from objects 0,1,2,...,i, having an exact weight of j */
void maxpack2 ( int *w, int *c, int n, int B )
{
   int **T, **CNT;
   int i, j, k, weight, cost;

   printf("\n+++ Method 2\n");
   T = (int **)malloc(n * sizeof(int *));
   CNT = (int **)malloc(n * sizeof(int *));
   for (i=0; i<n; ++i) {
      T[i] = (int *)malloc((B+1) * sizeof(int));
      CNT[i] = (int *)malloc((B+1) * sizeof(int));
      for (j=0; j<=B; ++j) {
         T[i][j] = 0;
         CNT[i][j] = 0;
      }
   }
   weight = 0; cost = 0; k = 0;
   while (weight <= B) {
      T[0][weight] = cost;
      CNT[0][weight] = k;
      weight += w[0];
      cost += c[0];
      ++k;
   }
   for (i=1; i<n; ++i) {
      for (j=0; j<=B; ++j) {
         weight = j; cost = T[i-1][j]; k = 0;
         while (weight <= B) {
            if (cost > T[i][weight]) {
               T[i][weight] = cost;
               CNT[i][weight] = k;
            }
            weight += w[i]; cost += c[i]; ++k;
         }
      }
   }
   cost = 0; weight = 0;
   for (j=1; j<=B; ++j) {
      if (T[n-1][j] > cost) {
         cost = T[n-1][j];
         weight = j;
      }
   }
   printf("    Maximum cost = %d\n    Weight = %d\n", cost, weight);
   printf("    Counts  : "); printcounts(w,CNT,weight,n-1);
   printf("\n");
}

int main ( int argc, char *argv[] )
{
   int n, B, *w, *c;

   srand((unsigned int)time(NULL));
   n = (argc >= 2) ? atoi(argv[1]) : N_DFLT;
   B = (argc >= 3) ? atoi(argv[2]) : B_DFLT;

   printf("+++ n = %d\n+++ B = %d\n", n, B);

   w = (int *)malloc(n * sizeof(int));
   c = (int *)malloc(n * sizeof(int));

   genarrays(w,c,n,MIN_VAL,MAX_VAL);
   maxpack1(w,c,n,B);
   maxpack2(w,c,n,B);

   exit(0);
}