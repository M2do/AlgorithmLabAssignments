#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int *populate ( int n )
{
   int *A, i;

   if (n <= 0) return NULL;

   A = (int *)malloc(n * sizeof(int));
   for (i=0; i<n; ++i) {
      A[i] = 1 + rand() % 9;
      printf("%-3d", A[i]);
   }
   printf("\n");

   return A;
}

/* The following function attempts to locate i,j such that
   Sum(A[i...j]) <= t and Sum(A[i...j+1]) > t. If Sum(A[i...j]) = t,
   we have found a match. Otherwise, we increment i, and try to
   locate j' >= j such that Sum(A[i+1...j'] <= t and Sum(A[i+1...j'+1] > t.
   The special case j = n-1 is to be handled separately.

   An unsuccessful search terminates with i = j = n-1. These variables
   are never decremented. The total number of increments is n + n = 2n.
   Between any two consecutive increments, only a constant number of
   operations are carried out. Therfore this function has running time
   O(n) despite the fact that there is a nested loop. */

void locate ( int *A, int n, int t )
{
   int i, j, sum, s;

   if (t <= 0) return;

   i = j = 0; sum = A[0];
   while (i < n) {
      while (1) {
         if (sum == t) {
            printf("+++ Subarray found...\n");
            printf("%d = ", t);
            while (i <= j) {
               printf("%d %c ", A[i], (i == j) ? '\n' : '+');
               ++i;
            }
            return;
         }
         if (j == n-1) break;
         s = sum + A[j+1]; if (s > t) break;
         ++j; sum = s;
      }
      sum -= A[i]; ++i;
   }

   printf("+++ No subarray found...\n");
}

int main ( int argc, char *argv[] )
{
   int *A, n, t;

   srand((unsigned int)time(NULL));

   if (argc >= 3) {
      n = atoi(argv[1]);
      t = atoi(argv[2]);
   } else {
      printf("n = "); scanf("%d", &n);
      printf("t = "); scanf("%d", &t);
   }

   A = populate(n);

   locate(A,n,t);

   exit(0);
}