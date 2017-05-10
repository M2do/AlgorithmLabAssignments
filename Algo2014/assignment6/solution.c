#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

typedef struct {
   char rollno[10];
   float cgpa;
} studrec;

typedef struct {
   unsigned int s;  /* Allocation size of data */
   unsigned int n;  /* Number of elements stored */
   unsigned int d;  /* Number of deleted cells */
   studrec *data;   /* Pointer to the hash array */
} hashtable;

#define NDEPT 23
const char DEPT[NDEPT][3] =
   { "AE", "AG", "AR", "BT", "CE", "CH", "CS", "CY", "EC", "EE", "EX", "GG",
     "HS", "IE", "IM", "MA", "ME", "MF", "MI", "MT", "NA", "PH", "QD" };

#define EMPTY (float)(-1)
#define DELETED (float)(-2)

/* Function to compute the smallest (odd) prime >= n */
unsigned int nextprime ( unsigned int n )
{
   unsigned int d, s;

   if (n <= 2) return 3;
   if (!(n & 1U)) n |= 1U; /* Make n odd */
   while (1) {
      s = sqrt(n + 0.1);
      for (d = 3; d <= s; d += 2)
         if (!(n % d)) break; /* if a non-trivial factor is located */
      if (d > s) break;       /* no non-trivial factor found, so n is prime */
      n += 2;                 /* check the next odd number */
   }
   return n;
}

/* Function to return a random student record */
studrec randRecord ( )
{
   int t;
   studrec R;

   t = rand() % 15;
   if (t < 10) sprintf(R.rollno, "0%d", t);
   else sprintf(R.rollno, "%d", t);     /* Random year in the range 00-14 */
   t = rand() % NDEPT;
   sprintf(R.rollno+2, "%s", DEPT[t]);  /* Random department */
   R.rollno[4] = '1' + rand() % 9;      /* Non-zero */
   R.rollno[5] = '0' + rand() % 10;     /* Random digit */
   R.rollno[6] = '0' + rand() % 10;     /* Random digit */
   R.rollno[7] = '0' + rand() % 10;     /* Random digit */
   R.rollno[8] = '0' + rand() % 10;     /* Random digit */
   R.rollno[9] = '\0';
   R.cgpa = (int)(500 + 500 * (double)rand() / (double)RAND_MAX);
   R.cgpa /= 100;                       /* Random CGPA in the range 5-10 */

   return R;
}

/* Function to print the entire hash table */
void prnHashTable ( hashtable H )
{
   unsigned int i;

   printf("    s = %d, n = %d, d = %d\n", H.s, H.n, H.d);
   for (i = 0; i < H.s; ++i) {
      printf("\tEntry %3d: ", i);
      if (H.data[i].cgpa == EMPTY) printf("EMPTY");
      else if (H.data[i].cgpa == DELETED) printf("DELETED");
      else printf("%s, %4.2f", H.data[i].rollno, H.data[i].cgpa);
      printf("\n");
   }
   printf("\n");
}

/* Function to create a new hash table of allocation size s */
hashtable newHashTable ( unsigned int s )
{
   hashtable H;
   unsigned int i;

   H.s = s;
   H.n = H.d = 0;
   H.data = (studrec *)malloc(s * sizeof(studrec));
   for (i = 0; i < s; ++i) {
      strcpy(H.data[i].rollno,"         ");
      H.data[i].cgpa = EMPTY;  /* Initially all cells are empty */
   }
   return H;
}

/* Function to return index of a department (binary search) */
int getidx ( char *depname )
{
   int L, R, M;

   L = 0; R = NDEPT - 1;
   while (L < R) {
      M = (L + R) / 2;
      if (strcmp(depname, DEPT[M]) <= 0) R = M; else L = M + 1;
   }
   if (strcmp(depname, DEPT[L])) return -1;
   return L;
}

/* Function to return the hash of R in the k-th iteration for a hash table
   of allocation size s */
int hash ( char *R, int s, int k )
{
   unsigned int h = 0U, i;
   char D[3];

   /* First compute the contributions of the digits in R */
   h = (R[0] - '0') * 10 + (R[1] - '0');
   for (i = 4; i <= 8; ++i) {
      h *= 10;
      h += R[i] - '0';
   }

   /* Then add the contribution of the department */
   D[0] = R[2]; D[1] = R[3]; D[2] = '\0';
   h += getidx(D);

   /* Finally add the contribution of quadratic probing */
   h += k * k;

   return h % s;
}

/* Function to return the CGPS of roll number R (EMPTY is returned if H
   does not contain R) */
float search ( hashtable H, char *R, int prnflag )
{
   unsigned int i, k;

   if (prnflag) printf("+++ Search(%s)\n", R);
   for (k = 0; k < (H.s + 1) / 2; ++k) {
      i = hash(R, H.s, k);
      if (H.data[i].cgpa == EMPTY) return EMPTY;                /* Search fails */
      if (H.data[i].cgpa == DELETED) continue;                  /* Search continues */
      if (!strcmp(H.data[i].rollno, R)) return H.data[i].cgpa;  /* Search succeeds */
   }
   return EMPTY;
}

/* Prototype of the rehash function to be used by insert() and delete() */
hashtable rehash ( hashtable, unsigned int );

/* Function to insert the student record R in the hash table H */
hashtable insert ( hashtable H, studrec R, int prnFlag )
{
   unsigned int i, k;

   if (prnFlag) printf("+++ Insert (%s, %4.2f)\n", R.rollno, R.cgpa);

   /* If R already exists in H, do nothing */
   if (search(H,R.rollno,0) != EMPTY) return H;

   for (k = 0; k < (H.s + 1) / 2; ++k) {
      i = hash(R.rollno, H.s, k); /* The k-th attempt */
      if ((H.data[i].cgpa == EMPTY) || (H.data[i].cgpa == DELETED)) {
         /* A suitable insertion location i is found */
         if (H.data[i].cgpa == DELETED) --(H.d); /* Insertion in a deleted location */
         H.data[i] = R;
         ++(H.n);
         break;
      }
   }

   if (k == (H.s + 1) / 2) {
      fprintf(stderr, "+++ Error: Unable to insert\n");
      return H;
   }

   /* Check load factor */
   if ((float)(H.n) / (float)(H.s) < 0.5) return H;

   /* If the load factor exceeds 0.5, rehashing is to be done */
   return rehash(H, nextprime((H.n << 1) | 1U));
}

/* Function to delete roll nnumber R from hash table H */
hashtable delete ( hashtable H, char *R )
{
   unsigned int i, k;

   printf("+++ Delete %s\n", R);
   for (k = 0; k < (H.s + 1) / 2; ++k) {
      i = hash(R, H.s, k);                      /* The k-th attempt */
      if (H.data[i].cgpa == EMPTY) return H;    /* Search fails, nothing else to do */
      if (H.data[i].cgpa == DELETED) continue;  /* Search continues */
      if (!strcmp(H.data[i].rollno, R)) {       /* Search succeeds */
         H.data[i].cgpa = DELETED;              /* Store the DELETED marker */
         strcpy(H.data[i].rollno, "         ");
         --(H.n);
         ++(H.d);
         break;
      }
   }

   if (k == (H.s + 1) / 2) return H;            /* Search failed in all attempts */

   /* Check the value of delta = d / n */
   if ((float)(H.d) / (float)(H.s) < 0.2) return H;

   /* If delta exceed 0.2, then rehashing is to be done */
   return rehash(H, H.s);
}

/* Create and return a new hash table T after rehashing the old hash table H */
hashtable rehash ( hashtable H, unsigned int s )
{
   hashtable T;
   unsigned int i;

   printf("+++ Rehashing... Please wait...\n");
   T = newHashTable(s);
   for (i = 0; i < H.s; ++i) {
      if ((H.data[i].cgpa != EMPTY) && (H.data[i].cgpa != DELETED))
         T = insert(T,H.data[i],0);
   }
   free(H.data);
   return T;
}

int main ( int argc, char *argv[] )
{
   hashtable H;
   unsigned int n, r;
   float c;
   studrec s;

   srand((unsigned int)time(NULL));
   n = (argc >= 2) ? atoi(argv[1]) : 10;

   printf("+++ Initializing...\n");
   H = newHashTable(nextprime(n << 1)); prnHashTable(H);
   while (n) {
      H = insert(H,randRecord(),1);
      --n;
   }
   prnHashTable(H);

   while (1) {
      r = rand() % 101;
      if (!r) break;
      if (r <= 30) {
         n = rand() % H.s;
         while ((n < H.s) && ((H.data[n].cgpa == EMPTY) || (H.data[n].cgpa == DELETED))) ++n;
         if (n == H.s) {
            s = randRecord();
            c = search(H,s.rollno,1);
         } else {
            c = search(H,H.data[n].rollno,1);
         }
         if (c == EMPTY) printf("Roll number %s does not exist\n",
                                (n == H.s) ? s.rollno : H.data[n].rollno);
         else printf("The CGPA of %s is %4.2f\n", H.data[n].rollno, c);
         printf("\n");
      } else if (r <= 70) {
         H = insert(H,randRecord(),1);
         prnHashTable(H);
      } else {
         n = rand() % H.s;
         while ((n < H.s) && ((H.data[n].cgpa == EMPTY) || (H.data[n].cgpa == DELETED))) ++n;
         if (n == H.s) continue;
         H = delete(H,H.data[n].rollno);
         prnHashTable(H);
      }
   }

   exit(0);
}