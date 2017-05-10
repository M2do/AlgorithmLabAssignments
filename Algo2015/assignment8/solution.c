#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_SIZE 256
#define MAX_PENALTY 99
#define MAX_LOAD 99

typedef struct {                    /* Data type for a task */
   int penalty;                     /* Delay penalty for the task */
   int load;                        /* Running time of the task */
} task;

typedef struct {                    /* Data type for a task chain */
   int taskcount;                   /* Number of tasks in the chain */
   task component[MAX_SIZE];        /* Array of the components of the chain */
   int subchainstart;               /* Index of first unfinished task */
   int bestprefixend;               /* Ending index of the optimal prefix for the chain */
} chain;

typedef chain allchains[MAX_SIZE];  /* Array storing chain headers */

typedef struct {                    /* Data type for a heap node */
   int index;                       /* Index of a chain in the chain-header array */
   double ratio;                    /* Penalty / Load */
} hnode;

typedef hnode heap[1+MAX_SIZE];     /* Max-heap based on ratio */

void heapify ( heap H, int n, int i )
{
   int l, r, max;
   hnode t;

   while (1) {
      l = 2*i; r = 2*i+1;
      if (l > n) return;
      if (l == n) max = l;
      else max = (H[l].ratio >= H[r].ratio) ? l : r;
      if (H[max].ratio < H[i].ratio) return;
      t = H[i]; H[i] = H[max]; H[max] = t;
      i = max;
   }
}

void heapinit ( heap H, int n )
{
   int i;

   for (i=n/2; i>0; --i) heapify(H,n,i);
}

void heapins ( heap H, int n, hnode x )
{
   int i, p;

   H[n+1] = x;
   i = n+1;
   while (1) {
      if (i == 1) return;
      p = i / 2;
      if (H[p].ratio >= H[i].ratio) return;
      H[i] = H[p]; H[p] = x;
      i = p;
   }
}

void heapdel ( heap H, int n )
{
   H[1] = H[n];
   heapify(H,n-1,1);
}

void readtasks ( allchains C, int n )
{
   int i;

   printf("\n    n = %d\n", n);
   for (i=0; i<n; ++i) {
      if (i % 10 == 0) printf("\n    ");
      C[i].component[0].penalty = 1 + rand() % MAX_PENALTY;
      C[i].component[0].load = 1 + rand() % MAX_LOAD;
      /* Other fields are not used in this part */
      printf("%2d %2d ", C[i].component[0].penalty, C[i].component[0].load);
   }
   printf("\n");
}

void optnocons ( allchains C, heap H, int n )
{
   int i, T, OBJ;

   /* Initialize heap array */
   for (i=0; i<n; ++i) {
      H[i+1].index = i;
      H[i+1].ratio = (double)(C[i].component[0].penalty) / (double)(C[i].component[0].load);
   }

   /* Initialize heap */
   heapinit(H,n);

   /* Schedule */
   T = OBJ = 0;
   while (n > 0) {
      i = H[1].index;
      T += C[i].component[0].load;
      OBJ += T * C[i].component[0].penalty;
      printf("    Selecting Task %2d (%2d, %2d): Goodness = %lf\n", i, C[i].component[0].penalty, C[i].component[0].load, (double)(C[i].component[0].penalty)/(double)(C[i].component[0].load));
      heapdel(H,n);
      --n;
   }

   printf("\n    Minimized objective function = %d\n", OBJ);
}

void buildchains ( allchains C, int n, int m )
{
   int i, j;

   printf("\n    m = %d\n\n    Moving tasks to chains:", m);
   for (i=0; i<m; ++i) {
      if (i % 20 == 0) printf("\n   ");
      C[i].taskcount = 1;
      printf(" %2d", i);
   }

   for (; i<n; ++i) {
      if (i % 20 == 0) printf("\n   ");
      j = rand() % m;
      C[j].component[C[j].taskcount].penalty = C[i].component[0].penalty;
      C[j].component[C[j].taskcount].load = C[i].component[0].load;
      ++(C[j].taskcount);
      printf(" %2d", j);
   }

   printf("\n\n");

   /* Diagnostic message: Chain compositions */
   for (i=0; i<m; ++i) {
      printf("    Chain %2d:", i);
      for (j=0; j<C[i].taskcount; ++j) printf(" %3d", C[i].component[j].penalty);
      printf("\n             ");
      for (j=0; j<C[i].taskcount; ++j) printf(" %3d", C[i].component[j].load);
      printf("\n");
   }
}

void optchain ( allchains C, heap H, int m )
{
   int i, j, T, OBJ;
   int sump, suml, bestidx;
   double maxrat, thisrat;

   /* Initialize heap array */
   for (i=0; i<m; ++i) {
      H[i+1].index = i;
      sump = C[i].component[0].penalty;
      suml = C[i].component[0].load;
      maxrat = (double)sump / (double)suml;
      bestidx = 0;
      for (j=1; j<C[i].taskcount; ++j) {
         sump += C[i].component[j].penalty;
         suml += C[i].component[j].load;
         thisrat = (double)sump / (double)suml;
         if (thisrat > maxrat) { maxrat = thisrat; bestidx = j; }
      }
      H[i+1].ratio = maxrat;
      C[i].bestprefixend = bestidx;
      C[i].subchainstart = 0;
   }

   /* Convert array to heap */
   heapinit(H,m);

   /* Schedule */
   T = OBJ = 0;
   while (m > 0) {
      /* Find the best chain */
      i = H[1].index;

      /* Schedule all tasks in the best prefix of the best chain */
      for (j=C[i].subchainstart; j<=C[i].bestprefixend; ++j) {
         printf("    Selecting Task %2d of Chain %2d (%2d, %2d): Goodness = %lf\n", j, i, C[i].component[j].penalty, C[i].component[j].load, H[1].ratio);
         T += C[i].component[j].load;
         OBJ += T * C[i].component[j].penalty;
      }

      /* Delete the old chain */
      heapdel(H,m); --m;

      /* If the chain is fully processed */
      if (C[i].bestprefixend == C[i].taskcount - 1) continue;

      /* Recalculate the best prefix of the remaining part of the chain */
      C[i].subchainstart = j = C[i].bestprefixend + 1;
      sump = C[i].component[j].penalty;
      suml = C[i].component[j].load;
      maxrat = (double)sump / (double)suml;
      bestidx = j;
      while (++j < C[i].taskcount) {
         sump += C[i].component[j].penalty;
         suml += C[i].component[j].load;
         thisrat = (double)sump / (double)suml;
         if (thisrat > maxrat) { maxrat = thisrat; bestidx = j; }
      }
      C[i].bestprefixend = bestidx;

      heapins(H,m,(hnode){i,maxrat});
      ++m;
   }

   printf("\n    Minimized objective function = %d\n", OBJ);
}

int main ( int argc, char *argv[] )
{
   allchains C;
   heap H;
   int n, m;

   srand((unsigned int)time(NULL));

   if (argc >= 3) { n = atoi(argv[1]); m = atoi(argv[2]); }
   else scanf("%d%d", &n, &m);

   readtasks(C,n);

   printf("\n+++ Optimization without constraints\n");
   optnocons(C,H,n);

   buildchains(C,n,m);

   printf("\n+++ Optimization satisfying chain constraints\n");
   optchain(C,H,m);

   printf("\n");

   exit(0);
}