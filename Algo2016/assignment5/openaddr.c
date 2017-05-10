#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STR_SIZE 21
#define MAX_NO_ASSOC 10

typedef struct {
   char att[STR_SIZE];
   int nassoc;
   int assoc[MAX_NO_ASSOC];
} htnode;

typedef struct {
   int S;
   htnode *A, *B;
} hashtable;

typedef struct {
   char x[STR_SIZE];
   char y[STR_SIZE];
} pair;

int H ( char *z, int S )
{
   int i, l, sum;

   l = strlen(z); sum = 0;
   for (i=0; i<l; ++i) {
      sum <<= 7;
      sum += (int)z[i];
      sum %= S;
   }
   return sum;
}

hashtable inittable ( int S )
{
   hashtable T;
   int i;

   T.S = S;
   T.A = (htnode *)malloc(S * sizeof(htnode));
   T.B = (htnode *)malloc(S * sizeof(htnode));
   for (i=0; i<S; ++i) {
      strcpy(T.A[i].att,"EMPTY");
      strcpy(T.B[i].att,"EMPTY");
      T.A[i].nassoc = T.B[i].nassoc = 0;
   }
   return T;
}

int findidx ( htnode *C, char *z, int S, int h )
{
   int k;

   for (k=0; k<S; ++k) {
      if (!strcmp(C[h].att,z)) return h;
      if (!strcmp(C[h].att,"EMPTY")) return -1;
      if (++h == S) h = 0;
   }
   return -1;
}

void searchpair ( hashtable T, pair data )
{
   int i, j, ai, aj;

   i = findidx(T.A,data.x,T.S,H(data.x,T.S));
   j = findidx(T.B,data.y,T.S,H(data.y,T.S));
   if ((i >= 0) && (j >= 0)) {
      for (ai=0; ai<T.A[i].nassoc; ++ai)
         if (T.A[i].assoc[ai] == j) break;
      for (aj=0; aj<T.B[j].nassoc; ++aj)
         if (T.B[j].assoc[aj] == i) break;
   }
   if ((i < 0) || (j < 0) || (ai == T.A[i].nassoc) || (aj == T.B[j].nassoc)) {
      printf("    (%s,%s) not found\n", data.x, data.y);
   } else {
      printf("    (%s,%s) found at (%d,%d)\n", data.x, data.y, i, j);
   }
}

void searchattr ( hashtable T, char *key, int which )
{
   int i, j;
   htnode *A, *B;

   if (which == 1) {
      A = T.A; B = T.B;
      printf("    (%s,-) is associated with:", key);
   } else {
      A = T.B; B = T.A;
      printf("    (-,%s) is associated with:", key);
   }
   i = findidx(A, key, T.S, H(key,T.S));
   if (i >= 0) {
      for (j=0; j<A[i].nassoc; ++j) printf(" %s", B[A[i].assoc[j]].att);
   }
   printf("\n");
}

int findinsidx ( htnode *C, char *z, int S, int i )
{
   int j;

   j = findidx(C,z,S,i); if (j >= 0) return j;
   while (1) {
      if (!strcmp(C[i].att,z)) return i;
      if ((!strcmp(C[i].att,"EMPTY")) || (!strcmp(C[i].att,"DELETED"))) {
         strcpy(C[i].att,z); C[i].nassoc = 0; return i;
      }
      if (++i == S) i = 0;
   }
}

hashtable insertpair ( hashtable T, pair data )
{
   int hi, hj, i, j, k;

   hi = H(data.x,T.S);
   hj = H(data.y,T.S);
   i = findinsidx(T.A,data.x,T.S,hi);
   j = findinsidx(T.B,data.y,T.S,hj);

   for (k=0; k<T.A[i].nassoc; ++k) if (T.A[i].assoc[k] == j) return T;
   for (k=0; k<T.B[j].nassoc; ++k) if (T.B[j].assoc[k] == i) return T;

   T.A[i].assoc[T.A[i].nassoc++] = j;
   T.B[j].assoc[T.B[j].nassoc++] = i;

   printf("    hash = (%d,%d), insertion at (%d,%d)\n", hi, hj, i, j);

   return T;
}

int finddelidx ( htnode *C, char *z, int S, int i )
{
   int k;

   for (k=0; k<S; ++k) {
      if (!strcmp(C[i].att,z)) return i;
      if (++i == S) i = 0;
   }
   return -1;
}

hashtable deletepair ( hashtable T, pair data )
{
   int hi, hj, i, j, u, v;

   hi = H(data.x,T.S);
   hj = H(data.y,T.S);
   i = finddelidx(T.A,data.x,T.S,hi); if (i < 0) return T;
   j = finddelidx(T.B,data.y,T.S,hj); if (j < 0) return T;

   for (u=0; u<T.A[i].nassoc; ++u) if (T.A[i].assoc[u] == j) break;
   if (u == T.A[i].nassoc) return T;
   for (v=0; v<T.B[j].nassoc; ++v) if (T.B[j].assoc[v] == i) break;
   if (v == T.B[j].nassoc) return T;

   T.A[i].nassoc--;
   if (T.A[i].nassoc == 0) {
      strcpy(T.A[i].att,"DELETED");
   } else {
      while (u < T.A[i].nassoc) {
         T.A[i].assoc[u] = T.A[i].assoc[u+1];
         ++u;
      }
   }

   T.B[j].nassoc--;
   if (T.B[j].nassoc == 0) {
      strcpy(T.B[j].att,"DELETED");
   } else {
      while (v < T.B[j].nassoc) {
         T.B[j].assoc[v] = T.B[j].assoc[v+1];
         ++v;
      }
   }

   printf("    hash = (%d,%d), deletion at (%d,%d)\n", hi, hj, i, j);

   return T;
}

int main ()
{
   int S;
   hashtable T;
   int resp;
   pair data;

   printf("S = "); scanf("%d", &S); printf("%d\n", S);
   T = inittable(S);

   while (1) {
      printf("+++ Enter choice: "); scanf("%d", &resp);
      switch (resp) {
         case 0: printf("0\n"); break;
         case 1: scanf("%s", data.x); printf("1 %s\n", data.x);
                 searchattr(T,data.x,1); break;
         case 2: scanf("%s", data.y); printf("2 %s\n", data.y);
                 searchattr(T,data.y,2); break;
         case 3: scanf("%s%s", data.x, data.y);
                 printf("3 %s %s\n", data.x, data.y);
                 searchpair(T,data); break;
         case 4: scanf("%s%s", data.x, data.y);
                 printf("4 %s %s\n", data.x, data.y);
                 T = insertpair(T,data); break;
         case 5: scanf("%s%s", data.x, data.y);
                 printf("5 %s %s\n", data.x, data.y);
                 T = deletepair(T,data); break;
         default: printf("%d\n", resp); printf("Invalid choice...\n");
      }
      if (resp == 0) break;
   }

   exit(0);
}