#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define UNDEFINED -1

typedef struct _node {
   int u, v;
   struct _node *L, *M, *R;
} node;
typedef node *TST;

int search ( TST T, int x )
{
   if (x <= 0) return 0;

   while (T) {
      if ((x == T -> u) || (x == T -> v)) return 1;
      if (x < T -> u) T = T -> L;
      else if (x < T -> v) T = T -> M;
      else T = T -> R;
   }

   return 0;
}

TST insert ( TST T, int x )
{
   TST p, q;

   printf("%3d ", x);

   if (x <= 0) return T;

   q = T; p = NULL;
   while (q) {
      if ((q -> u == x) || (q -> v == x)) return T;
      p = q;
      if (x < q -> u) q = q -> L;
      else if (x < q -> v) q = q -> M;
      else q = q -> R;
   }

   if ((p) && (p -> u == UNDEFINED)) {
      if (x > p -> v) { p -> u = p -> v; p -> v = x; }
      else p -> u = x;
      return T;
   }

   if ((p) && (p -> v == UNDEFINED)) {
      if (x < p -> u) { p -> v = p -> u; p -> u = x; }
      else p -> v = x;
      return T;
   }

   q = (node *)malloc(sizeof(node));
   q -> u = x; q -> v = UNDEFINED;
   q -> L = q -> M = q -> R = NULL;

   if (p == NULL) return q;

   if (x < p -> u) p -> L = q;
   else if (x < p -> v) p -> M = q;
   else p -> R = q;

   return T;
}

void print2largest ( TST T, int *status )
{
   if (T == NULL) return;
   print2largest(T->R, status);
   if (*status == 2) return;
   if (T -> v != UNDEFINED) {
      ++(*status);
      if (*status == 2) {
         printf("+++ The second largest element in tree is %d\n", T -> v);
         return;
      }
      printf("+++ The largest element in tree is %d\n", T -> v);
   }
   print2largest(T->M, status);
   if (*status == 2) return;
   if (T -> u != UNDEFINED) {
      ++(*status);
      if (*status == 2) {
         printf("+++ The second largest element in tree is %d\n", T -> u);
         return;
      }
      printf("+++ The largest element in tree is %d\n", T -> u);
   }
   print2largest(T->L, status);
}

void printsmall ( TST T, int a )
{
   if (T == NULL) return;
   if (T -> u == UNDEFINED) {
      if (T -> v <= a) printf("%3d ", T->v);
      return;
   }
   if (T -> v == UNDEFINED) {
      if (T -> u <= a) printf("%3d ", T->u);
      return;
   }
   printsmall(T->L,a);
   if (T -> u > a) return;
   printf("%3d ", T->u);
   if (T -> u < a) printsmall(T->M,a);
   if (T -> v > a) return;
   printf("%3d ", T->v);
   if (T -> v < a) printsmall(T->R,a);
}

int main ( int argc, char *argv[] )
{
   int nins, a, status, i, x;
   TST T = NULL;

   srand((unsigned int)time(NULL));
   if (argc >= 3) {
      nins = atoi(argv[1]); a = atoi(argv[2]);
   } else scanf("%d%d", &nins, &a);

   printf("\n+++ Inserting elements in tree");
   for (i=0; i<nins; ++i) {
      if (i%16 == 0) printf("\n    ");
      T = insert(T,1+rand()%100);
   }

   printf("\n\n+++ x = "); scanf("%d", &x);
   printf("    Search(%d): %s\n", x, (search(T,x) ? "SUCCESS" : "FAILURE"));
   printf("+++ y = "); scanf("%d", &x);
   printf("    Search(%d): %s\n", x, (search(T,x) ? "SUCCESS" : "FAILURE"));

   printf("\n"); status = 0;
   print2largest(T,&status);

   printf("\n+++ Values in tree <= %d are\n    ", a);
   printsmall(T,a);
   printf("\n");

   exit(0);
}