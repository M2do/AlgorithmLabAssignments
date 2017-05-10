/* Implements the two- and three-dimensional rank-finding algorithms         *
 * described in the paper: Jon Louis Bentley, "Multidimensional              *
 * Divide-and-Conquer", Cummunications of ACM, 23(4), April 1980, 214--229.  *
 *                                                                           *
 * Running times: O(n log n) in 2-d, and O(n log^2 n) in 3-d.                *
 *                                                                           *
 * Implemented by: Abhijit Das, Date: 08-October-2014                        */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* Default number of points */
#define N 64

typedef struct {
   double coord[3];   /* The three coordinates x, y and z */
   int ypart, zpart;  /* One of the two halves to which a point belongs during merging */
} point;

/* Randomly generate n points in the unit square */
point *genpoints ( int n )
{
   point *A;
   int i;

   A = (point *)malloc(n * sizeof(point));
   for (i=0; i<n; ++i) {
      A[i].coord[0] = (double)rand() / (double)RAND_MAX;
      A[i].coord[1] = (double)rand() / (double)RAND_MAX;
      A[i].coord[2] = (double)rand() / (double)RAND_MAX;
      A[i].ypart = A[i].zpart = -1;
   }
   return A;
}

/* Merge sort with respect to coordinate c (0, 1 or 2). Stores in idx[]
   the indices of the points of A in increasing order of the c-th
   coordinate values. */
void msort ( point *A, int n, int *idx, int c )
{
   int *index, i, j, i1, j1, i2, j2, k;

   if (n <= 1) return;

   i1 = 0; i2 = (n - 1) / 2;
   j1 = i2 + 1; j2 = n - 1;

   /* Recursive calls */
   msort(A,i2-i1+1,idx,c);
   msort(A,j2-j1+1,idx+(i2-i1+1),c);

   /* Merging step */
   index = (int *)malloc(n*sizeof(int));
   k = 0; i = i1; j = j1;
   while ((i <= i2) || (j <= j2)) {
      if (i > i2) index[k] = idx[j++];
      else if (j > j2) index[k] = idx[i++];
      else if (A[idx[i]].coord[c] < A[idx[j]].coord[c]) index[k] = idx[i++];
      else index[k] = idx[j++];
      ++k;
   }

   for (k=0; k<n; ++k) idx[k] = index[k];

   free(index);
}

/* Print the points stored in A[]. If idx is NULL, print the points in the
   same order stored in A[]. Otherwise, print the points following the
   indices stored in idx[]. This is needed, for example, during printing
   the sorted list with respect to the individual coordinates. */
void prnpoints ( point *A, int n, int *idx )
{
   int i, j;

   for (i=0; i<n; ++i) {
      j = (idx == NULL) ? i : idx[i]; 
      printf("(%lf,%lf,%lf) ", A[j].coord[0], A[j].coord[1], A[j].coord[2]);
      if (i % 4 == 3) printf("\n");
   }
   if (i % 4) printf("\n");
}

/* This function recursively computes the superiority indices (ranks).
   Input variables: The array A[] of n points. The sorted list of indices
      of these n points with respect to y- and x-coordinates (yidx[] and
      xidx[]), and a flag zflag indicating whether ranks are computed
      in dimension 2 (value 0) or during the merging step for dimension
      3 (any on-zero value).
   Output variables: The entries of the rank array sid[] are incremented
      as domination instances (superiority cases) are located. */
void calcsupidx2 ( point *A, int n, int *yidx, int *xidx, int *sid, int zflag )
{
   int i, j, k0, k1, n0;
   int *xidx0, *xidx1;
   int *ypart;

   if (n <= 1) return;

   /* This function will nake two recusrsive calls on the lower and higher
      halves (L and H). The ypart of each point in L is set to zero and the
      ypart of each point in H is set to one (or anything non-zero). But
      this destroys the old ypart values of the points as used by the
      caller function(s). So we first keep a copy of the old ypart vaues
      in the local array ypart[]. */
   ypart = (int *)malloc(n * sizeof(int));
   for (i=0; i<(n+1)/2; ++i) {
      j = yidx[i]; ypart[i] = A[j].ypart; A[j].ypart = 0;
   }
   for (i=0; i<n/2; ++i) {
      j = yidx[(n+1)/2+i]; ypart[(n+1)/2+i] = A[j].ypart; A[j].ypart = 1;
   }

   /* In the 2-d case, the points are considered in the sorted sequence
      with respect to the y-coordinates. They occupy contigous blocks in
      yidx[]. But the positions of the points in xidx[] are in general
      not contiguous. We will create two local xidx arrays for the
      recursive calls with the x-coordinates of the points in each
      half in the sorted order. xidx0[] is for L, and xidx1[] is for H. */
   xidx0 = (int *)malloc(((n+1)/2) * sizeof(int));
   xidx1 = (int *)malloc((n/2) * sizeof(int));

   for (i=k0=k1=0; i<n; ++i) {
      j = xidx[i];
      if (A[j].ypart == 0) xidx0[k0++] = j;
      else if (A[j].ypart == 1) xidx1[k1++] = j;
      else printf("*** Error 1 in calcsupidx2()\n");
   }

   /* Now we are ready for the two recursive calls */

   calcsupidx2(A,(n+1)/2,yidx,xidx0,sid,zflag);
   calcsupidx2(A,n/2,yidx+(n+1)/2,xidx1,sid,zflag);

   /* After the recursive calls return, sid[] stores the ranks of points
      of L in L, and the points of H in H. But we need the ranks of all
      the points in A, so require a merging step. For that, we look at
      the points in A in the ascending order of their x-coordinates.

      Each point in H has a higher y-coordinate than each point in
      L, so no point in L can dominate over any point in H. However,
      points in H may dominate points in L. They already dominate
      with respect to the y-coordinates. We only need to check the
      number of L points with x-coordinates smaller than each H point. */

   /* Initialize the number of L points to zero */
   n0 = 0;
   if (zflag == 0) { /* The case of 2-d */
      for (i=0; i<n; ++i) {
         j = xidx[i];
         if (A[j].ypart == 0) ++n0;              /* A[j] belongs to L, so increment n0 */
         else if (A[j].ypart == 1) sid[j] += n0; /* A[j] belongs to H, so increment its rank by n0 */
         else printf("*** Error 2 in calcsupidx2()\n");
      }
   } else {         /* The modified case of merging for 3-d */
      /* In this case, the initial breaking of A into L and H is with respect
         to the z-coordinates. Now we count, for each point in H, the number
         of points in L that are dominated by the H point. Therefore, we
         additionally look at the zparts of the points. */
      for (i=0; i<n; ++i) {
         j = xidx[i];
         if ((A[j].ypart == 0) && (A[j].zpart == 0)) ++n0;
         else if ((A[j].ypart == 1) && (A[j].zpart == 1)) sid[j] += n0;
      }
   }

   /* Finally, restore the yparts of points in A to the old values in
      order that these values are consistent with the caller function. */
   for (i=0; i<n; ++i) { j = yidx[i]; A[j].ypart = ypart[i]; }

   /* Free locally allocated memory */
   free(xidx0); free(xidx1); free(ypart);
}

/* Naive quadratic-time algorithm to check the correctness of calcsupidx2() */
void diagnostic2 ( point *A, int n, int *sid )
{
   int i, j;

   for (i=0; i<n; ++i) {
      for (j=i+1; j<n; ++j) {
         if ((A[i].coord[0] > A[j].coord[0]) && (A[i].coord[1] > A[j].coord[1])) sid[i]++;
         else if ((A[i].coord[0] < A[j].coord[0]) && (A[i].coord[1] < A[j].coord[1])) sid[j]++;
      }
   }
}

/* Functions for computing ranks (superiority indices) in 3-d space.
   Input variables: The array A with n points, and the sorted index arrays
      for each of the three dimensions (zidx, yidx and xidx).
   Output variables: Increment the ranks in sid[] */
void calcsupidx3 ( point *A, int n, int *zidx, int *yidx, int *xidx, int *sid )
{
   int i, j, k0, k1;
   int *xidx0, *xidx1, *yidx0, *yidx1;
   int *zpart;

   if (n <= 1) return;

   /* In 3-d, we first divide A with respect to the z-coordinate values.
      L consists of half of the points with the lower z-coordinates, and
      H consists of half of the points with the higher z-coordinates.
      The merging step requires remembering whether a point in A is
      from L or from H. So we modify the zpart values of these points.
      But before doing that, we remember the old zpart values of the
      points in the local array zpart[]. */

   zpart = (int *)malloc(n * sizeof(int));

   for (i=0; i<(n+1)/2; ++i) {
      j = zidx[i]; zpart[i] = A[j].zpart; A[j].zpart = 0;
   }
   for (i=0; i<n/2; ++i) {
      j = zidx[(n+1)/2+i]; zpart[(n+1)/2+i] = A[j].zpart; A[j].zpart = 1;
   }

   /* zidx[] is contiguous. But xidx[] and yidx[] are not so in general.
      So we make local copies for each of the two recursive calls. */

   xidx0 = (int *)malloc((n+1)/2 * sizeof(int));
   xidx1 = (int *)malloc(n/2 * sizeof(int));
   yidx0 = (int *)malloc((n+1)/2 * sizeof(int));
   yidx1 = (int *)malloc(n/2 * sizeof(int));

   for (i=k0=k1=0; i<n; ++i) {
      j = xidx[i];
      if (A[j].zpart == 0) xidx0[k0++] = j;
      else if (A[j].zpart == 1) xidx1[k1++] = j;
      else printf("*** Error 1 in calcsupidx3()\n");
   }

   for (i=k0=k1=0; i<n; ++i) {
      j = yidx[i];
      if (A[j].zpart == 0) yidx0[k0++] = j;
      else if (A[j].zpart == 1) yidx1[k1++] = j;
      else printf("*** Error 2 in calcsupidx3()\n");
   }

   /* Now the stage is set for the two recursive calls. */

   calcsupidx3(A,(n+1)/2,zidx,yidx0,xidx0,sid);
   calcsupidx3(A,n/2,zidx+(n+1)/2,yidx1,xidx1,sid);

   /* After the recursive calls, we need to increment ranks of points
      in H by the counts of points in L dominated by the H points. So
      we solve the problem in two dimension. But now we do not compute
      the 2-d ranks of all points in A. We instead compute the
      cross-ranks between L and H. So we send 1 as zflag. */

   calcsupidx2(A,n,yidx,xidx,sid,1);

   /* Restore the zpart values of the points to the old values for the
      caller function. */
   for (i=0; i<n; ++i) { j = zidx[i]; A[j].zpart = zpart[i]; }

   /* Free locally allocated memory */
   free(xidx0); free(xidx1); free(yidx0); free(yidx1); free(zpart);
}

/* Naive quadratic-time algorithm to check the correctness of calcsupidx3() */
void diagnostic3 ( point *A, int n, int *sid )
{
   int i, j;

   for (i=0; i<n; ++i) {
      for (j=i+1; j<n; ++j) {
         if ((A[i].coord[0] > A[j].coord[0]) && (A[i].coord[1] > A[j].coord[1]) && (A[i].coord[2] > A[j].coord[2])) sid[i]++;
         else if ((A[i].coord[0] < A[j].coord[0]) && (A[i].coord[1] < A[j].coord[1]) && (A[i].coord[2] < A[j].coord[2])) sid[j]++;
      }
   }
}

int main ( int argc, char *argv[] )
{
   int n, i;
   int *xidx, *yidx, *zidx;
   int *supidx;
   point *A;

   srand((unsigned int)time(NULL));

   if (argc > 1) n = atoi(argv[1]); else n = N;

   A = genpoints(n);

   xidx = (int *)malloc(n * sizeof(int));
   yidx = (int *)malloc(n * sizeof(int));
   zidx = (int *)malloc(n * sizeof(int));
   for (i=0; i<n; ++i) xidx[i] = yidx[i] = zidx[i] = i;
   msort(A,n,xidx,0);
   msort(A,n,yidx,1);
   msort(A,n,zidx,2);

   printf("+++ The original points:\n"); prnpoints(A,n,NULL);
   printf("+++ The points sorted wrt x-coordinates:\n"); prnpoints(A,n,xidx);
   printf("+++ The points sorted wrt y-coordinates:\n"); prnpoints(A,n,yidx);
   printf("+++ The points sorted wrt z-coordinates:\n"); prnpoints(A,n,zidx);

   supidx = (int *)malloc(n * sizeof(int));

   for (i=0; i<n; ++i) supidx[i] = 0;
   calcsupidx2(A,n,yidx,xidx,supidx,0);
   printf("+++ Superiority indices in two dimensions (divide-and-conquer):\n");
   for (i=0; i<n; ++i) printf("%3d", supidx[i]);
   printf("\n");

   for (i=0; i<n; ++i) supidx[i] = 0;
   diagnostic2(A,n,supidx);
   printf("+++ Superiority indices in two dimensions (quadratic-time):\n");
   for (i=0; i<n; ++i) printf("%3d", supidx[i]);
   printf("\n");

   for (i=0; i<n; ++i) supidx[i] = 0;
   calcsupidx3(A,n,zidx,yidx,xidx,supidx);
   printf("+++ Superiority indices in three dimensions (divide-and-conquer):\n");
   for (i=0; i<n; ++i) printf("%3d", supidx[i]);
   printf("\n");

   for (i=0; i<n; ++i) supidx[i] = 0;
   diagnostic3(A,n,supidx);
   printf("+++ Superiority indices in three dimensions (quadratic-time):\n");
   for (i=0; i<n; ++i) printf("%3d", supidx[i]);
   printf("\n");

   exit(0);
}