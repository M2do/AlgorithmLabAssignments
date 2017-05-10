#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MERGE_PROB 0.1

typedef struct _node {
   int data;
   struct _node *next;
} node;

void init ( node **A, node **B, int t )
{
   int i, merged;
   node *p, *q, *r, *tp, *tq;

   p = q = tp = tq = NULL;

   merged = 0;
   if ((double)rand() / (double)RAND_MAX <= MERGE_PROB) merged = 1;

   for (i = 0; i < t; ++i) {
      /* Create a node with random data */
      r = (node *)malloc(sizeof(node));
      r -> data = rand() % 1000;
      r -> next = NULL;

      if (merged) {
         /* if the two lists are already merged, just append */
         if (p == NULL) p = r;
         if (q == NULL) q = r;
         if (tp) tp -> next = r;
         tp = r;
      } else if ((double)rand() / (double)RAND_MAX <= MERGE_PROB) {
         /* this is going to be the first common node */
         if (p == NULL) p = r;
         if (q == NULL) q = r;
         if (tp) tp -> next = r;
         if (tq) tq -> next = r;
         tp = r;
         merged = 1;
      } else if (rand() % 2 == 0) {
         /* append to the first list */
         if (p == NULL) {
            p = tp = r;
         } else {
            tp -> next = r; tp = r;
         }
      } else {
         /* append to the second list */
         if (q == NULL) {
            q = tq = r;
         } else {
            tq -> next = r; tq = r;
         }
      }
   }

   *A = p; *B = q;
}

/* Straigthforward implementation of linked-list printing */
void printlist ( node *A )
{
   while (A) {
      printf("%d ", A -> data);
      A = A -> next;
   }
   printf("\n");
}

/* Function for printing the two lists together, that is, the three
   segments of Y */
void printlists ( node *A, node *B )
{
   int m, n, i;
   node *p, *q, *r;

   /* First compute the sizes m and n of the two lists */
   m = 0; p = A; while (p) { ++m; p = p -> next; }
   n = 0; q = B; while (q) { ++n; q = q -> next; }

   /* Skip the surplus number of elements in the longer list */
   p = A; if (m > n) for (i = 0; i < m-n; ++i) p = p -> next;
   q = B; if (m < n) for (i = 0; i < n-m; ++i) q = q -> next;

   /* Now, locate the first common node. If no such node exists, the
      loop will terminate with p = q = NULL. */
   while (1) {
      if (p == q) break;
      p = p -> next; q = q -> next;
   }
   r = p;

   printf("Initial part of A : ");
   p = A;
   while (p != r) {
      printf("%d ", p -> data);
      p = p ->next;
   }
   printf("\n");

   printf("Initial part of B : ");
   q = B;
   while (q != r) {
      printf("%d ", q -> data);
      q = q ->next;
   }
   printf("\n");

   printf("Common part       : ");
   while (r != NULL) {
      printf("%d ", r -> data);
      r = r ->next;
   }
   printf("\n");
}

int main ( int argc, char *argv[] )
{
   int t;
   node *A, *B;

   srand((unsigned int)time(NULL));

   if (argc >= 2) {
      t = atoi(argv[1]);
   } else {
      printf("t = "); scanf("%d", &t);
   }

   init(&A,&B,t);
   printf("\n+++ Part(a)\n");
   printf("List A            : "); printlist(A);
   printf("List B            : "); printlist(B);

   printf("\n+++ Part(b)\n");
   printlists(A,B);

   exit(0);
}