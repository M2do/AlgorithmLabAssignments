#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NSYMB 3
#define N 100
#define M 4

char *genstring ( int n, int nsymb )
{
   char *C;
   int i;

   C = (char *)malloc((n + 1) * sizeof(char));
   C[n] = '\0';
   for (i = 0; i < n; ++i) C[i] = 'a' + rand() % nsymb;
   return C;
}

int issubseq ( char *B, int m, char *A, int n )
{
   int i, j;

   i = j = 0;
   while ((i < n) && (j < m)) {
      if (A[i] == B[j]) ++j;
      ++i;
   }
   return (j == m);
}

void prnsubseq ( char *B, int m, int k, char *A, int n )
{
   int i, j, inside = 0;

   printf("%s\n", A);
   i = j = 0;
   while ((i < n) && (j < m * k)) {
      if (A[i] == B[j]) printf("%c", B[j++]);
      else printf(" ");
      ++i;
   }
   printf("\n");
   i = j = 0;
   while ((i < n) && (j < m * k)) {
      if (A[i] == B[j]) {
         if (j % k == 0) { if (k > 1) inside = 1; printf("|"); }
         else if (j % k == k-1) { inside = 0; printf("|");}
         else printf("-");
         ++j;
      } else if (inside) printf("-");
      else printf(" ");
      ++i;
   }
   printf("\n");
}

char *genBi ( char *B, int m, int i )
{
   int j, k, l;
   char *Bi;

   Bi = (char *)malloc((m * i + 1) * sizeof(char));
   Bi[m * i] = '\0';
   l = 0;
   for (j = 0; j < m; ++j)
      for (k = 0; k < i; ++k)
         Bi[l++] = B[j];
   return Bi;
}

int subseqsearch ( char *A, int n, char *B, int m )
{
   int L, R, i;
   char *Bi;

   L = 0; R = n / m;
   while (L < R) {
      i = (L + R + 1) / 2;
      Bi = genBi(B,m,i);
      if (issubseq(Bi,m*i,A,n)) L = i;
      else R = i - 1;
      free(Bi);
   }
   return L;
}

int main ( int argc, char *argv[] )
{
   int nsymb, n, m, i;
   char *A, *B, *Bi;

   srand((unsigned int)time(NULL));
   if (argc >= 4) {
      nsymb = atoi(argv[1]); n = atoi(argv[2]); m = atoi(argv[3]);
   } else {
      nsymb = NSYMB; n = N; m = M;
   }
   A = genstring(n,nsymb); printf("A = %s\n", A);
   B = genstring(m,nsymb); printf("B = %s\n", B);
   i = subseqsearch(A,n,B,m);
   printf("i = %d\n", i);
   if (i > 0) {
      Bi = genBi(B,m,i);
      prnsubseq(Bi,m,i,A,n);
      free(Bi);
   }
   free(A); free(B);
   exit(0);
}