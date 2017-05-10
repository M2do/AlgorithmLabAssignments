#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NBTECH 6
#define NDUAL 3
#define NMS 2
#define NPHD 3
#define NBLOG 1
#define SKIP_PROB 0.4

typedef struct {
   char R[10];    /* Roll number */
   int A;         /* Arts marks */
   int S;         /* Science marks */
} studrec;

/* Add nb BTech, nd Dual, nm MS, np PhD and nl backlog students.
   Random addition used in place of reading from the user. */
studrec *readData ( int nb, int nd, int nm, int np, int nl )
{
   studrec *L;
   int i, r, rold;

   L = (studrec *)malloc((nl + nb + nd + nm + np) * sizeof(studrec));
   if (L) {
      rold = 0;
      for (i=0; i<nl; ++i) {
         do { r = 1 + rand() % (10 * (i+1)); } while (r <= rold);
         rold = r;
         sprintf(L[i].R, "14SY1%04d", r);
         L[i].A = 10 + rand() % 75;
         L[i].S = 20 + rand() % 80;
      }
      r = 0;
      for (i=nl; i<nl+nb; ++i) {
         ++r;
         while ((double)rand()/(double)RAND_MAX < SKIP_PROB) ++r;
         sprintf(L[i].R, "15SY1%04d", r);
         L[i].A = 10 + rand() % 75;
         L[i].S = 20 + rand() % 80;
      }
      r = 0;
      for (i=nl+nb; i<nl+nb+nd; ++i) {
         ++r;
         while ((double)rand()/(double)RAND_MAX < SKIP_PROB) ++r;
         sprintf(L[i].R, "15SY3%04d", r);
         L[i].A = 10 + rand() % 75;
         L[i].S = 20 + rand() % 80;
      }
      r = 0;
      for (i=nl+nb+nd; i<nl+nb+nd+nm; ++i) {
         ++r;
         while ((double)rand()/(double)RAND_MAX < SKIP_PROB) ++r;
         sprintf(L[i].R, "15SY71P%02d", r);
         L[i].A = 10 + rand() % 75;
         L[i].S = 20 + rand() % 80;
      }
      r = 0;
      for (i=nl+nb+nd+nm; i<nl+nb+nd+nm+np; ++i) {
         ++r;
         while ((double)rand()/(double)RAND_MAX < SKIP_PROB) ++r;
         sprintf(L[i].R, "15SY91R%02d", r);
         L[i].A = 10 + rand() % 75;
         L[i].S = 20 + rand() % 80;
      }
   }
   return L;
}

/* Print the original list (sorted by roll number) */
void printList1 ( studrec *L, int n )
{
   int i;

   for (i=0; i<n; ++i) printf("    %s,%3d,%3d\n", L[i].R, L[i].A, L[i].S);
}

/* Print according to the index array J[]. Not part of assignment. For
   diagnostic purposes only. */
void printList2 ( studrec *L, int *J, int n )
{
   int i;

   for (i=0; i<n; ++i) printf("    %s,%3d,%3d\n", L[J[i]].R, L[J[i]].A, L[J[i]].S);
}

/* Compare two arts marks */
int cmpA ( studrec *L, int i, int j )
{
   if (L[i].A > L[j].A) return 1;
   if (L[i].A < L[j].A) return -1;
   return 0;
}

/* Compare two science marks */
int cmpS ( studrec *L, int i, int j )
{
   if (L[i].S > L[j].S) return 1;
   if (L[i].S < L[j].S) return -1;
   return 0;
}

/* Compare an arts mark with an immediate operand (x) */
int cmpImmA ( studrec *L, int i, int x )
{
   if (L[i].A > x) return 1;
   if (L[i].A < x) return -1;
   return 0;
}

/* Compare a science mark with an immediate operand (x) */
int cmpImmS ( studrec *L, int i, int x )
{
   if (L[i].S > x) return 1;
   if (L[i].S < x) return -1;
   return 0;
}

/* Bubble sort parameterized by the comparison routine cmp().
   C reference: See function pointers. */
int *sort ( studrec *L, int n, int (*cmp)(studrec *, int, int) )
{
   int *J, i, j, k;

   J = (int *)malloc(n * sizeof(int));
   if (J) {
      for (i=0; i<n; ++i) J[i] = i;
      for (j=n-2; j>=0; --j) {
         for (i=0; i<=j; ++i) {
            if (cmp(L,J[i],J[i+1]) < 0) {
               k = J[i]; J[i] = J[i+1]; J[i+1] = k;
            }
         }
      }
   }
   return J;
}

/* Binary search, indexed by J[], parameterized by cmp() */
int binsearch ( studrec *L, int *J, int n, int x, int (*cmp)(studrec *,int,int) )
{
   int l, u, m;


   l = 0; u = n-1;
   while (l != u) {
      m = (l + u) / 2;
      if (cmp(L,J[m],x) > 0) l = m+1; else u = m;
   }
   return l;
}

/* Part 1: Search for a and b (the boundaries), determine the start and end
   indices, and print all records between them (both inclusive). Indexed by
   J[], parameterized by cmp(). */
void intervalSearch ( studrec *L, int *J, int n, int a, int b, int (*cmp)(studrec *,int,int) )
{
   int start, end, i;

   printf("--- List of students:\n");

   if (cmp(L,J[0],a) < 0) return;
   if (cmp(L,J[n-1],b) > 0) return;

   start = binsearch(L,J,n,b,cmp);
   while ((start > 0) && (cmp(L,J[start],b) == 0)) --start;

   end = binsearch(L,J,n,a,cmp);
   while ((end < n-1) && (cmp(L,J[end],a) == 0)) ++end;

   if (cmp(L,J[start],b) > 0) ++start;
   if (cmp(L,J[end],a) < 0) --end;

   for (i=start; i<=end; ++i) {
      printf("    %s,%3d,%3d\n", L[J[i]].R, L[J[i]].A, L[J[i]].S);
   }
}

/* Solves Part 2. First generate the rank arrays RA[] and RS[] for the two
   courses. Then make a run over all the students. */
void rankCompare ( studrec *L, int *JA, int *JS, int n )
{
   int i, rank, *RA, *RS, nBA, nBS, nSC;

   RA = (int *)malloc(n * sizeof(int));
   RS = (int *)malloc(n * sizeof(int));
   if (RA && RS) {
      rank = 0;
      for (i=0; i<n; ++i) {
         if ((i == 0) || (L[JA[i]].A != L[JA[i-1]].A)) rank = i + 1;
         RA[JA[i]] = rank;
      }

      nBA = nBS = nSC = rank = 0;
      for (i=0; i<n; ++i) {
         if ((i == 0) || (L[JS[i]].S != L[JS[i-1]].S)) rank = i + 1;
         RS[JS[i]] = rank;
      }

      for (i=0; i<n; ++i) {
         printf("    %s, %2d, %2d, Arts rank = %2d, Science rank = %2d, ",
                 L[i].R, L[i].A, L[i].S, RA[i], RS[i]);
         if (RA[i] < RS[i]) { printf("Decision: BA\n"); ++nBA; }
         else if (RA[i] > RS[i]) { printf("Decision: BS\n"); ++nBS; }
         else { printf("Decision: SC\n"); ++nSC; }
      }
   }

   printf("\n");
   printf("    Number of BA = %d\n", nBA);
   printf("    Number of BS = %d\n", nBS);
   printf("    Number of SC = %d\n", nSC);

   free(RA); free(RS);
}

int main ( int argc, char *argv[] )
{
   int nb, nd, nm, np, nl, n;
   studrec *L;
   int *JA, *JS;
   int a, b;

   srand((unsigned int)time(NULL));

   if (argc >= 6) {
      nb = atoi(argv[1]);
      nd = atoi(argv[2]);
      nm = atoi(argv[3]);
      np = atoi(argv[4]);
      nl = atoi(argv[5]);
   } else {
      nb = NBTECH; nd = NDUAL; nm = NMS; np = NPHD; nl = NBLOG;
   }
   n = nb + nd + nm + np + nl;

   L = readData(nb,nd,nm,np,nl); if (L == NULL) exit(1);
   printf("\n+++ Initial list (sorted by roll numbers)\n"); printList1(L,n);

   JA = sort(L,n,cmpA); if (JA == NULL) exit(2);
   printf("\n+++ List sorted by arts marks\n"); printList2(L,JA,n);

   JS = sort(L,n,cmpS); if (JS == NULL) exit(2);
   printf("\n+++ List sorted by science marks\n"); printList2(L,JS,n);

   printf("\n+++ Interval search\n");

   a = 10 + rand() % 60; b = a + 10 + rand() % 10;
   printf("\n+++ Arts: a = %d, b = %d\n", a, b);
   intervalSearch(L,JA,n,a,b,cmpImmA);
   a = 10 + rand() % 60; b = a + 1;
   printf("\n+++ Arts: a = %d, b = %d\n", a, b);
   intervalSearch(L,JA,n,a,b,cmpImmA);

   a = 20 + rand() % 60; b = a + 5;
   printf("\n+++ Science: a = %d, b = %d\n", a, b);
   intervalSearch(L,JS,n,a,b,cmpImmS);
   a = 20 + rand() % 60; b = a + 10 + rand() % 10;
   printf("\n+++ Science: a = %d, b = %d\n", a, b);
   intervalSearch(L,JS,n,a,b,cmpImmS);

   printf("\n+++ Rank comparisons\n");
   rankCompare(L,JA,JS,n);

   free(L); free(JA); free(JS);

   exit(0);
}