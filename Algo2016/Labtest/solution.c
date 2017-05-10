#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define EVEN_PC 0
#define ODD_PC 1
#define N_DFT 20
#define T_DFT 500
#define ELT_BND 99
#define PLUS_INFINITY 1000000000
#define MINUS_INFINITY -1000000000

int *genarray ( int n, int PC )
{
   int i, *A, BND;

   if (PC == ODD_PC) BND = 2 * ELT_BND + 1; else BND = ELT_BND;
   A = (int *)malloc(n * sizeof(int));
   for (i=0; i<n; ++i) {
      A[i] = rand() % BND;
      if (PC == ODD_PC) A[i] -= ELT_BND; else ++A[i];
      printf(" %3d", A[i]);
   }
   printf("\n");
   return A;
}

/*** DP IDEA
 ***
 *** For each k = 0,1,2,...,i, make one of the two choices
 ***    1) c_k = a_k
 ***    2) c_k = b_k (odd pc) / c_k = -b_k (even pc)
 *** We say that a sum j is realizable from the indices 0,1,2,...,i if
 *** j = c_0 + c_1 + c_2 + ... + c_i for some of the choices of c_k.
 ***
 *** Initial condition (i=0): Only a_0 and b_0 (odd pc) / -b_0 (even pc)
 *** are realizable.
 ***
 *** Recursion (i>0): j is realizable from 0,1,2,...,i if and only if
 *** either j - a_i or j - b_i (odd pc) / j + b_i (even pc) is realizable
 *** from 0,1,2,...,i-1.
 ***
 *** We maintain a 2-d array S to store which sums are realizable.
 *** S[i][j] = 0 if the sum j is not realizable from indices 0,1,2,...,i.
 *** S[i][j] = a if a_i is chosen to realize the sum j.
 *** S[i][j] = b if b_i is chosen to realize the sum j.
 ***
 *** S[0][j] = a if and only if j = a_0
 *** S[0][j] = b if and only if j = b_0 (odd pc) / -b_0 (even pc)
 ***
 *** We populate the i-th row S[i] by shifting S[i-1] by a_i and also by
 *** b_i (odd pc) / -b_i (even pc) and taking OR of the two shifted rows
 *** (both a and b are treated as logical 1).
 ***
 *** The final decision is available as S[n-1][T] (T = 0 for even pc).
 ***/

/* Shift S[i-1] (passed as p) by c and store in S[i] (passed as q) */
void addnmark ( char *p, char *q, int minsum, int maxsum, int c, char array )
{
   int i, j;

   for (i=minsum; i<=maxsum; ++i) {
      if (p[i]) {
         j = i + c;
         if (!q[j]) q[j] = array;
      }
   }
}

/* Function for both deciding the existence of a solution and printing
   if it exists */
void DPsolve ( int *A, int *B, int n, int T, int PC )
{
   int i, j, sum1, sum2, maxsum, minsum, shift;
   char **S, *p, *sol;

   /* The sums may be positive, negative, or zero. We first determine
      the minimum and maximum achievable sums. This is needed for
      allocating memory appropriately to the rows of S. */
   sum1 = sum2 = 0; maxsum = MINUS_INFINITY; minsum = PLUS_INFINITY;
   if (PC == ODD_PC) {
      for (i=0; i<n; ++i) {
         if (A[i] >= B[i]) { sum1 += A[i]; sum2 += B[i]; }
         else { sum1 += B[i]; sum2 += A[i]; }
         if (sum1 > maxsum) maxsum = sum1;
         if (sum2 < minsum) minsum = sum2;
      }
   } else {
      maxsum = minsum = 0;
      for (i=0; i<n; ++i) {
         maxsum += A[i];
         minsum -= B[i];
      }
   }
   shift = -minsum;

   /* Allocate memory */
   S = (char **)malloc(n * sizeof(char *));
   for (i=0; i<n; ++i) {
      S[i] = (char *)malloc((maxsum - minsum + 1) * sizeof(char));
      for (j=0; j<=maxsum-minsum; ++j) S[i][j] = '\0';
   }

   /* Populate S[0] from initial conditions */
   p = S[0] + shift;
   p[A[0]] = 'a';
   if (PC == ODD_PC) p[B[0]] = 'b'; else p[-B[0]] = 'b';

   /* Loop for generating S[i] from S[i-1] */
   for (i=1; i<n; ++i) {
      addnmark(S[i-1]+shift,S[i]+shift,minsum,maxsum,A[i],'a');
      addnmark(S[i-1]+shift,S[i]+shift,minsum,maxsum,(PC==ODD_PC)?B[i]:-B[i],'b');
   }

   p = S[n-1] + shift;
   printf("\n");
   if (p[T] == '\0') {
      printf("+++ No solution exists\n");
   } else {
      printf("+++ Solution exists\n");

      /* Array for storing a or b choices */
      sol = (char *)malloc(n * sizeof(char));

      /* Trace back how the sum T is achieved */
      j = T;
      for (i=n-1; i>=0; --i) {
         p = S[i] + shift;
         sol[i] = p[j];
         if (p[j] == 'a') j -= A[i];
         else if (PC == ODD_PC) j -= B[i];
         else j += B[i];
      }

      /* Print contributions from A */
      printf("A : "); sum1 = 0;
      for (i=0; i<n; ++i) {
         if (sol[i] == 'a') {
            printf(" %3d", A[i]); sum1 += A[i];
         } else {
            printf("    ");
         }
      }

      /* Print contributions from B */
      printf("\nB : "); sum2 = 0;
      for (i=0; i<n; ++i) {
         if (sol[i] == 'b') {
            printf(" %3d", B[i]); sum2 += B[i];
         } else {
            printf("    ");
         }
      }
      printf("\nSum1 = %d, Sum2 = %d\n", sum1, sum2);
      free(sol);
   }

   for (i=0; i<n; ++i) free(S[i]);
   free(S);
}

int main ( int argc, char *argv[] )
{
   int n, *A, *B, T;

   srand((unsigned int)time(NULL));
   n = (argc >= 2) ? atoi(argv[1]) : N_DFT;
   T = (argc >= 3) ? atoi(argv[2]) : T_DFT;

   printf("\n+++ FOR ODD PC\nn =   %d\n", n);
   printf("A = "); A = genarray(n,ODD_PC);
   printf("B = "); B = genarray(n,ODD_PC);
   printf("T =  %d\n", T);
   DPsolve(A,B,n,T,ODD_PC);

   printf("\n+++ FOR EVEN PC\nn =   %d\n", n);
   printf("A = "); A = genarray(n,EVEN_PC);
   printf("B = "); B = genarray(n,EVEN_PC);
   DPsolve(A,B,n,0,EVEN_PC);

   exit(0);
}