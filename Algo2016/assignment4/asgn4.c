#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define DFT_M 64
#define DFT_N 60
#define DFT_K 10

typedef struct {
   int i, j;
} pair;

int *genarray ( int n )
{
   int i, *A;

   A = (int *)malloc((n + 1) * sizeof(int));
   for (i=1; i<=n; ++i) {
      A[i] = 1 + rand() % 1999;
      printf("%5d", A[i]);
      if (i % 20 == 0) printf("\n");
   }
   if (i % 20 != 1) printf("\n");
   return A;
}

void heapify ( int H[], int n, int i )
{
   int l, r, min, t;

   while (1) {
      l = 2*i; r = 2*i+1;
      if (l > n) return;
      min = ((l == n) || (H[l] <= H[r])) ? l : r;
      if (H[min] >= H[i]) return;
      t = H[i]; H[i] = H[min]; H[min] = t;
      i = min;
   }
}

void makeHeap ( int H[], int n )
{
   int i;

   for (i=n/2; i>0; --i) heapify(H,n,i);
}

void deleteMin ( int H[] , int n )
{
   H[1] = H[n];
   heapify(H,n-1,1);
}

void method1 ( int A[], int B[], int m, int n, int k )
{
   int i, j, l, *S, size;

   printf("\n+++ Method 1:\n");
   size = m * n;
   S = (int *)malloc((1 + size) * sizeof(int));
   S[0] = 0;
   l = 0;
   for (i=1; i<=m; ++i) {
      for (j=1; j<=n; ++j) {
         S[++l] = A[i] + B[j];
      }
   }
   makeHeap(S,size);
   l = 0;
   while (l<k) {
      printf(" %4d", S[1]);
      ++l; if (l % 20 == 0) printf("\n");
      deleteMin(S,size);
      --size;
   }
   if (l % 20) printf("\n");
   free(S);
}

void heapifyQ ( pair Q[], int A[], int B[], int size, int i )
{
   int l, r, min;
   pair t;

   while (1) {
      l = 2*i; r = 2*i+1;
      if (l > size) return;
      min = ((l == size) || (A[Q[l].i]+B[Q[l].j] <= A[Q[r].i]+B[Q[r].j])) ? l : r;
      if (A[Q[min].i]+B[Q[min].j] >= A[Q[i].i]+B[Q[i].j]) return;
      t = Q[i]; Q[i] = Q[min]; Q[min] = t;
      i = min;
   }
}

void insertQ ( pair Q[] , int A[], int B[], int m, int n, int size, pair idx )
{
   int i, p;

   Q[(i = size + 1)] = idx;
   while (1) {
      if (i == 1) return;
      p = i / 2;
      if (A[Q[p].i]+B[Q[p].j] <= A[Q[i].i]+B[Q[i].j]) return;
      Q[i] = Q[p]; Q[p] = idx;
      i = p;
   }
}

/* Find out whether (i,j) was printed earlier. The indices printed
   are stored in P[] of size l. P[] is sorted with respect to the
   sum A[i]+B[j]. So we can do binary search in P[]. However, the
   same sum may be realized by multiple pairs. So over all locations
   in P[] in which the sum matches, we check whether (i,j) also
   matches. */
int printed ( int A[], int B[], pair P[], int l, int i, int j )
{
   int L, U, M, sum, t;

   if ((l == 0) || (i == 0) || (j == 0)) return 0;

   /* Standard binary search */
   L = 0; U = l-1; sum = A[i]+B[j];
   while (L < U) {
      M = (L + U) / 2;
      if (sum > A[P[M].i] + B[P[M].j]) L = M+1; else U = M;
   }

   /* Unsuccessful search */
   if (sum != A[P[L].i] + B[P[L].j]) return 0;

   /* Successful search. We assume that the following two loops run for
      a constant number of times. */
   t = L;
   while ((t >= 0) && (sum == A[P[t].i] + B[P[t].j])) {  /* sum matches */
      if ((i == P[t].i) && (j == P[t].j)) return 1;      /* indices also match */
      --t;
   }
   t = L + 1;
   while ((t < l) && (sum == A[P[t].i] + B[P[t].j])) {   /* sum matches */
      if ((i == P[t].i) && (j == P[t].j)) return 1;      /* indices also match */
      ++t;
   }
   return 0;
}

/* Try to insert (i,j) to Q */
int tryInsertQ ( pair Q[] , int A[], int B[], int m, int n, int size,
                 int i, int j, pair P[], int l )
{
   /* If i or j is an invalid index, the attempt fails */
   if ((i > m) || (j > n)) return 0;

   /* If (i,j) was inserted earlier, do not re-insert it. Note that
      (i,j) is inserted if and only if one of (i/2,j) or (i,j/2) is
      printed. */
   if ((printed(A,B,P,l,i/2,j)) || (printed(A,B,P,l,i,j/2))) return 0;

   /* No problems with this (i,j), so insert it */
   insertQ(Q,A,B,m,n,size,(pair){i,j});
   return 1;
}

void deleteQ ( pair Q[] , int A[], int B[], int size )
{
   Q[1] = Q[size];
   heapifyQ(Q,A,B,size-1,1);
}

void method3 ( int A[], int B[], int m, int n, int k )
{
   int i, j, l, size;
   pair *Q, *P;

   printf("\n+++ Method 2:\n");
   makeHeap(A,m);
   makeHeap(B,n);
   Q = (pair *)malloc((4*k + 1) * sizeof(pair));
   P = (pair *)malloc(k * sizeof(pair));
   Q[(size = 1)] = (pair){1,1};
   l = 0;
   while (l < k) {
      i = Q[1].i; j = Q[1].j;

      /* Try the following four insertions */
      if (tryInsertQ(Q,A,B,m,n,size,i,2*j,P,l)) ++size;
      if (tryInsertQ(Q,A,B,m,n,size,i,2*j+1,P,l)) ++size;
      if (tryInsertQ(Q,A,B,m,n,size,2*i,j,P,l)) ++size;
      if (tryInsertQ(Q,A,B,m,n,size,2*i+1,j,P,l)) ++size;

      printf("     %4d = A[%2d] + B[%2d] = %3d + %3d\n",
             A[i]+B[j], i, j, A[i], B[j]);
      P[l++] = Q[1];  /* Store the pair for which the sum is printed */
      deleteQ(Q,A,B,size);
      --size;
   }
   free(Q);
   free(P);
}

int main ( int argc, char *argv[] )
{
   int m, n, k;
   int *A, *B;

   srand((unsigned int)time(NULL));
   if (argc >= 4) {
      m = atoi(argv[1]);
      n = atoi(argv[2]);
      k = atoi(argv[3]);
   } else {
      m = DFT_M;
      n = DFT_N;
      k = DFT_K;
   }
   printf("\nm = %d\n", m); printf("\n+++ Array A:\n"); A = genarray(m);
   printf("\nn = %d\n", n); printf("\n+++ Array B:\n"); B = genarray(n);
   printf("\nk = %d\n", k);
   method1(A,B,m,n,k);
   /* method2() not implemented */
   method3(A,B,m,n,k);

   exit(0);
}