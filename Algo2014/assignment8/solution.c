#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 100
#define M 5

/* Random string of length n over an alphabet of size m */
char *genarr ( int n, int m )
{
   int i, t, r;
   char *A;

   A = (char *)malloc((n+1)*sizeof(char));
   A[n] = '\0';
   t = rand() % m; /* The character that is going to have frequency about n/2 */
   for (i=0; i<n; ++i) {
      if (rand() % 2) A[i] = 'a' + t;
      else {
         while ((r = rand() % m) == t);
         A[i] = 'a' + r;
      }
   }
   return A;
}

/* Return the frequency of x in A[] */
char findfreq ( char *A, int n, char x )
{
   int i, f = 0;

   for (i=0; i<n; ++i) if (A[i] == x) ++f;
   return f;
}

/* Return the majority element in A[], or -1 if no such element exists */
char findmaj ( char *A, int n )
{
   char *B, x;
   int i, j;

   printf("--- %s\n", A);

   /* Boundary cases */
   if (n == 0) return (char)(-1);
   if (n == 1) return A[0];

   /* Handle odd values of n */
   if (n % 2 == 1) {
      /* Manually check whether the last element is a majority element */
      if (findfreq(A,n,A[n-1]) > n / 2) return A[n-1];

      /* If not, forget the last element */
      --n;
   }

   /* Prepare the array for the recursive call */
   B = (char *)malloc((n/2+1)*sizeof(char));
   j = 0;
   for (i=0; i<n; i+=2) if (A[i] == A[i+1]) B[j++] = A[i];
   B[j] = '\0';

   /* Recursively find the majority in B[] */
   x = findmaj(B,j);

   free(B);

   /* If B[] does not have a majority element */
   if (x == (char)(-1)) return x;

   /* B[] has a majority element. We need to check whether that is the
      majority element in A[] too. */
   if (findfreq(A,n,x) > n / 2) return x;
   return (char)(-1);
}

/* For printing diagnostic messages */
void diagnostic ( char *A, int n, int m )
{
   int i;

   for (i=0; i<m; ++i)
      printf("+++ Frequency(%c) = %d\n", 'a'+i, findfreq(A,n,'a'+i));
}

int main ( int argc, char *argv[] )
{
   int n, m;
   char *A, x;

   if (argc >= 3) { n = atoi(argv[1]); m = atoi(argv[2]); }
   else { n = N; m = M; }

   srand((unsigned int)time(NULL));

   A = genarr(n,m);

   x = findmaj(A,n);
   if (x == (char)(-1)) {
      printf("No majority element exists\n");
      diagnostic(A,n,m);
   } else {
      printf("Majority element is %c with frequency %d\n", x, findfreq(A,n,x));
   }

   free(A);
   exit(0);
}