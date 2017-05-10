/* Implementation of an interval tree data structure. This data structure is
   suited to return a single interval, if any, that overlaps with the query
   interval. The classroom-allocation problem of the assignment is perhaps
   not suitable to the solved by this data structure. We need separate type(s)
   of interval trees for that problem. Moreover, a sweep algorithm can solve
   the problem in O(n log n) time. */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_START_TIME 20
#define MAX_DURATION 10
#define VERBOSE 1

/* Data type interval */
typedef struct {
   int starttime;
   int finishtime;
} interval;

typedef struct _node {
   interval I;         /* interval stored in the node */
   int count;          /* how many times the interval occurs */
   int max;            /* the maximum finish time in the subtree rooted at the node */
   struct _node *L;    /* left child pointer */
   struct _node *R;    /* right child pointer */
} node;

typedef node *evtree;

/* Lexicographic comparison of intervals I and J */
int intervalcmp ( interval I, interval J )
{
   if (I.starttime < J.starttime) return -1;
   if (I.starttime > J.starttime) return 1;
   if (I.finishtime < J.finishtime) return -1;
   if (I.finishtime > J.finishtime) return 1;
   return 0;
}

/* Insert an interval I in the tree T */
evtree insert ( evtree T, interval I )
{
   node *p, *q;
   int cmpres;

   printf("(%d,%d) ", I.starttime, I.finishtime);

   /* Standard BST insertion procedure */
   p = T; q = NULL;
   while (p) {
      cmpres = intervalcmp(I, p->I);
      if (cmpres == 0) {   /* if I already resides in T, */
         ++(p -> count);   /* increment its occurrence count, */
         return T;         /* and return */
      }
      /* modify (if necessary) the max field caused by insertion */
      if (I.finishtime > p -> max) p -> max = I.finishtime;
      q = p;
      if (cmpres < 0) p = p -> L; else p = p -> R;
   }

   p = (node *)malloc(sizeof(node));
   p -> I = I;
   p -> count = 1;
   p -> max = I.finishtime;
   p -> L = p -> R = NULL;

   if (T == NULL) return p;

   if (cmpres < 0) q -> L = p; else q -> R = p;
   return T;
}

int preorder ( evtree T, int nprinted )
{
   if (T == NULL) return nprinted;
   printf("[(%2d,%-2d),%d,%2d] ", (T -> I).starttime, (T -> I).finishtime, T -> count, T -> max);
   ++nprinted; if (nprinted == 5) { printf("\n               "); nprinted = 0; }
   nprinted = preorder(T -> L, nprinted);
   nprinted = preorder(T -> R, nprinted);
   return nprinted;
}

int inorder ( evtree T, int nprinted )
{
   if (T == NULL) return nprinted;
   nprinted = inorder(T -> L, nprinted);
   printf("[(%2d,%-2d),%d,%2d] ", (T -> I).starttime, (T -> I).finishtime, T -> count, T -> max);
   ++nprinted; if (nprinted == 5) { printf("\n               "); nprinted = 0; }
   nprinted = inorder(T -> R, nprinted);
   return nprinted;
}

/* Function to return the boolean value whether intervals I anf J overlap */
int overlap ( interval I, interval J )
{
   return ((I.starttime < J.finishtime) && (J.starttime < I.finishtime));
}

/* This function returns the overlap count for the query interval I with all
   the intervals stored in the tree T. This function may not run in O(h + t)
   time. Indeed, this tree appears unsuitable to output this count in
   O(h + t) time. Here is an example.

   Suppose that T is an almost complete BST with n lodes, so h = log n.
   At a depth of about (log n) / 2, there is a level l with sqrt(n) nodes.
   We choose one leaf under each such node and let the leaf store an interval
   of the form (i, 2n). All other nodes in the tree store intervals of the
   form (j, 2n-2). We adjust the i and j values so that T is a BST.

   Now, take the query interval I = (2n-1, 2n+1). Only the sqrt(n) leaves
   storing (i, 2n) overlap with I, so t = sqrt(n). The algorithm must
   visit all the nodes at level l. This will take O(sqrt(n)) = O(t) time.
   But then, the search must proceed along sqrt(n) mutually edge-disjoint
   paths from the nodes at level l to the leaves (i, 2n). Each such subpath
   is of length about (log n)/2, and there are t = sqrt(n) such paths.
   Therefore, the search must take at least Theta(ht) times.

   But then, is ht also an upper bound on the search complexity? This is not
   clear to me. Definitely, the search cannot be poorer than O(n), but can
   we say that the complexity of the algorithm is O(min(ht,n))? */
int overlapcnt ( evtree T, interval I, int verbose, int *nprinted )
{
   int cnt = 0;

   if (T == NULL) return 0;

   /* I overlaps with the interval stored at the root */
   if (overlap(T -> I, I)) {
      if (verbose) {
         printf("[(%2d,%-2d),%d] ", (T->I).starttime, (T->I).finishtime, T->count);
         ++(*nprinted);
         if (*nprinted == 5) {
            printf("\n                      ");
            *nprinted = 0;
         }
      }
      cnt = T -> count;
   }

   /* Now make two recursive calls provided that there is a possibility of
      locating an overlapping interval in the subtree. In both the subtrees,
      the max values are used for that purpose. For the right subtree, the
      start time of the interval at the root is additionally considered to
      have additional pruning. */
   if ((T -> L) && (I.starttime < T -> L -> max))
      cnt += overlapcnt(T -> L, I, verbose, nprinted);

   if ((T -> R) && (I.starttime < T -> R -> max) && (I.finishtime > (T->I).starttime))
      cnt += overlapcnt(T -> R, I, verbose, nprinted);

   return cnt;
}

/* Function to return the minimum requirement of classrooms. The function
   essentially makes a preorder traversal in T. For each interval (a, b)
   stored in T, it computes the overlap count of the unit-length interval
   (a, a+1). When an event starts, the requirement of a classroom increases.
   Moreover, no event starts at a fractional time. Therefore, it suffices
   to look at only the intervals (a, a+1) as mentioned above. The maximum
   overlap count for these intervals is the desired answer. */
void minclasscnt ( evtree T, evtree p, interval *I, int *gmax )
{
   int max;
   interval J;

   if (p == NULL) return;

   J.starttime = (p -> I).starttime; J.finishtime = J.starttime + 1;
   max = overlapcnt(T, J, !VERBOSE, NULL);

   if (max > *gmax) {
      *gmax = max;
      *I = J;
   }

   minclasscnt(T, p->L, I, gmax);
   minclasscnt(T, p->R, I, gmax);
}

int main ( int argc, char *argv[] )
{
   int i, n, s, f, q, c;
   evtree T = NULL;
   interval I;

   srand((unsigned int)time(NULL));

   if (argc >= 3) {
      n = atoi(argv[1]);
      q = atoi(argv[2]);
   } else {
      printf("n = "); scanf("%d", &n);
      printf("q = "); scanf("%d", &q);
   }

   printf("\n+++ Insert   : ");
   for (i=0; i<n; ++i) {
      s = rand() % MAX_START_TIME;
      f = s + 1 + rand() % MAX_DURATION;
      T = insert(T,(interval){s,f});
   }

   printf("\n\n");
   printf("+++ Preorder : "); preorder(T,0); printf("\n");
   printf("+++ Inorder  : "); inorder(T,0); printf("\n\n");

   for (i=0; i<q; ++i) {
      s = rand() % MAX_START_TIME;
      f = s + 1 + rand() % MAX_DURATION;
      printf("+++ Overlap (%2d,%-2d) : ", s, f); n = 0;
      c = overlapcnt(T, (interval){s,f}, VERBOSE, &n);
      if (n) printf("\n                      ");
      printf("Overlap count = %d\n", c);
   }

   c = -1; minclasscnt(T,T,&I,&c);
   printf("\n+++ Minimum class count is %d\n", c);
   printf("+++ A busiest hour is (%d,%d)\n", I.starttime, I.finishtime);
   printf("+++ Overlap (%2d,%-2d) : ", I.starttime, I.finishtime); n = 0;
   c = overlapcnt(T, I, VERBOSE, &n);
   if (n) printf("\n                      ");
   printf("Overlap count = %d\n", c);

   exit(0);
}