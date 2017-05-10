#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define BIAS 1000
#define RANGE 9000
#define EXP_SUBLIST_SIZE 10

typedef struct _node {
   int data;
   struct _node *next;
} node;

typedef node *sublist;

/* Randomly generate an array A of size n */
int *readArray ( int n )
{
   int *A, i;

   A = (int *)malloc(n * sizeof(int));
   for (i=0; i<n; ++i) A[i] = BIAS + rand() % RANGE;
   return A;
}

/* Print array A from beginning to end */
void printArray ( int *A, int n )
{
   int i, c;

   for (i=c=0; i<n; ++i) {
      if (c == 0) printf("    ");
      printf("%-6d", A[i]);
      if (++c == 10) { printf("\n"); c = 0; }
   }
   if (c) printf("\n");
}

/* Function to bubble sort a linked list of integers */
void sortSublist ( sublist L )
{
   node *p, *q;
   int t;

   if (L == NULL) return;

   /* First let q point to the last node in the list */
   q = L; while (q -> next) q = q -> next;

   while (q != L) {  /* so long as q is not the first node in the list */
      p = L;         /* p runs from the first node to the node just before q */
      while (1) {
         if ( p -> data > p -> next -> data ) { /* two consecutive elements are out of order */
            t = p -> data;                      /* swap */
            p -> data = p -> next -> data;
            p -> next -> data = t;
         }
         if (p -> next == q) break;             /* p points to the node just before q */
         p = p -> next;
      }
      q = p;                                    /* q moves one node back in the list */
   }
}

/* Function to subdivide the array into sublists (buckets) */
sublist *subdivide ( int *A, int n, int *b )
{
   sublist *L;
   node *p;
   int m, M, i, j, k, s;

   /* first compute the minimum m and maximum M in A */
   m = M = A[0];
   for (i=1; i<n; ++i) {
      if (A[i] < m) m = A[i];
      if (A[i] > M) M = A[i];
   }

   k = EXP_SUBLIST_SIZE;   /* expected size of each bucket (sublist) */
   *b = (n + k - 1) / k;   /* number of buckets */
   s = 1 + (M - m) / *b;   /* width of each subinterval */

   L = (sublist *)malloc((*b) * sizeof(sublist));
   for (i=0; i<*b; ++i) L[i] = NULL;  /* initially every bucket is empty */

   for (i=0; i<n; ++i) {
      j = (A[i] - m) / s;  /* A[i] will go to the j-th bucket */

      /* insert at the beginning of the j-th list */
      p = (node *)malloc(sizeof(node));
      p -> data = A[i];
      p -> next = L[j];
      L[j] = p;
   }

   return L;
}

/* print the b sublists */
void printSublists ( sublist *L, int b )
{
   int i;
   node *p;

   for (i=0; i<b; ++i) {
      printf("    Sublist(%2d): ", i);
      p = L[i];
      while (p) {
         printf("%-6d", p -> data);
         p = p -> next;
      }
      printf("\n");
   }
}

/* Concatenate the b sorted sublists in the  array A */
void writeBack ( int *A, sublist *L, int b )
{
   int i, j;
   node *p;

   for (i=j=0; i<b; ++i) {
      p = L[i];
      while (p) {
         A[j++] = p -> data;
         p = p -> next;
      }
   }
}

int main ( int argc, char *argv[] )
{
   int n, *A, b, i;
   sublist *L;

   if (argc > 1) n = atoi(argv[1]); else scanf("%d", &n);
   srand((unsigned int)time(NULL));

   printf("+++ Reading elements of A\n");
   A = readArray(n);
   printArray(A,n);

   printf("\n+++ Dividing the array into sublists\n");
   L = subdivide(A,n,&b);
   printSublists(L,b);

   printf("\n+++ Sorting the sublists\n");
   for (i=0; i<b; ++i) sortSublist(L[i]);
   printSublists(L,b);

   printf("\n +++ Writing back to A\n");
   writeBack(A,L,b);
   printArray(A,n);

   exit(0);
}