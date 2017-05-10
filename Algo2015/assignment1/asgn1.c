#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct _node {
   int key;
   struct _node *L, *R;
} node;

typedef node *BST;

/* Standard BST insertion procedure */
BST insert ( BST T, int x )
{
   node *p, *q;

   printf("%d ", x);

   p = T; q = NULL;
   while (p) {
      if (x == p -> key) return T;
      q = p;
      if (x < p -> key) p = p -> L; else p = p -> R;
   }

   p = (node *)malloc(sizeof(node));
   p -> key = x;
   p -> L = p -> R = NULL;

   if (q == NULL) return p;

   if (x < q -> key) q -> L = p; else q -> R = p;

   return T;
}

void preorder ( BST T )
{
   if (T) {
      printf("%d ", T -> key);
      preorder(T -> L);
      preorder(T -> R);
   }
}

void inorder ( BST T )
{
   if (T) {
      inorder(T -> L);
      printf("%d ", T -> key);
      inorder(T -> R);
   }
}

void printBST ( BST T )
{
   printf("\n+++ The BST created has the following listings\n");
   printf("    Preorder    : "); preorder(T); printf("\n");
   printf("    Inorder     : "); inorder(T); printf("\n\n");
}

/* Standard height-computation function. Needed only for fancy output. */
int height ( BST T )
{
   int lht, rht;

   if (T == NULL) return -1;
   lht = height(T->L);
   rht = height(T->R);
   return 1 + ((lht >= rht) ? lht : rht);
}

/* Recursive helper function for fancy output */
void printhelper ( BST T, int level, char *str )
{
   int i;

   /* Print prefix string received from parent */
   printf("    %s", str);

   /* Print own data (or NULL) */
   if (T == NULL) { printf("NULL\n"); return; }
   printf("%d\n", T -> key);

   /* Update the prefix string for children */
   for (i=0; i<4*level; ++i) {
      if (str[i] == 'L') str[i] = '|';
      else if (str[i] != '|') str[i] = ' ';
   }

   /* Append L--> to the prefix string for printing the left child */
   str[4*level] = 'L';
   str[4*level+1] = str[4*level+2] = '-'; str[4*level+3] = '>';
   str[4*level+4] = '\0';
   printhelper(T -> L, level+1, str);

   /* Append R--> to the prefix string for printing the right child */
   str[4*level] = 'R';
   str[4*level+1] = str[4*level+2] = '-'; str[4*level+3] = '>';
   str[4*level+4] = '\0';
   printhelper(T -> R, level+1, str);

   /* While returning to parent, reset the effect of adding L--> and R--> */
   str[4*level] = '\0';
}

/* The wwrapper function for fancy printing */
void fancyprintBST ( BST T )
{
   char *str;
   int h, i;

   printf("+++ The following fancy printing of the BST is not for submission.\n");
   printf("+++ You may implement it as a take-home programming exercise.\n");
   h = height(T);
   if (h < 0) return;
   str = (char *)malloc(4*h+1);
   for (i=0; i<=4*h; ++i) str[i] = ' ';
   str[0] = '\0';
   printhelper(T,0,str);
   free(str);
   printf("\n");
}

/* search1(x) = min {v | v is on the search path for x and v >= x} */
BST search1 ( BST T, int x )
{
   node *p, *minp;
   int min;

   min = 1000000000; minp = NULL;
   p = T;
   while (p) {
      if (x == p -> key) return p; /* can be omitted to save one comparison */
      if (x < p -> key) {          /* but then, < should be changed to <= */
         if (p -> key < min) {
            min = p -> key; minp = p;
         }
         p = p -> L;
      } else p = p -> R;
   }
   return minp;
}

/* search2(y) = max {v | v is on the search path for y and v <= y} */
BST search2 ( BST T, int y )
{
   node *p, *maxp;
   int max;

   max = -1000000000; maxp = NULL;
   p = T;
   while (p) {
      if (y == p -> key) return p;
      if (y < p -> key) p = p -> L;
      else {
         if (p -> key > max) {
            max = p -> key; maxp = p;
         }
         p = p -> R;
      }
   }
   return maxp;
}

/* lca(p,q) is the unique node r satisfying key(p) <= key(r) <= key(q).
   It is allowed to have p == q. In this case, r = p = q.
   The function can be easily rewritten without recursion. */
node *lca ( node *T, node *p, node *q )
{
   if ((p == NULL) || (q == NULL)) return NULL; /* Check for error condition */
   if (p -> key > q -> key) return lca(T,q,p);  /* Ensure that key(p) <= key(q) */
   if ((p -> key <= T -> key) && (T -> key <= q -> key)) return T;
   if (q -> key < T -> key) return lca(T -> L, p, q);
   return lca(T -> R, p, q);
}

/* Print only the relevant values in the left subtree of lca(p,q) */
void restrictedinorder1 ( BST T, int x )
{
   if (T == NULL) return;

   /* If key(T) <= x, then the left subtree of T cannot contain keys
      >= x. In this case, there is no need to make a recursive call on
      the left subtree. If key(T) >  x, the left subtree may contain
      relevant keys. */
   if (x < T -> key) restrictedinorder1(T->L,x);

   /* Print the node key if relevant */
   if (x <= T -> key) printf("%d ", T -> key);

   /* Right subtree cannot be (totally) skipped */
   restrictedinorder1(T->R,x);
}

/* Print only the relevant values in the right subtree of lca(p,q) */
void restrictedinorder2 ( BST T, int y )
{
   if (T == NULL) return;
   restrictedinorder2(T->L,y);
   if (y >= T -> key) printf("%d ", T -> key);
   if (y > T -> key) restrictedinorder2(T->R,y);
}

/* This function recalculates p, q and r (already computed in main()). This
   could be avoided if p, q and r are additionally passed as parameters.
   However, as a stand-alone function with the minimal input requirements,
   this is fine. */
void printrange ( BST T, int x, int y )
{
   int t;
   node *p, *q, *r;

   if (x > y) { t = x; x = y; y = t; } /* Deal with error condition */
   printf("+++ Values in T between %d and %d are: ", x, y);
   p = search1(T,x); q = search2(T,y);
   if ((p) && (q) && (p -> key <= q -> key)) {
      r = lca(T,p,q);

      restrictedinorder1(r->L,x); /* print relevant keys in left subtree */
      printf("%d ", r -> key);    /* print key at lca */
      restrictedinorder2(r->R,y); /* print relevant keys in right subtree */

      /* Note: The two restricted inorder traversals along with the printing
         of key(r) can be merged to a single restricted inorder function. */
   }
   printf("\n");
}

int main ( int argc, char *argv[] )
{
   BST T = NULL;
   node *p, *q, *r;
   int n, x, y, i;

   srand((unsigned int)time(NULL));
   if (argc >= 4) {
      n = atoi(argv[1]); x = atoi(argv[2]); y = atoi(argv[3]);
   } else {
      printf("n = "); scanf("%d", &n);
      printf("x = "); scanf("%d", &x);
      printf("y = "); scanf("%d", &y);
   }

   printf("+++ Insert      : ");
   for (i=0; i<n; ++i) T = insert(T,rand()%100);
   printf("\n");

   printBST(T);
   fancyprintBST(T);

   printf("+++ search1(%2d) : ",x);
   p = search1(T,x);
   if (p == NULL) printf("NULL\n"); else printf("%d\n", p -> key);

   printf("+++ search2(%2d) : ",y);
   q = search2(T,y);
   if (q == NULL) printf("NULL\n\n"); else printf("%d\n\n", q -> key);

   printf("+++ lca(%2d,%2d)  : ", (p) ? p -> key : -1, (q) ? q -> key : -1);
   r = lca(T,p,q);
   if (r == NULL) printf("NULL\n\n"); else printf("%d\n\n", r -> key);

   printrange(T,x,y);

   exit(0);
}