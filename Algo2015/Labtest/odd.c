#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* We use zero-based storage of the heap. The children of the node at index i
   are at indices 3i+1, 3i+2 and 3i+3. */
void heapify ( int *H, int n, int i )
{
   int L, M, R, m, t;

   while (1) {
      L = 3*i+1; M = 3*i+2; R = 3*i+3;
      if (L >= n) return;
      m = L;
      if ((M < n) && (H[M] < H[m])) m = M;
      if ((R < n) && (H[R] < H[m])) m = R;
      if (H[i] < H[m]) return;
      t = H[i]; H[i] = H[m]; H[m] = t;
      i = m;
   }
}

void initheap ( int *H, int n )
{
   int i;

   for (i=(n-1)/3; i>=0; --i) heapify(H,n,i);
}

void insert ( int *H, int n, int x )
{
   int i, p;

   H[n] = x;
   i = n;
   while (1) {
      if (i == 0) return;
      p = (i-1) / 3;
      if (H[p] > H[i]) {
         H[i] = H[p]; H[p] = x;
      }
      i = p;
   }
}

int deletemin ( int *H, int n )
{
   int t;

   t = H[0]; H[0] = H[n-1];
   heapify(H,n-1,0);
   return t;
}

void printheap ( int *H, int n, const char *msg )
{
   int i;

   printf("--- %s", msg);
   for (i=0; i<n; ++i) {
      if (i % 16 == 0) printf("\n    ");
      printf("%3d ", H[i]);
   }
   printf("\n");
}

void ksmallest ( int *H, int n, int k )
{
   int i, *G;

   if ((k <= 0) || (k > n)) {
      fprintf(stderr, "*** Error in kthsmallest(): invalid number of elements\n");
      return;
   }
   G = (int *)malloc(k*sizeof(int));
   printf("    ");
   for (i=0; i<k; ++i) printf("%3d ", (G[i] = deletemin(H,n--)));
   printf("\n");
   for (i=0; i<k; ++i) insert(H,n++,G[i]);
   free(G);
}

void printsmall( int *H, int n, int i, int a )
{
   if ((i >= n) || (H[i] > a)) return;
   printf("%3d ", H[i]);
   printsmall(H,n,3*i+1,a);
   printsmall(H,n,3*i+2,a);
   printsmall(H,n,3*i+3,a);
}

int main ( int argc, char *argv[] )
{
   int n, k, a, i, *H;

   srand((unsigned int)time(NULL));
   if (argc >= 4) {
      n = atoi(argv[1]); k = atoi(argv[2]); a = atoi(argv[3]);
   } else scanf("%d%d%d", &n, &k, &a);

   H = (int *)malloc(n * sizeof(int));

   printf("+++ Reading elements...");
   for (i=0; i<n; ++i) {
      if (i % 16 == 0) printf("\n    ");
      H[i] = 1 + rand() % 100;
      printf("%3d ", H[i]);
   }

   initheap(H,n); printf("\n\n");
   printheap(H,n,"Heap after buildheap():");

   printf("\n+++ Finding the %d smallest elements in heap\n", k);
   ksmallest(H,n,k);
   printheap(H,n,"Heap after ksmallest():");

   printf("\n+++ Printing elements in heap <= %d\n    ", a);
   printsmall(H,n,0,a); printf("\n");
   printheap(H,n,"Heap after printsmall():");

   free(H);

   exit(0);
}