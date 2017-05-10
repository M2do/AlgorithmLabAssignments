#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define CAPACITY 100
#define MAX_POSITIVE 20
#define MAX_NEGATIVE 10
#define POS_PROBABILITY 0.667
#define INFINITY 1234567890

/* Generate a random array of size s. The flag POS_ONLY dictates whether
   the elements are all positive or a mixture of positive and negative
   values. */
int *genarray ( int s, int POS_ONLY )
{
   int *A, i;

   A = (int *)malloc(s * sizeof(int));
   if (POS_ONLY) for (i=0; i<s; ++i) A[i] = 1 + rand() % MAX_POSITIVE;
   else for (i=0; i<s; ++i) {
      if ((double)rand() / (double)RAND_MAX <= POS_PROBABILITY)
         A[i] = 1 + rand() % MAX_POSITIVE;
      else
         A[i] = -(1 + rand() % MAX_NEGATIVE);
   }
   for (i=0; i<s; ++i) {
      if (i % 16 == 0) printf("\n    ");
      printf("%4d", A[i]);
   }
   printf("\n");
   return A;
}

/* Greedy packing of m items of A[] */
void packpos ( int *A, int m )
{
   int i, j, k, c; /* i is for reading from A[]. j is the carton number.
                      k is the first index of the item packed in the current
                      carton. c is a variable standing for the sum of costs
                      of the items packed in the current carton. */

   printf("\n+++ Packing items of positive costs\n");

   i = k = c = 0; j = 1;
   while (1) {
      /* Keep on adding items to the current carton until the carton capacity
         is exceeded or the input ends.*/
      c += A[i++];
      if ((i < m) && (c < CAPACITY)) continue;

      if (c > CAPACITY) c -= A[--i]; /* Discard last item */

      printf("    Carton %2d: Packing A[%-2d ... %2d]: Total cost = %3d\n", j, k, i-1, c);

      if (i == m) break; /* End of input array */

      ++j; k = i; c = 0; /* Prepare for packing in the next carton. */
   }

   printf("*** Total number of cartons needed = %d\n", j);
}

/* A[] contains both positive and negative integers. We use dynamic programming
   to optimally pack the items of A[].

   We use an array T[] such that T[i] stores the minimum count of cartons
   needed for packing A[i...m-1]. We compute T[i] for i = m, m-1, ..., 1, 0
   (in that order). The boundary condition is T[m] = 0. For i < m, we have
   T[i] = 1 + min T[j+1], where the minimum is taken over all indices j
   such that i <= j <= m-1 and sum(A[i...j]) <= CAPACITY. T[0] is the desired
   output count.

   We use a second array S[] to reconstruct an optimal solution. Let T[i] be
   minimized at j = J. We store S[i] = J indicating that A[i...S[i]] should
   be packed in the first carton to minimize the carton count for packing the
   subarray A[i...n-1]. After all T[i] and S[i] values are computed, we
   construct an optimal packing for the entire A[] as follows. Carton 1
   gets the items A[0...S[0]], Carton 2 gets the items A[S[0]+1...S[S[0]+1]],
   and so on.

   *********************************** NOTE **********************************
   *  The greedy algorithm of covering as many elements as possible in the   *
   *  first carton (and repeat for the rest) may fail to produce an optimal  *
   *  solution. For example, take C = 100 and A = [100, 50, -100, 100, 1].   *
   *  Greedy packing requires three cartons: [100, 50, -100], [100] and [1]. *
   *  But the optimal solution is two cartons: [100], [50, -100, 100, 1].    *
   ***************************************************************************
*/
void packposneg ( int *A, int m )
{
   int *T, *S;
   int i, j, k, c;

   printf("\n+++ Packing items of both positive and negative costs\n");

   T = (int *)malloc((m + 1) * sizeof(int));
   S = (int *)malloc((m + 1) * sizeof(int));
   T[m] = S[m] = 0; /* No cartons are needed to pack A[m,m-1] */

   /* Populate T[i] and S[i] for i=m-1,m-2,...,1,0 (in that order) */
   for (i=m-1; i>=0; --i) {
      c = 0; T[i] = INFINITY; S[i] = -1;
      for (j=i; j<m; ++j) {  /* Look up to the end of A[] */
         c += A[j];
         if ((c <= CAPACITY) && (1 + T[j+1] <= T[i])) { /* Better packing found */
            T[i] = 1 + T[j+1];
            S[i] = j;
         }
      }
   }

   /* Print an optimal solution. S[] gives an optimal packing for each carton. */
   i = j = 0;
   while (i < m) {
      ++j;
      c = 0; for (k=i; k<=S[i]; ++k) c += A[k];
      printf("    Carton %2d: Packing A[%-2d ... %2d]: Total cost = %d\n", j, i, S[i], c);
      i = S[i] + 1;
   }

   printf("*** Total number of cartons needed = %d\n", T[0]);

   free(T); free(S);
}

/* Optimal packing of two positive-valued sequences of items, stored in arrays
   A[] and B[] of sizes m and n, respectively. Dynamic programming is used.

   We use a two-dimensional array T[][]. T[i][j] stores the minimum carton
   count for the subarrays A[i...m-1], B[j...n-1].

   Boundary conditions:

   (1) T[m][n] = 0.

   (2) T[m][j] = 1 + T[m][J+1], where j <= J <= n-1, and J is the largest such
       index for which sum(B[j...J]) <= CAPACITY. In this case, array A[] is
       exhausted. In order to pack only one positive-valued array (the rest
       of B[]), we can take a greedy approach.

   (3) T[i][n] = 1 + T[I+1][n], where i <= I <= m-1, and I is the largest such
       index for which sum(A[i...I]) <= CAPACITY. This choice is again greedy.

   Recurrence: Let i < m and j < n. Then T[i][j] = 1 + min T[I+1][J+1], where
   the minimum is taken over all indices I,J satisfying the conditions:
   (a) i-1 <= I <= m-1, (b) j-1 <= J <= n-1, and (c) sum(A[i...I],B[j...J]) <=
   CAPACITY. The table is populated in the "reverse" row-major order
   (starting at m-1,n-1 and ending at 0,0). T[0][0] is the optimal carton
   count for the full arrays A[], B[].

   For reconstructing an optimal solution, we populate two other two-dimensional
   arrays R[][] and S[][] alongside T[][]. If T[i][j] is minimized for indices
   I,J, we store R[i][j] = I and S[i][j] = J. In boundary case 2 (resp. 3),
   we take I = m (resp. J = n).

   The first carton packs A[0...R[0][0]], B[0...S[0][0]], the second carton
   packs A[R[0][0]+1...R[R[0][0]+1,S[0][0]+1]],
   B[S[0][0]+1...S[[R[0][0]+1,S[0][0]+1]], and so on.
*/
void packpostwo ( int *A, int *B, int m, int n )
{
   int **T, **R, **S;
   int i, j, i1, j1, k, c, d;

   printf("\n+++ Packing two sets of items with positive costs (dynamic programming, optimal)\n");

   T = (int **)malloc((m+1)*sizeof(int *));
   R = (int **)malloc((m+1)*sizeof(int *));
   S = (int **)malloc((m+1)*sizeof(int *));
   for (i=0; i<=m; ++i) {
      T[i] = (int *)malloc((n+1) * sizeof(int));
      R[i] = (int *)malloc((n+1) * sizeof(int));
      S[i] = (int *)malloc((n+1) * sizeof(int));
   }

   /* Packing A[m,m-1],B[n,n-1] requires no cartons */
   T[m][n] = 0; R[m][n] = m-1; S[m][n] = n-1;

   /* Array B[] exhausted. We only need to pack the remaining elements of
      A[], that is, we compute T[i][n] as boundary conditions. This is similar
      to packpos(), so greedy approach can be taken. */
   for (i=m-1; i>=0; --i) {
      c = 0; i1 = i;
      while (1) {
         c += A[i1++];
         if ((i1 < m) && (c < CAPACITY)) continue;
         if (c > CAPACITY) { c -= A[--i1]; break; }
         if (i1 == m) break;
      }
      T[i][n] = 1 + T[i1][n]; R[i][n] = i1-1; S[i][n] = n-1;
   }

   /* Array A[] exhausted. We only need to pack the remaining elements of
      B[], that is, we compute T[m][j] as boundary conditions. This is similar
      to packpos(), so greedy approach can be taken. */
   for (j=n-1; j>=0; --j) {
      c = 0; j1 = j;
      while (1) {
         c += B[j1++];
         if ((j1 < n) && (c < CAPACITY)) continue;
         if (c > CAPACITY) { c -= B[--j1]; break; }
         if (j1 == n) break;
      }
      T[m][j] = 1 + T[m][j1]; R[m][j] = m-1; S[m][j] = j1-1;
   }

   /* Populate the rest of T[][] in a row-major order with decreasing i and j
      values. */
   for (i=m-1; i>=0; --i) {     /* Vary i */
      for (j=n-1; j>=0; --j) {  /* Vary j */
         T[i][j] = INFINITY; R[i][j] = S[i][j] = -1; /* Initialize */
         c = 0; i1 = i-1;
         while (1) {     /* For a fixed i1 >= i, check for different j1 >= j */
            d = c; j1 = j-1;
            while (1) {  /* Repeat as long as B[] is not exhausted and capacity is not exceeded */
               if (1+T[i1+1][j1+1] <= T[i][j]) {     /* Better step found */
                  T[i][j] = 1 + T[i1+1][j1+1];
                  R[i][j] = i1; S[i][j] = j1;
               }
               if (++j1 == n) break;                 /* B[] exhausted */
               d += B[j1];
               if (d > CAPACITY) break;              /* Capacity exceeded */
            }
            if (++i1 == m) break;                    /* A[] exhausted */
            c += A[i1];
            if (c > CAPACITY) break;                 /* Capacity exceeded */
         }
      }
   }

   /* Print an optimal solution, guided by the arrays R[][] and S[][] */
   i = j = k = 0;
   while ((i < m) || (j < n)) {
      /* Carton k is going to contain A[i...R[i,j]] and B[j...S[i][j] */
      ++k; c = 0;
      for (i1=i; i1<=R[i][j]; ++i1) c += A[i1];
      for (j1=j; j1<=S[i][j]; ++j1) c += B[j1];
      printf("    Carton %2d: Packing ", k);
      if (i <= R[i][j]) printf("A[%-2d ... %2d] ", i, R[i][j]); else printf("             ");
      if (j <= S[i][j]) printf("B[%-2d ... %2d] ", j, S[i][j]); else printf("             ");
      printf(": Total cost = %d\n", c);
      i1 = i; j1 = j;
      i = R[i1][j1] + 1; j = S[i1][j1] + 1; /* Prepare for the next carton */
   }

   printf("*** Total number of cartons needed = %d\n", T[0][0]);

   for (i=0; i<=m; ++i) { free(T[i]); free(R[i]); free(S[i]); }
   free(T); free(R); free(S);
}

/* This is a greedy algorithm for packing positive-valued arrays A[] and B[].
   This may fail to give an optimal solution. Run the program a few times,
   and verify it yourself. */
void packpostwogreedy ( int *A, int *B, int m, int n, int ROLES_REVERSED )
{
   int i, j, k, l, l1, c;

   printf("\n+++ Packing two sets of items with positive costs (greedy, need not be optimal)\n");

   i = k = l = c = 0; j = 1;
   while (1) {
      /* First try to pack as many elements from A[] as possible */
      c += A[i++];
      if ((i < m) && (c < CAPACITY)) continue;
      if (c > CAPACITY) c -= A[--i];

      /* If there is some leftover space, pack as many elements from B[] as possible */
      l1 = l;
      while (1) {
         c += B[l1++];
         if ((l1 < n) && (c < CAPACITY)) continue;
         if (c > CAPACITY) { c -= B[--l1]; break; }
         if (l1 == n) break;
      }

      if (!ROLES_REVERSED) {
         if (l1 == l)
            printf("    Carton %2d: Packing A[%-2d ... %2d]              : Total cost = %3d\n", j, k, i-1, c);
         else
            printf("    Carton %2d: Packing A[%-2d ... %2d] B[%-2d ... %2d] : Total cost = %3d\n", j, k, i-1, l, l1-1, c);
      } else {
         if (l1 == l)
            printf("    Carton %2d: Packing              B[%-2d ... %2d] : Total cost = %3d\n", j, k, i-1, c);
         else
            printf("    Carton %2d: Packing A[%-2d ... %2d] B[%-2d ... %2d] : Total cost = %3d\n", j, l, l1-1, k, i-1, c);
      }
      ++j; c = 0; k = i; l = l1;
      if (i == m) break;
   }

   if (l == n) --j;  /* B[] is exhausted, so no more cartons are needed */
   else {
      while (1) {
         /* Try to pack as many elements from B[] as possible */
         c += B[l++];
         if ((l< n) && (c < CAPACITY)) continue;
         if (c > CAPACITY) c -= B[--l];
         if (!ROLES_REVERSED)
            printf("    Carton %2d: Packing              B[%-2d ... %2d] : Total cost = %3d\n", j, l1, l-1, c);
         else
            printf("    Carton %2d: Packing A[%-2d ... %2d]              : Total cost = %3d\n", j, l1, l-1, c);
         if (l == n) break;
         ++j; l1 = l; c = 0;
      }
   }

   printf("*** Total number of cartons needed = %d\n", j);
}

int main ( int argc, char *argv[] )
{
   int m, n;
   int *A, *B, *C;

   if (argc >= 3) { m = atoi(argv[1]); n = atoi(argv[2]); }
   else scanf("%d%d", &m, &n);

   srand((unsigned int)time(NULL));

   printf("\n+++ Input array with positive entries");
   A = genarray(m,1);

   packpos(A,m);

   printf("\n+++ Input array with positive and negative entries");
   C = genarray(m,0);

   packposneg(C,m);

   printf("\n+++ Input array with positive entries");
   B = genarray(n,1);

   packpostwo(A,B,m,n);

   /* The following two calls establish that the straightforward greedy
      approach may fail to output the optimal solution. This is not
      included in the assignment. */
   packpostwogreedy(A,B,m,n,0);
   packpostwogreedy(B,A,n,m,1);

   free(A); free(B); free(C);

   exit(0);
}