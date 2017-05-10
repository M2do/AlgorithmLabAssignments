#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N_DFT 10
#define MAX_WT 99

typedef struct {
   int wt;
   char type;
   int idx;
} hnode;

typedef hnode *heap;

void heapify ( heap H, int n, int i )
{
   int l, r, min;
   hnode t;

   while (1) {
      l = 2*i; r = 2*i+1;
      if (l > n) return;
      min = ((l == n) || (H[l].wt <= H[r].wt)) ? l : r;
      if (H[min].wt >= H[i].wt) return;
      t = H[i]; H[i] = H[min]; H[min] = t;
      i = min;
   }
}

void makeheap ( heap H, int n )
{
   int i;

   for (i=n/2; i>0; --i) heapify(H,n,i);
}

void insert ( heap H, int n, int wt, int k )
{
   int i, p;
   hnode h;

   h.wt = wt;
   h.type = 'G';
   h.idx = k;
   H[(i = n+1)] = h;
   while (1) {
      if (i == 1) return;
      p = i / 2;
      if (H[p].wt <= H[i].wt) return;
      H[i] = H[p]; H[p] = h;
      i = p;
   }
}

void delete ( heap H, int n )
{
   H[1] = H[n];
   heapify(H,n-1,1);
}

int *getweights ( int n )
{
   int *A, i;

   printf("n = %d\n", n);
   A = (int *)malloc(n * sizeof(int));
   for (i=0; i<n; ++i) {
      A[i] = 1 + rand() % MAX_WT;
      printf("%d ", A[i]);
   }
   printf("\n");
   return A;
}

void mineffort ( int *w, int n, int option )
{
   heap H;
   int i, k;
   hnode h1, h2;
   int G, E, size;

   H = (hnode *)malloc((n + 1) * sizeof(hnode));
   for (i=0; i<n; ++i) {
      H[i+1].wt = w[i];
      H[i+1].type = 'w';
      H[i+1].idx = i+1;
   }
   makeheap(H,n);
   size = n;
   G = E = 0;
   printf("\n+++ Part %d\n", option);
   if (option == 1) {
      for (k=0; k<n; ++k) {
         h1 = H[1];
         delete(H,size);
         --size;
         G += h1.wt;
         printf("    Floor(%2d): Adding w[%2d] = %3d to bag, G[%2d] = %4d\n",
                     k, h1.idx, h1.wt, k, G);
         E += G;
      }
   } else {
      for (k=0; k<n; ++k) {
         h1 = H[1]; delete(H,size); --size;
         h2 = H[1];
         if ((k == 0) || (k == 1) || (k == n-1) || (G <= h2.wt)) {
            G += h1.wt;
            printf("    Floor(%2d): Adding %c[%2d] = %3d              to bag, G[%2d] = %4d\n",
                        k, h1.type, h1.idx, h1.wt, k, G);
         } else {
            printf("    Floor(%2d): Depositing G[%2d] = %4d\n", k, k-1, G);
            insert(H,size,G,k-1); ++size;
            G = h1.wt + h2.wt;
            delete(H,size); --size;
            printf("             : Adding %c[%2d] = %3d, %c[%2d] = %3d to bag, G[%2d] = %4d\n",
                        h1.type, h1.idx, h1.wt, h2.type, h2.idx, h2.wt, k, G);
         }
         E += G;
      }
   }

   printf("--- Total effort = %d\n", E);
   free(H);
}

int main ( int argc, char *argv[] )
{
   int n, *w;

   srand((unsigned int)time(NULL));

   if (argc >= 2) n = atoi(argv[1]); else n = N_DFT;
   w = getweights(n);
   mineffort(w,n,1);
   mineffort(w,n,2);

   free(w);
   exit(0);
}