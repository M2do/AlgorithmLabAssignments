#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 100

/* Structure to store a node in the binary tree */
typedef struct _tnode {
   int val;            /* value */
   struct _tnode *L;   /* pointer to left child */
   struct _tnode *R;   /* pointer to right child */
} tnode;

/* Binary tree data type */
typedef tnode *bintree;

/* Recursively generate a binary tree from an array A with n values */
bintree gentree ( int A[], int n )
{
   int x;
   tnode *p;

   if (n == 0) return NULL;

   /* Store the first element of A[] in a new node */
   p = (tnode *)malloc(sizeof(tnode));
   p -> val = A[0]; p -> L = p -> R = NULL;

   /* Make a random choice about existence of two subtrees */
   x = rand() % 3;
   if (x == 0) {
      /* The right subtree is NULL, so all of the remaining n-1 nodes
         will go to the left subtree */
      p -> L = gentree(A+1,n-1);
   } else if (x == 1) {
      /* The left subtree is NULL, so all of the remaining n-1 nodes
         will go to the right subtree */
      p -> R = gentree(A+1,n-1);
   } else {
      /* Both subtrees exist. floor((n-1)/2) values go to the left subtree.
         ceiling((n-1)/2) values go to the right subtree */
      p -> L = gentree(A+1,(n-1)/2);
      p -> R = gentree(A+(n+1)/2,n/2);
   }
   return p;
}

/* The preorder listing of the created tree should produce the same listing
   as in the original array. The integer k remembers how many values are
   printed. If it is a multiple of 20, a new line is output. This is used
   for pretty printing. */
void preorder ( bintree T, int *k )
{
   if (T == NULL) return;
   printf("%4d", T->val);
   ++(*k);
   if (*k % 20 == 0) printf("\n");
   preorder(T->L,k);
   preorder(T->R,k);
}

/* The linear-time max-weight finder algorithm. The sum of the values
   stored on the unique path from the root to the parent of the current
   node is passed via currwt. If T is the root, currwt is zero. */
int maxweight ( bintree T, int currwt )
{
   int lwt, rwt;

   if (T == NULL) return currwt;  /* End of a path */
   currwt += T -> val;            /* Add the value stored at the current node */
   lwt = maxweight(T->L,currwt);  /* Recurse on the left subtree */
   rwt = maxweight(T->R,currwt);  /* Recurse on the right subtree */

   /* Take the larger of the weights returned by the two recursive calls */
   currwt = (lwt >= rwt) ? lwt : rwt;

   return currwt;
}

/* This function prints a unique path (the values on the path, to be more
   precise) from the root to a leaf having the maximum weight. The first two
   arguments are the same as in maxweight. The third argument maxwt passes
   the maximum leaf weight returned by maxweight. This function runs in
   expected linear time, but its worst-case running time is quadratic. */
void printpath ( bintree T, int currwt, int maxwt )
{
   int cwt;

   if (T == NULL) return;        /* End of path */
   printf("%d",T->val);          /* Print value at this node */
   currwt += T -> val;           /* Increment currwt by the node value */
   if ((T -> L == NULL) && (T -> R == NULL)) {
      /* A max-weight leaf is reached. Just print a new line. */
      printf("\n");
      return;
   }
   /* Here we deal with a non-leaf node on the path. More values will come,
      so print + as a sign of continuation. */
   printf(" + ");

   /* Recalculate the maximum weight of one of the subtrees. This will
      identify the subtree (left or right) in which maxwt is achieved.
      Recursively print the path by going to that subtree. The recalculation
      of maxweight at every node on the path gives the worst-case
      quadratic-time behavior of the printpath function. If the tree
      is reasonably balanced, then the recalculation effort runs in linear
      time, since the recursive recalculation of max weight is made on
      one subtree only. */
   cwt = maxweight(T->L,currwt);
   if (cwt == maxwt)
      printpath(T->L,currwt,maxwt);
   else
      printpath(T->R,currwt,maxwt);
}

/* As a benign gesture, let us free the memory allocated to the tree nodes */
void courtesy ( bintree T )
{
   if (T == NULL) return;
   if (T -> L) courtesy(T -> L);
   if (T -> R) courtesy(T -> R);
   free(T);
}

int main ( int argc, char *argv[] )
{
   int n, *A, i, k = 0, w;
   bintree T;

   srand((unsigned int)time(NULL));
   if (argc > 1) n = atoi(argv[1]); else n = N;
   if (n < 0) n = -n;
   A = (int *)malloc(n * sizeof(int));
   for (i=0; i<n; ++i) A[i] = 1 + rand() % 999;
   printf("+++ INPUT ARRAY\n");
   for (i=0; i<n; ++i) {
      printf("%4d", A[i]); if (i % 20 == 19) printf("\n");
   }
   if (n % 20) printf("\n");
   T = gentree(A,n);
   free(A);
   printf("+++ TREE TRAVERSAL LISTING\n");
   preorder(T,&k);
   if (n % 20) printf("\n");
   w = maxweight(T,0);
   printf("+++ MAXIMUM WEIGHT = %d\n", w);
   printf("+++ VALUES ON THE MAX-WEIGHT PATH\n");
   printpath(T,0,w);
   courtesy(T);
   exit(0);
}
