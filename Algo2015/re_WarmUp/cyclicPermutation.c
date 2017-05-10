#include <stdio.h>
#include <stdlib.h>

#define MAX_SIZE 100
#define DEFAULT_N 10

/* Recursive function to generate all cyclic permutations of 0, 1, ..., n-1.
   Each such permutation begins with 0. A cyclic permutation is printed if
   and only if A[1] < A[n-1]. */
void allcyclicperms ( int A[], int n, int k, int *nperms, int *ncalls )
{
   int i, t;

   ++(*ncalls);

   if (k == n-1) { /* Only one remaining element */
      /* Check whether the cyclic permutation is already printed. This check
         avoids duplicate printing, but does not reduce the cost of the
         generation of the duplicate permutations. */
      if (A[1] < A[n-1]) {
         ++(*nperms);
         printf("Permutation Number %6d: ", *nperms); 
         for (i=0; i<n; ++i) printf("%2d ", A[i]);
         printf("\n");
      }
   }

   /* The following loop puts each of A[k], A[k+1], ..., A[n-1] at the k-th
      index of A[], and makes a recursive call */
   for (i=k; i<n; ++i) {
      t = A[k]; A[k] = A[i]; A[i] = t;        /* Swap */
      allcyclicperms(A,n,k+1,nperms,ncalls);  /* Recursive call */
      t = A[k]; A[k] = A[i]; A[i] = t;        /* Undo the swap */
   }
}

/* This function is similar to allcyclicperms() with the only exception that
   the permutations not to be printed are not generated at all. */
void gencyclicperms ( int A[], int n, int k, int *nperms, int *ncalls )
{
   int i, j, t;

   ++(*ncalls);

   if (k == n-1) {
      ++(*nperms);
      printf("Permutation Number %6d: ", *nperms);
      for (i=0; i<n; ++i) printf("%2d ", A[i]);
      printf("\n");
   }

   for (i=k; i<n; ++i) {
      t = A[k]; A[k] = A[i]; A[i] = t;
      /* Check whether this arrangement leads to any printable permutation */
      for (j=k+1; j<n; ++j) if (A[j] > A[1]) break;
      if (j < n) gencyclicperms(A,n,k+1,nperms,ncalls);
      t = A[k]; A[k] = A[i]; A[i] = t;
   }
}

int main ( int argc, char *argv[] )
{
   int n, A[MAX_SIZE], i, nperms, ncalls;

   if (argc > 1) n = atoi(argv[1]); else scanf("%d", &n);
   if ((n < 3) || (n > DEFAULT_N)) n = DEFAULT_N;

   /* Initially store the permutation 0, 1, 2, ..., n-1 in A[] */
   for (i=0; i<n; ++i) A[i] = i;

   printf("---------------------------------------------------\n");

   nperms = ncalls = 0;
   allcyclicperms(A,n,1,&nperms,&ncalls);
   printf("---------------------------------------------------\n");
   printf("Number of cyclic permutations = %d\n", nperms);
   printf("Number of function calls      = %d\n", ncalls);
   printf("---------------------------------------------------\n");

   nperms = ncalls = 0;
   gencyclicperms(A,n,1,&nperms,&ncalls);
   printf("---------------------------------------------------\n");
   printf("Number of cyclic permutations = %d\n", nperms);
   printf("Number of function calls      = %d\n", ncalls);
   printf("---------------------------------------------------\n");

   exit(0);
}