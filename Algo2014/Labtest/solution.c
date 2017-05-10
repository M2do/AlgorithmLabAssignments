#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ALPH_SIZE 3

char *genrandstr ( int n )
{
   int i;
   char *S;

   S = (char *)malloc((n+1) * sizeof(char));
   for (i=0; i<n; ++i) S[i] = 'a' + rand() % ALPH_SIZE;
   S[n] = '\0';
   return S;
}

/* Start the next string in the decomposition at every repeated occurrence
   of any symbol in consecutive positions */
void oddPart1 ( char *S, int n )
{
   int i = 0, k = 0;
   char last = 'a' - 1;

   for (i=0; i<n; ++i) {
      if (S[i] == last) { printf(" "); ++k; }
      printf("%c", S[i]); last = S[i];
   }
   ++k;
   printf("\nNumber of substrings = %d\n", k);
}

/* Delete from S all duplicate entries of symbols from consecutive runs */
void evenPart1 ( char *S, int n )
{
   int i = 0;
   char last = 'a' - 1;

   for (i=0; i<n; ++i) {
      if (S[i] != last) { printf("%c", S[i]); last = S[i]; }
   }
   printf("\n");
}

/* This function is based on the computation of a two-dimensional table F.
   F[l][i] stores the Boolean value whether the substring S[i...i+l-1] of
   length l and starting at index i is a palindrome. For l = 1, every
   F[1][i] is true. For l = 2, F[2][i] is true if and only if S[i] is equal
   to S[i+1]. A substring S[u...v] of length >= 3 is palindromic if and
   only if S[u] is equal to S[v] and the substring S[u+1,v-1] of length
   l - 2 is a palindrome. */
void oddPart2 ( char *S, int n )
{
   int *F, *G, *H, *Fbest;
   int i, l, maxlen;

   /* We do not maintain the entire F table which takes Theta(n^2) space.
      Computation of a row requires the second previous row, so we need
      to store only two previous rows. */
   F = (int *)malloc(n * sizeof(int)); /* The current row */
   G = (int *)malloc(n * sizeof(int)); /* The previous row */
   H = (int *)malloc(n * sizeof(int)); /* The second previous row */

   /* Fbest stores the row F[l] with largest l such that F[l][i] is true
      for some i. This is needed for printing all the maximum-length
      palindromic substrings. */
   Fbest = (int *)malloc(n * sizeof(int));

   maxlen = 1; /* All length-one substrings are palindromic */

   for (i=0; i<=n-1; ++i) Fbest[i] = H[i] = 1; /* Store F[1] in H */

   for (i=0; i<=n-2; ++i) {                    /* Store F[2] in G */
      if (S[i] == S[i+1]) {
         G[i] = 1; maxlen = 2;
      } else {
         G[i] = 0;
      }
   }

   if (maxlen == 2) /* if some length-two substring is palindromic */
      for (i=0; i<=n-2; ++i) Fbest[i] = G[i];

   /* Iteratively generate the rows F[3], F[4], ..., F[l] */
   for (l=3; l<=n; ++l) {
      /* Use H to generate the current row in F */
      for (i=0; i<=n-l; ++i) {
         if ((S[i] == S[i+l-1]) && (H[i+1])) {
            F[i] = 1; maxlen = l;
         } else {
            F[i] = 0;
         }
      }

      if (maxlen == l) /* if some length-l substring is palindromic */
         for (i=0; i<=n-l; ++i) Fbest[i] = F[i];

      /* Prepare for the next iteration */
      for (i=0; i<=n-l+1; ++i) H[i] = G[i];
      for (i=0; i<=n-l; ++i) G[i] = F[i];
   }

   printf("Maximum length of a palindromic substring is %d\n", maxlen);
   printf("All maximum-length palindromic substrings are ");
   for (l=0; l<=n-maxlen; ++l) {
      if (Fbest[l]) { /* If F[maxlen][l] is a palindrome */
         for (i=0; i<maxlen; ++i) printf("%c", S[l+i]);
         printf(" ");
      }
   }
   printf("\n");
}

/* Recursively print an optimal decomposition */
void prndecomposition ( char *S, int *bestidx, int k )
{
   int i;

   /* S[bestidx[k]...k] is palindromic. We first recursively print the
      best decomposition of S[0...bestidx[k]-1]. If bestidx[k] is 0,
      then recursion stops. */
   if (bestidx[k]) prndecomposition(S, bestidx, bestidx[k] - 1);

   /* Then print S[bestidx[k]...k] */
   for (i=bestidx[k]; i<=k; ++i) printf("%c", S[i]);

   printf(" ");
}

/* This function is based on the computation of a two-dimensional table F.
   F[l][i] stores the Boolean value whether the substring S[i...i+l-1] of
   length l and starting at index i is a palindrome. For l = 1, every
   F[1][i] is true. For l = 2, F[2][i] is true if and only if S[i] is equal
   to S[i+1]. A substring S[u...v] of length >= 3 is palindromic if and
   only if S[u] is equal to S[v] and the substring S[u+1,v-1] of length
   l - 2 is a palindrome. */
void evenPart2 ( char *S, int n )
{
   int **F, *bestidx, *bestcnt, i, l, k, t;

   /* Allocate memory for the two-dimensional table F */
   F = (int **)malloc((n+1) * sizeof(int *));
   for (l=1; l<=n; ++l) F[l] = (int *)malloc((n-l+1) * sizeof(int));

   for (i=0; i<=n-1; ++i) F[1][i] = 1;                        /* Populate F[1] */
   for (i=0; i<=n-2; ++i) F[2][i] = (S[i] == S[i+1]) ? 1 : 0; /* Populate F[2] */
   for (l=3; l<=n; ++l) {                         /* Generate F[l] from F[l-2] */
      for (i=0; i<=n-l; ++i) {
         if ((S[i] == S[i+l-1]) && (F[l-2][i+1])) F[l][i] = 1;
         else F[l][i] = 0;
      }
   }

   /* Construct a best decomposition using the F table. bestcnt[k] stores the
      minimum count of substrings in the decomposition of S[0...k]. bestidx[k]
      stores the index of the last substring in an optimal decomposition. */
   bestidx = (int *)malloc(n * sizeof(int));
   bestcnt = (int *)malloc(n * sizeof(int));
   for (l=1; l<=n; ++l) {
      if (F[l][0]) { /* If the entire substring S[0...l-1] is a palindrome */
         /* The optimal decomposition consists of a single string */
         bestidx[l-1] = 0; bestcnt[l-1] = 1; continue;
      }
      bestcnt[l-1] = n; /* Initialize bestcnt[l-1] */
      for (k=1; k<=l-1; ++k) {
         /* First we need to check whether S[k...l-1] is a palindrome.
            Then we optimally decompose S[0...k-1]. The case k = 0 is
            handled earlier separately. */
         if ((F[l-k][k]) && ( (t = 1 + bestcnt[k-1]) < bestcnt[l-1])) {
            /* if a better decompsition is found, remember it */
            bestcnt[l-1] = t;
            bestidx[l-1] = k;
         }
      }
   }

   prndecomposition(S,bestidx,n-1);
   printf("\nNumber of substrings = %d\n", bestcnt[n-1]);

   for (l=1; l<=n; ++l) free(F[l]);
   free(F);
   free(bestidx);
   free(bestcnt);
}

int main ( int argc, char *argv[] )
{
   int n;
   char *S;

   n = (argc > 1) ? atoi(argv[1]) : 100;
   srand((unsigned int)time(NULL));
   S = genrandstr(n); printf("S = %s\n", S);
   printf("\n+++ Odd PC, Part I\n"); oddPart1(S,n);
   printf("\n+++ Even PC, Part I\n"); evenPart1(S,n);
   printf("\n+++ Odd PC, Part II\n"); oddPart2(S,n);
   printf("\n+++ Even PC, Part II\n"); evenPart2(S,n);
   free(S);
   exit(0);
}