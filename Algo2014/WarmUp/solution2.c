#include <stdio.h>
#include <stdlib.h>

#define N 30
#define S 12

void printarr ( int A[], int n )
{
   int i;

   for (i=0; i<n; ++i) printf("%3d ", A[i]);
   printf("\n");
}

int main ( int argc, char *argv[] )
{
   int n, s, *A, i, t;

   if (argc > 1) n = atoi(argv[1]); else n = N;
   if (argc > 2) s = atoi(argv[2]); else s = S;
   s %= n;

   A = (int *)malloc(n * sizeof(int));
   for (i=0; i<n; ++i) A[i] = i+1;

   printf("+++ The array before shifting\n"); printarr(A,n);

   /* Reverse A[0...n-s-1] */
   for (i=0; i<(n-s)/2; ++i) {
      t = A[i];
      A[i] = A[n-s-i-1];
      A[n-s-i-1] = t;
   }

   /* Reverse A[n-s...n-1] */
   for (i=0; i<s/2; ++i) {
      t = A[n-s+i];
      A[n-s+i] = A[n-i-1];
      A[n-i-1] = t;
   }

   /* Reverse A[0...n-1] */
   for (i=0; i<n/2; ++i) {
      t = A[i];
      A[i] = A[n-i-1];
      A[n-i-1] = t;
   }

   printf("+++ The array after shifting\n"); printarr(A,n);

   free(A);

   exit(0);
}