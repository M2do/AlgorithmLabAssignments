#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_SIZE 64
#define N 10
#define FAILURE 0
#define SUCCESS 1

/* A node in the union-find tree */
typedef struct _node {
   int rank;
   int pathnum;
   int isendpt;
   struct _node *P;
} node;

/* Pointer to a node in the union-find tree */
typedef node *nodeptr;

/* Two-dimensional array of pointers to nodes in the union-find tree */
typedef nodeptr board[MAX_SIZE][MAX_SIZE];

void initboard ( board B, int n )
{
   int i, j;

   /* Create n^2 singleton trees, one for each square in the board */
   for (i=0; i<n; ++i) for (j=0; j<n; ++j) {
      B[i][j] = (node *)malloc(sizeof(node));  /* Allocate memory */
      B[i][j] -> rank = 0;                     /* Rank of a singleton tree is 0 */
      B[i][j] -> pathnum = 0;                  /* Path number will be assigned later */
      B[i][j] -> isendpt = 0;                  /* Whether an endpoint of a path or not to be decided later */
      B[i][j] -> P = NULL;                     /* Parent is NULL */
   }
}

/* Function to find the set (pointer to the root) to which the (i,j)-the square belongs */
node *findset ( board B, int i, int j )
{
   node *r;

   r = B[i][j];
   while (r -> P) r = r -> P;
   return r;
}

/* Function to compute the union of the two sets with root pointers r1 and r2 */
void setunion ( node *r1, node *r2 )
{
   if (r1 == r2) return;
   if (r1 -> rank > r2 -> rank) r2 -> P = r1;
   else if (r1 -> rank < r2 -> rank) r1 -> P = r2;
   else { r2 -> P = r1; ++(r1 -> rank); }
}

/* This is a helper function to locate one random empty neighbor (k,l) of
   (i,j). If r is NULL, any empty neighbor will do. If not, an additional
   constraint needed during path extension is enforced. If a neighbor is
   found, the set to which (k,l) belongs is returned. Otherwise, the NULL
   pointer is returned. */
node *findemptynbr ( board B, int n, int i, int j, int *k, int *l, node *r )
{
   int u, v, i1, j1, cnt;
   node *r1, *r2;

   u = rand() % 4;          /* Start the search from a random direction */
   for (v=0; v<4; ++v) {    /* Search all the four neighbors */
      if (++u == 4) u = 0;  /* Set next direction */
      i1 = i; j1 = j;
      switch (u) {
         case 0: if (i == 0) continue; i1 = i - 1; break;       /* North */
         case 1: if (j == n-1) continue; j1 = j + 1; break;     /* East */
         case 2: if (j == 0) continue; j1 = j - 1; break;       /* West */
         case 3: if (i == n-1) continue; i1 = i + 1; break;     /* South */
      }
      r1 = findset(B,i1,j1);
      if (r1 -> rank == 0) {  /* A neighbor not belonging to any path is found */
         cnt = 0;
         if (r) {             /* Check additional constraint during path extension */
            if (i1 != 0) { r2 = findset(B,i1-1,j1); if (r2 == r) ++cnt; }
            if (i1 != n-1) { r2 = findset(B,i1+1,j1); if (r2 == r) ++cnt; }
            if (j1 != 0) { r2 = findset(B,i1,j1-1); if (r2 == r) ++cnt; }
            if (j1 != n-1) { r2 = findset(B,i1,j1+1); if (r2 == r) ++cnt; }
         }
         if (cnt <= 1) {
            *k = i1; *l = j1;
            return r1;
         }
      }
   }

   /* No suitable neighbor is found */
   *k = *l = 0;
   return NULL;
}

/* Attempt to add a random path to the board. Return SUCCESS or FAILURE. */
int addpath ( board B, int n )
{
   int i, j, k, l, s, t;
   node *r, *r1;

   /* Try to locate nodes (i, j) and (k, l) such that they are neighbors, and
      do not currently belong to any path added so far. */
   s = rand() % (n * n);     /* Start searching from a random square on the board */
   for (t=0; t<n*n; ++t) {   /* Search the entire board in a row-major wrap-around fashion */
      i = s / n; j = s % n;  /* Convert s to (i,j) */
      r = findset(B,i,j);
      if (r -> rank == 0) {  /* if (i,j) is an empty cell */
         r1 = findemptynbr(B,n,i,j,&k,&l,NULL);
         if (r1) break;      /* An empty neighbor (k,l) of (i,j) is found */
      }
      if (++s == n*n) s = 0;
   }

   /* If the entire board is searched, and no possibility of adding a new path is found. */
   if (t == n*n) return FAILURE;

   setunion(r,r1);
   printf("    New path: (%d,%d) (%d,%d)", i, j, k, l);
   B[i][j] -> isendpt = 1;

   while (1) {
      i = k; j = l; r = findset(B,i,j);

      /* Try to locate an empty neighbor (k,l) of (i,j) such that (i,j) is the
         only neighbor of (k,l) on the partially constructed path */
      r1 = findemptynbr(B,n,i,j,&k,&l,r);

      if (r1 == NULL) { /* The current path cannot be extended further */
         printf("\n");
         B[i][j] -> isendpt = 1;
         return SUCCESS;
      }

      printf(" (%d,%d)", k, l); /* extend the current path by adding (k,l) */
      setunion(r,r1);
   }
}

void addpathnum ( board B, int n )
{
   int i, j, pathnum = 0;
   node *r;

   for (i=0; i<n; ++i) for (j=0; j<n; ++j) {
      r = findset(B,i,j);
      if (r -> rank > 0) {
         if (r -> pathnum > 0) B[i][j] -> pathnum = r -> pathnum;
         else r -> pathnum = B[i][j] -> pathnum = ++pathnum;
      }
   }
}

void printboard ( board B, int n )
{
   int i, j;

   printf("\n+++ The puzzle\n");
   for (i=0; i<n; ++i) {
      printf("    +"); for (j=0; j<n; ++j) printf("----+"); printf("\n");
      printf("    |");
      for (j=0; j<n; ++j) {
         if (B[i][j] -> pathnum == 0) printf("XXXX|");
         else if (B[i][j] -> isendpt) printf("%3d |", B[i][j] -> pathnum);
         else printf("    |");
      }
      printf("\n");
   }
   printf("    +"); for (j=0; j<n; ++j) printf("----+"); printf("\n");

   printf("\n+++ The solution\n");
   for (i=0; i<n; ++i) {
      printf("    +"); for (j=0; j<n; ++j) printf("----+"); printf("\n");
      printf("    |");
      for (j=0; j<n; ++j) {
         if (B[i][j] -> pathnum == 0) printf("XXXX|");
         else printf("%3d |", B[i][j] -> pathnum);
      }
      printf("\n");
   }
   printf("    +"); for (j=0; j<n; ++j) printf("----+"); printf("\n");
}

int main ( int argc, char *argv[] )
{
   int n;
   board B;

   n = (argc > 1) ? atoi(argv[1]) : N;

   srand((unsigned int)time(NULL));

   printf("\n+++ Initializing board...\n");
   initboard(B,n);

   printf("\n+++ Attempting to add new paths...\n");
   while (addpath(B,n) == SUCCESS) {}

   printf("\n+++ Assigning path numbers...\n");
   addpathnum(B,n);

   printboard(B,n);

   exit(0);
}