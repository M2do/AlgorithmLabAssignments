#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define MIN_SIZE 10000
#define MAX_SIZE 10000000
#define MAX_ELEMENT 1000000000
#define OUT_OF_ORDER_FRACTION 0.01
#define FIRST 0
#define RANDOM 1
#define MEDIAN_OF_THREE_1 2
#define MEDIAN_OF_THREE_2 3

void genarray ( int *A, int n )
{
   int i;

   for (i=0; i<n; ++i) A[i] = rand() % MAX_ELEMENT;
}

void perturbarray ( int *A, int n )
{
   int i, k, u, v, t;

   k = round((double)n * OUT_OF_ORDER_FRACTION);
   for (i=0; i<k; ++i) {
      u = rand() % n; v = rand() % n;
      t = A[u]; A[u] = A[v]; A[v] = t;
   }
}

void issorted ( int *A, int n )
{
   int i, flag = 0;

   for (i=1; i<n; ++i) {
      if (A[i] < A[i-1]) { flag = 1; break; }
   }
   if (flag) printf("*** Bug: Unsorted array found...\n");
}

void quicksort ( int *A, int n, int pchoice )
{
   int i, j, k, u, v, w;

   if (n <= 1) return;
   if (n == 2) {
      if (A[0] > A[1]) { u = A[0]; A[0] = A[1]; A[1] = u; }
      return;
   }

   if (pchoice == RANDOM) {
      i = rand() % n; u = A[0]; A[0] = A[i]; A[i] = u;
   } else if ((pchoice == MEDIAN_OF_THREE_1) || (pchoice == MEDIAN_OF_THREE_2)) {
      if (pchoice == MEDIAN_OF_THREE_1) {
         u = 0; v = n/2; w = n-1;
      } else {
         u = n/4; v = n/2; w = 3*n/4;
      }
      if ((A[u] <= A[v]) && (A[u] <= A[w])) {         /* A[u] is smallest */
         i = (A[v] <= A[w]) ? v : w;
      } else if ((A[v] <= A[u]) && (A[v] <= A[w])) {  /* A[v] is smallest */
         i = (A[u] <= A[w]) ? u : w;
      } else {                                        /* A[w] is smallest */
         i = (A[u] <= A[v]) ? u : v;
      }
      u = A[i]; A[i] = A[0]; A[0] = u;
   }

   v = A[0]; i = 0; j = 1; k = n-1;
   while (j <= k) {
      if (A[j] <= v) {
         u = A[i]; A[i] = A[j]; A[j] = u; ++i; ++j;
      } else {
         u = A[j]; A[j] = A[k]; A[k] = u; --k;
      }
   }

   quicksort(A, i, pchoice);
   quicksort(A+j, n-j, pchoice);
}

void qsortperfpc ( int n, int pchoice )
{
   int *A;
   clock_t c1, c2;
   int tr, ts, ta;

   A = (int *)malloc(n * sizeof(int));
   genarray(A,n);
   c1 = clock(); quicksort(A,n,pchoice); c2 = clock(); tr = c2 - c1; issorted(A,n);
   c1 = clock(); quicksort(A,n,pchoice); c2 = clock(); ts = c2 - c1; issorted(A,n);
   perturbarray(A,n);
   c1 = clock(); quicksort(A,n,pchoice); c2 = clock(); ta = c2 - c1; issorted(A,n);
   printf("    %8d ", n);
   if (pchoice == FIRST) printf("%-18s ", "FIRST");
   else if (pchoice == RANDOM) printf("%-18s ", "RANDOM");
   else if (pchoice == MEDIAN_OF_THREE_1) printf("%-18s ", "MEDIAN OF THREE 1 ");
   else if (pchoice == MEDIAN_OF_THREE_2) printf("%-18s ", "MEDIAN OF THREE 2 ");
   else printf("%-18s ", "UNKNOWN");
   printf("%12.3lf %12.3lf %12.3lf\n",
          (double)tr / (double)CLOCKS_PER_SEC,
          (double)ts / (double)CLOCKS_PER_SEC,
          (double)ta / (double)CLOCKS_PER_SEC);
   free(A);
}

void qsortperf ( int n )
{
   qsortperfpc(n, FIRST);
   qsortperfpc(n, RANDOM);
   qsortperfpc(n, MEDIAN_OF_THREE_1);
   qsortperfpc(n, MEDIAN_OF_THREE_2);
   printf("\n");
}

void mergesort ( int *A, int start, int end )
{
   int s1, e1, s2, e2, i1, i2, j;
   int *B;

   if (start >= end) return;
   if (end == start+1) {
      if (A[start] > A[end]) { j = A[start]; A[start] = A[end]; A[end] = j; }
      return;
   }
   s1 = start; e1 = (start + end) / 2;
   s2 = e1 + 1; e2 = end;
   mergesort(A, s1, e1);
   mergesort(A, s2, e2);
   B = (int *)malloc((end-start+1)*sizeof(int));
   i1 = s1; i2 = s2; j = 0;
   while ((i1 <= e1) || (i2 <= e2)) {
      if (i2 > e2) { B[j] = A[i1]; ++i1; }
      else if (i1 > e1) { B[j] = A[i2]; ++i2; }
      else if (A[i1] <= A[i2]) { B[j] = A[i1]; ++i1; }
      else { B[j] = A[i2]; ++i2; }
      ++j;
   }
   for (j=0; j<=end-start; ++j) A[start+j] = B[j];
   free(B);
}

void msortperf ( int n )
{
   int *A;
   clock_t c1, c2;
   int tr, ts, ta;

   A = (int *)malloc(n * sizeof(int));
   genarray(A,n);
   c1 = clock(); mergesort(A,0,n-1); c2 = clock(); tr = c2 - c1; issorted(A,n);
   c1 = clock(); mergesort(A,0,n-1); c2 = clock(); ts = c2 - c1; issorted(A,n);
   perturbarray(A,n);
   c1 = clock(); mergesort(A,0,n-1); c2 = clock(); ta = c2 - c1; issorted(A,n);
   printf("    %8d                    ", n);
   printf("%12.3lf %12.3lf %12.3lf\n",
          (double)tr / (double)CLOCKS_PER_SEC,
          (double)ts / (double)CLOCKS_PER_SEC,
          (double)ta / (double)CLOCKS_PER_SEC);
   free(A);
}

int main ()
{
   int n;

   srand((unsigned int)time(NULL));

   printf("\n+++ Performance of Quick Sort\n\n");
   printf("        n    Pivot type               Random       Sorted    Almost sorted\n\n");
   for (n=MIN_SIZE; n<=MAX_SIZE; n *= 10) qsortperf(n);

   printf("\n+++ Performance of Merge Sort\n\n");
   printf("        n                             Random       Sorted    Almost sorted\n\n");
   for (n=MIN_SIZE; n<=MAX_SIZE; n *= 10) msortperf(n);

   exit(0);
}