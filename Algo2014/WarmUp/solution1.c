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

int gcd ( int a, int b )
{
   int r;

   if (a < 0) a = -a;
   if (b < 0) b = -b;
   if (a == 0) return b;
   while (b) {
      r = a % b;
      a = b;
      b = r;
   }
   return a;
}

int main ( int argc, char *argv[] )
{
   int n, s, *A, d, i, j, k, t1, t2;

   if (argc > 1) n = atoi(argv[1]); else n = N;
   if (argc > 2) s = atoi(argv[2]); else s = S;
   s %= n;

   A = (int *)malloc(n * sizeof(int));
   for (i=0; i<n; ++i) A[i] = i+1;

   printf("+++ The array before shifting\n"); printarr(A,n);

   d = gcd(n,s);

   for (j=0; j<d; ++j) {
      /* The outer loop runs for d iterations */
      t1 = A[j];
      k = j + s; if (k >= n) k -= n;
      for (i=0; i<n/d; ++i) {
         /* The inner loop handles the indices j, j+d, j+2d, j+3d, ...,
            j + (n/d-1)d */
         t2 = A[k];
         A[k] = t1;
         t1 = t2;
         k = k + s; if (k >= n) k -= n;
      }
   }

   printf("+++ The array after shifting\n"); printarr(A,n);

   free(A);

   exit(0);
}