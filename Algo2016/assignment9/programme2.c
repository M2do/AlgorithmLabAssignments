#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define MAX_LEN 100
#define N_DFLT 16

void gentable ( char T[][4] )
{
   int i, j;

   printf("+++ T\n");
   for (i=0; i<4; ++i) {
      printf("    ");
      for (j=0; j<4; ++j) {
         T[i][j] = (i == j) ? i : rand() % 4;
         printf("%c  ", 'a' + T[i][j]);
      }
      printf("\n");
   }
}

void genalpha ( char *alpha, int n )
{
   int i;

   printf("alpha = ");
   for (i=0; i<n; ++i) {
      alpha[i] = rand() % 4;
      printf("%c", 'a' + alpha[i]);
   }
   alpha[n] = '\0';
   printf("\n");
}

void isrealizable ( char x, char *alpha, int n, char T[][4] )
{
   int ***R;
   int i, j, k, l, s, t;

   R = (int ***)malloc(n * sizeof(int **));
   for (i=0; i<n; ++i) {
      R[i] = (int **)malloc(n * sizeof(int *));
      for (j=0; j<n; ++j) R[i][j] = (int *)malloc(4 * sizeof(int));
   }
   for (i=0; i<=n-1; ++i)
      for (j=0; j<=n-1; ++j)
         R[i][j][0] = R[i][j][1] = R[i][j][2] = R[i][j][3] = 0;
   for (i=0; i<=n-1; ++i) R[i][i][(int)alpha[i]] = 1;
   for (i=0; i<=n-2; ++i) R[i][i+1][(int)T[(int)alpha[i]][(int)alpha[i+1]]] = 1;
   for (l=3; l<=n; ++l) {
      i = 0; j = l-1;
      while (j <= n-1) {
         for (k=i; k<j; ++k) {
            for (s=0; s<4; ++s) {
               if (R[i][k][s]) {
                  for (t=0; t<4; ++t) {
                     if (R[k+1][j][t]) R[i][j][(int)T[s][t]] = 1;
                  }
               }
            }
         }
         ++i; ++j;
      }
   }
   printf("    %c is%srealizable\n", x, R[0][n-1][(int)(x-'a')] ? " " : " not ");
   for (i=0; i<n; ++i) {
      for (j=0; j<n; ++j) free(R[i][j]);
      free(R[i]);
   }
   free(R);
}

unsigned long long catalan ( int n )
{
   int i;
   double val;

   val = 1.0;
   for (i=0; i<n; ++i) val *= (double)(2*n-i) / (double)(n-i);
   val /= (double)(n+1);
   return (unsigned long long)round(val);
}

void countpossibilities ( char *alpha, int n, char T[][4] )
{
   unsigned long long ***R;
   int i, j, k, l, s, t;
   unsigned long long sum;

   R = (unsigned long long ***)malloc(n * sizeof(unsigned long long **));
   for (i=0; i<n; ++i) {
      R[i] = (unsigned long long **)malloc(n * sizeof(unsigned long long *));
      for (j=0; j<n; ++j) R[i][j] = (unsigned long long *)malloc(4 * sizeof(unsigned long long));
   }
   for (i=0; i<=n-1; ++i)
      for (j=0; j<=n-1; ++j)
         R[i][j][0] = R[i][j][1] = R[i][j][2] = R[i][j][3] = 0;
   for (i=0; i<=n-1; ++i) R[i][i][(int)alpha[i]] = 1;
   for (i=0; i<=n-2; ++i) R[i][i+1][(int)T[(int)alpha[i]][(int)alpha[i+1]]] = 1;
   for (l=3; l<=n; ++l) {
      i = 0; j = l-1;
      while (j <= n-1) {
         for (k=i; k<j; ++k) {
            for (s=0; s<4; ++s) {
               for (t=0; t<4; ++t) {
                   R[i][j][(int)T[s][t]] += R[i][k][s] * R[k+1][j][t];
               }
            }
         }
         ++i; ++j;
      }
   }
   sum = 0;
   for (s=0; s<4; ++s) {
      printf("    %c is realizable in %llu ways\n", 'a'+s, R[0][n-1][s]);
      sum += R[0][n-1][s];
   }
   printf("    Total number of possibilities = %llu\n", sum);
   printf("    Catalan(%2d)                   = %llu\n", n-1, catalan(n-1));
   for (i=0; i<n; ++i) {
      for (j=0; j<n; ++j) free(R[i][j]);
      free(R[i]);
   }
   free(R);
}
int main ( int argc, char *argv[] )
{
   int n;
   char alpha[MAX_LEN+1];
   char T[4][4], c;

   srand((unsigned int)time(NULL));
   n = (argc >= 2) ? atoi(argv[1]) : N_DFLT;
   gentable(T);
   genalpha(alpha,n);
   printf("\n+++ Checking realizability\n");
   for (c='a'; c<='d'; ++c) isrealizable(c,alpha,n,T);
   printf("\n+++ Counting possibilities\n");
   countpossibilities(alpha,n,T);

   exit(0);
}