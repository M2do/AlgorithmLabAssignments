#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STR_SIZE 21
#define MAX_NO_ASSOC 10

typedef struct _htnode {
   char att[STR_SIZE];
   int nassoc;
   struct _htnode *assoc[MAX_NO_ASSOC];
   struct _htnode *next;
} htnode;

typedef struct {
   int S;
   htnode **A, **B;
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
   T.A = (htnode **)malloc(S * sizeof(htnode *));
   T.B = (htnode **)malloc(S * sizeof(htnode *));
   for (i=0; i<S; ++i) T.A[i] = T.B[i] = NULL;
   return T;
}

htnode *findidx ( htnode **C, char *z, int h )
{
   htnode *p;

   p = C[h];
   while (p) {
      if (!strcmp(p->att,z)) break;
      p = p -> next;
   }
   return p;
}

void searchpair ( hashtable T, pair data )
{
   htnode *p, *q;
   int i, j, u, v;

   i = H(data.x,T.S); p = findidx(T.A,data.x,i);
   j = H(data.y,T.S); q = findidx(T.B,data.y,j);
   if ((p) && (q)) {
      for (u = 0; u < p->nassoc; ++u) if (p->assoc[u] == q) break;
      for (v = 0; v < q->nassoc; ++v) if (q->assoc[v] == p) break;
   }
   if ((p == NULL) || (q == NULL) || (u == p->nassoc) || (v == q->nassoc)) {
      printf("    (%s,%s) not found\n", data.x, data.y);
   } else {
      printf("    (%s,%s) found at (%d,%d)\n", data.x, data.y, i, j);
   }
}

void searchattr ( hashtable T, char *key, int which )
{
   int i, j;
   htnode **A, *p;

   if (which == 1) {
      A = T.A;
      printf("    (%s,-) is associated with:", key);
   } else {
      A = T.B;
      printf("    (-,%s) is associated with:", key);
   }
   i = H(key,T.S);
   p = findidx(A, key, i);
   if (p) {
      for (j=0; j<p->nassoc; ++j) printf(" %s", p->assoc[j]->att);
   }
   printf("\n");
}

htnode *findinsidx ( htnode **C, char *key, int i )
{
   htnode *p;

   p = C[i];
   while (p) {
      if (!strcmp(p->att,key)) break;
      p = p -> next;
   }
   if (p == NULL) {
      p = (htnode *)malloc(sizeof(htnode));
      strcpy(p->att, key);
      p->nassoc = 0;
      p->next = C[i];
      C[i] = p;
   }
   return p;
}

hashtable insertpair ( hashtable T, pair data )
{
   htnode *p, *q;
   int i, j, k, l;

   i = H(data.x,T.S); p = findinsidx(T.A,data.x,i);
   j = H(data.y,T.S); q = findinsidx(T.B,data.y,j);

   for (k=0; k<p->nassoc; ++k) if (p->assoc[k] == q) break;
   for (l=0; l<q->nassoc; ++l) if (q->assoc[l] == p) break;
   if ((k < p->nassoc) && (l < q->nassoc)) return T;

   p->assoc[p->nassoc++] = q;
   q->assoc[q->nassoc++] = p;

   printf("    Insertion at (%d,%d)\n", i, j);

   return T;
}

htnode *finddelidx ( htnode **C, char *key, int i )
{
   htnode *p;

   p = C[i];
   while (p) {
      if (!strcmp(p->att,key)) break;
      p = p -> next;
   }
   return p;
}

void delatnode ( htnode **C, int i, htnode *p, int k )
{
   htnode *q;

   p->nassoc--;
   while (k < p->nassoc) {
      p -> assoc[k] = p -> assoc[k+1];
      ++k;
   }
   if (p -> nassoc == 0) {
      if (p == C[i]) {
         C[i] = p -> next;
      } else {
         q = C[i];
         while (q -> next != p) q = q -> next;
         q -> next = p -> next;
      }
      free(p);
   }
}

hashtable deletepair ( hashtable T, pair data )
{
   htnode *p, *q;
   int i, j, k, l;

   i = H(data.x,T.S); p = findinsidx(T.A,data.x,i); if (p == NULL) return T;
   j = H(data.y,T.S); q = findinsidx(T.B,data.y,j); if (q == NULL) return T;

   for (k=0; k<p->nassoc; ++k) if (p->assoc[k] == q) break;
   for (l=0; l<q->nassoc; ++l) if (q->assoc[l] == p) break;
   if ((k == p->nassoc) && (l == q->nassoc)) return T;

   delatnode(T.A,i,p,k);
   delatnode(T.B,j,q,l);

   printf("    Deletion at (%d,%d)\n", i, j);

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