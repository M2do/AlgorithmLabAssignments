#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NINS 1000
#define NDEL 1000
#define MAXVAL 999
#define UNDEF -1

typedef struct _node {
   int val;                  /* Value stored at the node */
   int cnt;                  /* Count of the nodes in the subtree rooted at this node */
   struct _node *L, *R, *P;  /* Left, right and parent pointers */
} tnode;

void prttree ( tnode *, int );            /* Print tree */
tnode *tryins ( tnode *, int, tnode ** ); /* Plain BST insert attempt */
void inccnt ( tnode * );                  /* Increment node counts after insertion */
tnode *insert ( tnode *, int, int );      /* The total insertion routine */
tnode *trydel ( tnode *, int, tnode ** ); /* Plain BST delete attempt */
void deccnt ( tnode * );                  /* Decrement node counts after deletion */
tnode *delete ( tnode *, int, int );      /* The total deletion routine */
void balance ( tnode *, int );            /* Repair imbalance in node counts */

/* Preorder printing of a tree T. The parameter level indicates the level
   of the node. This is for pretty printing. */
void prttree ( tnode *T, int level )
{
   int i;

   if (T == NULL) return;
   for (i=0; i<level; ++i) printf("    ");
   printf("%d (%d) -> %d (%d), %d (%d)\n",
      T -> val, T -> cnt,
      (T -> L) ? T -> L -> val : UNDEF,
      (T -> L) ? T -> L -> cnt : 0,
      (T -> R) ? T -> R -> val : UNDEF,
      (T -> R) ? T -> R -> cnt : 0
   );
   prttree(T->L,level+1);
   prttree(T->R,level+1);
}

/* Try to insert the value x in the tree T. Return the modified T. Also
   return to *q a pointer to the parent of the inserted node. If insertion
   fails (because x was already present in T), then *q gets the value NULL. */
tnode *tryins ( tnode *T, int x, tnode **q )
{
   tnode *p;

   *q = NULL; p = T;
   while (p) {
      if (x == p -> val) {
         *q = NULL;
         return T;
      }
      *q = p;
      if (x < p -> val) p = p -> L; else p = p -> R;
   }
   p = (tnode *)malloc(sizeof(tnode));
   p -> val = x; p -> cnt = 1;
   p -> L = p -> R = NULL;
   p -> P = *q;
   if (*q == NULL) return p;
   if (x < (*q) -> val) (*q) -> L = p; else (*q) -> R = p;
   return T;
}

/* Increment count at every node on the insertion path */
void inccnt ( tnode *p )
{
   while (p) {
      ++(p -> cnt);
      p = p -> P;
   }
}

/* The outermost wrapper routine for insertion. This also does balancing.
   The third argument passes the recursion level for an easy understanding
   of the recursive calls. */
tnode *insert ( tnode *T, int x, int reclevel )
{
   tnode *q;
   int i;

   for (i=0; i<reclevel; ++i) printf("    ");
   printf("+++ Insertion of %d under ", x);
   if (T == NULL) printf("NULL"); else printf("%d", T -> val);
   printf(" at recursion level %d\n", reclevel);

   T = tryins(T,x,&q);     /* Try to insert */
   if (q) {                /* If needed */
      inccnt(q);           /* Increment the counts on the insretion path */
      balance(q,reclevel); /* Restore balance */
   }

   if ((q) || (T -> cnt == 1)) {
      for (i=0; i<reclevel; ++i) printf("    ");
      printf("------------------------------------------------------------\n");
      prttree(T,reclevel);
      for (i=0; i<reclevel; ++i) printf("    ");
      printf("------------------------------------------------------------\n");
   } else printf("    Value already present in tree\n");

   return T;
}

/* Try to delete the value x from the tree T. Return the modified T. Also
   return to *q a pointer to the parent of the deleted node. If deletion
   fails (because x was not present in T), then *q gets the value NULL. */
tnode *trydel ( tnode *T, int x, tnode **q )
{
   tnode *p, *r;

   p = T; *q = NULL;
   while (p) {
      if (x == p -> val) break;
      *q = p;
      if (x < p -> val) p = p -> L; else p = p -> R;
   }
   if (p == NULL) { *q = NULL; return T; } /* x not present in T */
   if ((p -> L == NULL) || (p -> R == NULL)) {
      r = (p -> L == NULL) ? p -> R : p -> L;
      if (r) r -> P = *q;
      if (*q == NULL) T = r; /* Deletion of root */
      else if (x < (*q) -> val) (*q) -> L = r;
      else (*q) -> R = r;
   } else {
      r = p; p = p -> R;
      while (p -> L) p = p -> L;
      r -> val = p -> val;
      *q = p -> P;
      if (p -> val < (*q) -> val) (*q) -> L = p -> R;
      else (*q) -> R = p -> R;
      if (p -> R) p -> R -> P = *q;
   }
   free(p);
   return T;
}

/* Decrement count at every node on the deletion path */
void deccnt ( tnode *p )
{
   while (p) {
      --(p -> cnt);
      p = p -> P;
   }
}

/* The outermost wrapper routine for deletion. This also does balancing. */
tnode *delete ( tnode *T, int x, int reclevel )
{
   tnode *q;
   int i;

   for (i=0; i<reclevel; ++i) printf("    ");
   printf("+++ Deletion of %d under ", x);
   if (T == NULL) printf("NULL"); else printf("%d", T -> val);
   printf(" at recursion level %d\n", reclevel);

   T = trydel(T,x,&q);      /* Make a deletion attempt */
   if (q) {                 /* If needed */
      deccnt(q);            /* Decrement the counts on the deletion path */
      balance(q,reclevel);  /* Restore balance */
   }

   if ( ((q) && (T)) || (T -> cnt == 1)) {
      for (i=0; i<reclevel; ++i) printf("    ");
      printf("------------------------------------------------------------\n");
      prttree(T,reclevel);
      for (i=0; i<reclevel; ++i) printf("    ");
      printf("------------------------------------------------------------\n");
   } else printf("    Value not present in tree\n");

   return T;
}

/* This is the balancing fumction called after every insert/delete for
   restoring the PB-BST condition. The repair attempt starts from the parent
   of an inserted/deleted node, and continues up the tree up to the root. At
   any node q on the path, the imbalance is calculated. If it is 0, 1, or -1,
   nothing is done. If the absolute value of the imbalace is larger than 2,
   then also no repair attempt is made, because this cannot happen in a
   PB-BST after a single insertion or deletion. If the imbalance is 2 or -2,
   the function makes an appropriate value movement from the heavier subtree
   to the lighter subtree via the node q. Since balance() may call insert()
   and delete() at higher levels of recursion, the recursion level is
   passed for neat printing. */
void balance ( tnode *q, int reclevel )
{
   int imbal;
   tnode *p;

   /* for all nodes on the insertion/deletion path */
   while (q) {
      if ((q -> L != NULL) || (q -> R != NULL)) {

         /* Compute imbalance */
         imbal = (q -> R) ? (q -> R -> cnt) : 0;
         imbal -= (q -> L) ? (q -> L -> cnt) : 0;

         /* Check for errors */
         if ((imbal > 2) || (imbal < -2)) {
            printf("*** Error: Invalid balance\n");
            return;
         }

         /* If imbalance is 0, 1 or -1, do nothing. Handle only the cases
            +2 and -2. */

         if (imbal == 2) { /* Right subtree is heavier than the left */

            /* Insert the value stored at q in the left subtree */
            if (q -> L) q -> L -> P = NULL;
            q -> L = insert(q -> L, q -> val, reclevel+1);
            q -> L -> P = q;

            /* Locate the immediate successor of q -> val in the right subtree */
            p = q -> R; while (p -> L) p = p -> L;

            /* Store the immediate successor at q */
            q -> val = p -> val;

            /* Delete the immediate successor from the right subtree */
            q -> R -> P = NULL;
            q -> R = delete(q -> R, p -> val, reclevel+1);
            q -> R -> P = q;

         } else if (imbal == -2) { /* Left subtree is heavier than the right */

            /* Insert the value stored at q in the right subtree */
            if (q -> R) q -> R -> P = NULL;
            q -> R = insert(q -> R, q -> val, reclevel+1);
            q -> R -> P = q;

            /* Locate the immediate predecessor of q -> val in the left subtree */
            p = q -> L; while (p -> R) p = p -> R;

            /* Store the immediate predecessor at q */
            q -> val = p -> val;

            /* Delete the immediate predecessor from the left subtree */
            q -> L -> P = NULL;
            q -> L = delete(q -> L, p -> val, reclevel+1);
            q -> L -> P = q;

         }
      }

      /* Move up by one node on the insertion/deletion path */
      q = q -> P;
   }
}

int main ( int argc, char *argv[] )
{
   int nins, ndel, maxval, i;
   tnode *T = NULL;

   if (argc >= 4) {
      nins = atoi(argv[1]);
      ndel = atoi(argv[2]);
      maxval = atoi(argv[3]);
   } else { nins = NINS; ndel = NDEL; maxval = MAXVAL; }

   srand((unsigned int)time(NULL));

   printf("\n+++ Random insertion attempts\n\n");
   for (i=0; i<nins; ++i) T = insert(T,1+rand()%maxval,0);

   printf("\n+++ Random deletion attempts\n\n");
   for (i=0; i<ndel; ++i) T = delete(T,1+rand()%maxval,0);

   exit(0);
}