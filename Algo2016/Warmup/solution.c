#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_BASE 32

/* Recursive function for locating digit-counting numbers:
   A[0...idx-1] stores the digits placed so far, A[idx...base-1] is filled
   with zeros, B[] stores the digit counts for A[0...base-1], sum stores the
   sum of the digits in A[]. */
void find ( int A[], int B[], int idx, int sum, int base )
{
   int i, d, found;

   d = 0;
   if (sum == base) idx = base; /* The remaining digits must be zero */
   else if (idx == base-2) {    /* A[base-1] = 0 so put the remaining sum at A[base-2] */
      d = base - sum;
      A[idx] = d;
      ++B[d]; --B[0];
      idx = base;
   }

   if (idx == base) {           /* Construction of A[] is complete */
      found = 1;
      for (i=0; i<base; ++i) {
         if (B[i] != A[i]) {    /* Count mismatch */
            found = 0; break;
         }
      }

      /* If a digit-counting number is found, print it. */
      if (found) {
         printf(" ");
         for (i=0; i<base; ++i)
            printf("%c", (A[i] < 10) ? '0'+A[i] : 'a'+A[i]-10);
         fflush(stdout);
      }

      if (d) {
         A[base-2] = 0; --B[d]; ++B[0];
      }

      return;
   }

   /* A[0] cannot be zero, so don't make recursive call with A[0] = 0 */
   if (idx) find(A,B,idx+1,sum,base);

   /* Put A[idx]=1,2,...,base-sum one by one, and make recursive calls */
   for (i=1; i<=base-sum; ++i) {
      A[idx] = i; ++B[i]; --B[0];  /* Count of i increases, count of 0 decreases */
      find(A,B,idx+1,sum+i,base);  /* Recursive call on the next index */
      --B[i]; ++B[0];              /* Restore counts */
   }

   if (idx) A[idx] = 0;  /* If A[idx] was set, restore it to zero */
}

/* This is a second recursive function for locating digit-counting numbers.
   The first five arguments are as in find(). This function makes use of the
   observation that A[1...base-1] consists only of the digits 0,1,2, with
   2 occurring in exactly one position. */
void findfast ( int A[], int B[], int idx, int sum, int base, int twoused )
{
   int i, d, found;

   d = 0;
   if (sum == base) idx = base; /* The remaining digits must be zero */
   else if (idx == base-2) {    /* A[base-1] = 0 so put the remaining sum at A[base-2] */
      d = base - sum;
      A[idx] = d;
      ++B[d]; --B[0];
      idx = base;
   }

   if (idx == base) {           /* Construction of A[] is complete */
      found = 1;
      for (i=0; i<base; ++i) {
         if (B[i] != A[i]) {    /* Count mismatch */
            found = 0; break;
         }
      }

      /* If a digit-counting number is found, print it. */
      if (found) {
         printf(" ");
         for (i=0; i<base; ++i)
            printf("%c", (A[i] < 10) ? '0'+A[i] : 'a'+A[i]-10);
         fflush(stdout);
      }

      if (d) {
         A[base-2] = 0; --B[d]; ++B[0];
      }

      return;
   }

   if (idx == 0) {
      /* A[0] must be non-zero. Try all non-zero values. */
      for (i=1; i<base; ++i) {
         A[0] = i; ++B[i]; --B[0];
         findfast(A,B,1,sum+i,base,0);
         --B[i]; ++B[0];
      }
   } else {  /* Here idx > 0 */
      /* Put A[idx] = 0 and recurse */
      findfast(A,B,idx+1,sum,base,twoused);

      /* Put A[idx] = 1 and recurse */
      if ((sum < base) && (B[1] < 2)) {
         A[idx] = 1; ++B[1]; --B[0];
         findfast(A,B,idx+1,sum+1,base,(B[1]==2));
         --B[1]; ++B[0];
      }

      /* Put A[idx] = 2 and recurse */
      if ((!twoused) && (sum <= base-2) && (B[2] < 2)) {
         A[idx] = 2; ++B[2]; --B[0];
         findfast(A,B,idx+1,sum+2,base,1);
         --B[2]; ++B[0];
      }

      A[idx] = 0;
   }
}

int main ()
{
   int A[MAX_BASE], B[MAX_BASE], i, base;
   clock_t t1, t2;

   printf("\n+++ Method 1: Uses the observations:\n");
   printf("    (a) A[0] != 0\n");
   printf("    (b) A[0] + A[1] + A[2] + ... + A[base-1] = base\n");
   printf("    (c) A[base-1] = 0\n");
   printf("\n    Hit enter when ready...\n"); getchar();
   t1 = clock();
   for (base=2; base<=16; ++base) {
      printf("base = %2d:", base); fflush(stdout);
      A[0] = 0; B[0] = base; for (i=1; i<base; ++i) A[i] = B[i] = 0;
      find(A,B,0,0,base);
      printf("\n");
   }
   t2 = clock();
   printf("Total time taken: %4.2lf sec\n", (double)(t2-t1) / (double)CLOCKS_PER_SEC);

   printf("\n+++ Method 2: Uses the additional observation:\n");
   printf("    (d) A[1...base-1] consists of 0,1,2 only, and 2 occurs only once\n");
   printf("\n    Hit enter when ready...\n"); getchar();
   t1 = clock();
   for (base=2; base<=MAX_BASE; ++base) {
      printf("base = %2d:", base); fflush(stdout);
      A[0] = 0; B[0] = base; for (i=1; i<base; ++i) A[i] = B[i] = 0;
      findfast(A,B,0,0,base,0);
      printf("\n");
   }
   t2 = clock();
   printf("Total time taken: %4.2lf sec\n", (double)(t2-t1) / (double)CLOCKS_PER_SEC);

   exit(0);
}