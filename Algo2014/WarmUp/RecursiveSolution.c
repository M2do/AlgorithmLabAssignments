#include <stdio.h>
#include <stdlib.h>

#define N 4
#define M 7

/* Structure to represent a cell in the n x n mess */
typedef struct {
   int row, col;
} loc;

/* We order the cells lexicographically, first with respect to the row number,
   and then with respect to the column number. This function returns the
   location next to p. If p is invalid or the last cell in the grid, an
   invalid location is returned. */
loc nextloc ( int n, loc p )
{
   if ((p.row < 0) || (p.col < 0)) return (loc){-1,-1};
   if ((p.row >= n) || (p.col >= n)) return (loc){-1,-1};
   if ((p.row == n-1) && (p.col == n-1)) return (loc){-1,-1};
   if (p.col == n-1) return (loc){p.row+1,0};
   return (loc){p.row,p.col+1};
}

/* Check whether two cells p1 and p2 are adjacent (that is, share an edge).
   This function assumes that p1 and p2 are different. */
int adjacent ( loc p1, loc p2 )
{
   if (p1.row == p2.row) {
      if ((p1.col == p2.col+1) || (p2.col == p1.col+1)) return 1;
      return 0;
   }
   if (p1.col == p2.col) {
      if ((p1.row == p2.row+1) || (p2.row == p1.row+1)) return 1;
      return 0;
   }
   return 0;
}

/* A currently stores k mutually non-adjacent cells. A new location p
   is intended to be added. The addition is valid if and only if p is not
   adjacent to any of the k cells already present in A. */
int valid ( loc A[], int k, loc p )
{
   int i;

   for (i=0; i<k; ++i) if (adjacent(A[i],p)) return 0;
   return 1;
}

/* This is the recursive function for generating all valid arrangements.
   n and m are read from the user (grid dimension and number of coins).
   A[] stores a placement of k cells already selected. The parameter serno
   stores the serial number of the current arrangement to be printed. */
void arrange ( loc A[], int n, int m, int k, int *serno )
{
   int i, j;
   loc p;

   /* If k equals m, then a valid placement is already found. Print it,
      and return to the caller function. An empty cell is printed as
      ".", and an occupied cell as "x". A space is added before and after
      . and x in order to make the output look pretty. We print each row
      of the grid as a string. It takes 3n characters to store the
      information for all the n cells (including the spaces). In addition,
      we need the NULL character at the end of the string. */
   if (k == m) {
      char **B;

      B = (char **)malloc(n * sizeof(char *));
      for (i=0; i<n; ++i) {
         B[i] = (char *)malloc((3*n+1) * sizeof(char));
         for (j=0; j<n; ++j) {
            B[i][3*j] = B[i][3*j+2] = ' ';
            B[i][3*j+1] = '.';
         }
         B[i][3*n] = '\0';
      }
      for (i=0; i<m; ++i) B[A[i].row][3*A[i].col+1] = 'x';
      /* Increment the serial number for the newly located arrangement */
      ++(*serno);
      printf("Arrangement %d:\n", *serno);
      for (i=0; i<n; ++i) {
         printf("%s\n", B[i]);
         free(B[i]);
      }
      free(B);
      return;
   }

   /* Here we have placed k < m coins in the board, as stored in A[].
      We add another location p and recursively call the function on
      placements of k+1 coins. p is chosen from all possible valid
      locations. A recursive call is made for each valid p. In order
      to avoid duplicate printing of the same arrangements, the array
      A[] is always kept sorted with respect to the lexicographic
      ordering on (row,col). When a new point p is added, it must be
      lexicographically larger than all of the k points already included
      in A. */
   if (k == 0) p = (loc){0,0};     /* The first point */
   else p = nextloc(n,A[k-1]);     /* The lexicographically next point */
   while (p.row >= 0) {            /* For all possibilities of p */
      if (valid(A,k,p)) {          /* If the insertion of p in A[] is valid */
         A[k] = p;
         arrange(A,n,m,k+1,serno); /* Recursive call on k+1 chosen cells */
      }
      p = nextloc(n,p);            /* Step through the next value of p */
   }
}

int main ( int argc, char *argv[] )
{
   int n, m, serno = 0;
   loc *A;

   if (argc > 1) n = atoi(argv[1]); else n = N;
   if (argc > 2) m = atoi(argv[2]); else m = M;
   A = (loc *)malloc(m * sizeof(loc));
   arrange(A,n,m,0,&serno);
   exit(0);
}