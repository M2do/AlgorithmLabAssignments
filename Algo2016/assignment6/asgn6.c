/* Constructive program to validate graphic and multi-graphic sequences */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N_DFT 10
#define N_MAX 100
#define INFINITY 999999999

typedef struct {
   int id;
   int deg;
} pair;

void genseq ( pair A[], int n, int B )
{
   int i, sum;

   printf("\nSequence:");
   for (i=sum=0; i<n; ++i) {
      A[i].id = i;
      sum += (A[i].deg = rand() % B);
   }
   if (sum % 2) {
      if (A[n-1].deg == 0) A[n-1].deg = 1;
      else --A[n-1].deg;
   }
   for (i=0; i<n; ++i) printf(" %3d", A[i].deg);
   printf("\n");
}

int compact ( pair A[], int n, int *k )
{
   int i, j;

   i = 0; if (k) *k = 0;
   while ((i < n) && (A[i].deg)) {
      if ((k) && (A[i].deg > *k)) *k = A[i].deg;
      ++i;
   }
   j = i;
   while (i < n) {
      if (A[i].deg) {
         if ((k) && (A[i].deg > *k)) *k = A[i].deg;
         A[j] = A[i];
         ++j;
      }
      ++i;
   }
   return j;
}

void csort ( pair A[], int n, int k )
{
   int i, d;
   pair B[N_MAX];
   int C[N_MAX];

   for (d=0; d<=k; ++d) C[d] = 0;
   for (i=0; i<n; ++i) {
      d = A[i].deg;
      ++C[d];
   }
   for (i=k; i>=1; --i) C[i] = C[i-1];
   for (i=1; i<=k; ++i) C[i] += C[i-1];
   for (i=0; i<n; ++i) {
      d = A[i].deg;
      B[C[d]] = A[i];
      ++C[d];
   }
   for (i=0; i<n; ++i) A[i] = B[n-i-1];
}

/* This is the friendship function, implementation of Havel--Hakimi algorithm */
int gengraph ( int M[][N_MAX], pair A[], int n )
{
   int i, j, k;

   for (i=0; i<n; ++i) for (j=0; j<n; ++j) M[i][j] = 0;
   while (1) {
      n = compact(A,n,&k);
      if (n == 0) break;
      if (k >= n) { printf("+++ Invalid sequence\n"); return 0; }
      csort(A,n,k);
      k = A[0].deg;
      A[0].deg = 0;
      for (i=1; i<=k; ++i) {
         --A[i].deg;
         M[A[0].id][A[i].id] = M[A[i].id][A[0].id] = 1;
      }
   }
   return 1;
}

/* This is the handshake function, the Boesch--Harary algorithm */
int genmultigraph ( int M[][N_MAX], pair A[], int n )
{
   int i, j, k, l, m;

   for (i=0; i<n; ++i) for (j=0; j<n; ++j) M[i][j] = 0;
   while (1) {
      n = compact(A,n,NULL);
      if (n == 0) break;
      if (n == 1) { printf("+++ Invalid sequence\n"); return 0; }
      if (n == 2) {
         k = A[0].deg;
         if (A[1].deg != k) { printf("+++ Invalid sequence\n"); return 0; }
         i = A[0].id; j = A[1].id;
         M[i][j] += k; M[j][i] += k;
         return 1;
      }
      /* Three linear equations, three variables, so unique solution. Just
         check that the solution makes sense. */
      if (n == 3) {
         i = A[0].id; j = A[1].id; k = A[2].id;
         l = (A[0].deg + A[1].deg - A[2].deg) / 2;
         if (l < 0) { printf("+++ Invalid sequence\n"); return 0; }
         M[i][j] += l; M[j][i] += l;
         l = (A[0].deg - A[1].deg + A[2].deg) / 2;
         if (l < 0) { printf("+++ Invalid sequence\n"); return 0; }
         M[i][k] += l; M[k][i] += l;
         l = (A[1].deg + A[2].deg - A[0].deg) / 2;
         if (l < 0) { printf("+++ Invalid sequence\n"); return 0; }
         M[j][k] += l; M[k][j] += l;
         return 1;
      }
      j = k = 0; l = m = A[0].deg;
      for (i=1; i<n; ++i) {
         if (A[i].deg <= l) { l = A[i].deg; j = i; } /* <= (not <) to ensure that the min */
         if (A[i].deg > m) { m = A[i].deg; k = i; }  /* and max indices are different */
      }
      M[A[j].id][A[k].id] = l; M[A[k].id][A[j].id] = l;
      A[j].deg = 0; A[k].deg = m-l;
   }
   return 1;
}

void prngraph ( int M[][N_MAX], int n )
{
   int i, j, rowsum, colsum[N_MAX];

   for (i=0; i<n; ++i) colsum[i] = 0;
   printf("\n         ");
   for (i=0; i<n; ++i) printf(" %3d", i+1); printf("    Sum");
   printf("\n");
   printf("        +"); for (i=0; i<=4*n; ++i) printf("-"); printf("+\n");
   for (i=0; i<n; ++i) {
      printf("    %3d |", i+1);
      rowsum = 0;
      for (j=0; j<n; ++j) {
         if (M[i][j]) {
            printf(" %3d", M[i][j]);
            rowsum += M[i][j];
            colsum[j] += M[i][j];
         } else {
            printf("    ");
         }
      }
      printf(" | %3d\n", rowsum);
   }
   printf("        +"); for (i=0; i<=4*n; ++i) printf("-"); printf("+\n");
   printf("    Sum  ");
   for (j=0; j<n; ++j) printf(" %3d", colsum[j]);
   printf("\n");
}

int main ( int argc, char *argv[] )
{
   int n;
   pair A[N_MAX];
   int M[N_MAX][N_MAX];

   if (argc == 1) n = N_DFT; else n = atoi(argv[1]);
   printf("n = %d\n", n);

   srand((unsigned int)time(NULL));

   genseq(A,n,n);
   if (gengraph(M,A,n)) prngraph(M,n);

   genseq(A,n,n*n);
   if (genmultigraph(M,A,n)) prngraph(M,n);

   exit(0);
}