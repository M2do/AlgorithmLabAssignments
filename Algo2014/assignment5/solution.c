#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define N 100

typedef struct {
   char rollno[10];
   float cgpa;
} studrec;

#define NDEPT 18
const char DEPT[NDEPT][3] =
   { "AG", "AR", "BT", "CE", "CH", "CS", "CY", "EC", "EE",
     "GG", "HS", "IE", "MA", "ME", "MI", "MT", "NA", "PH" };

void initDB ( studrec *A, int n )
{
   int i, t;

   for (i=0; i<n; ++i) {
       t = rand() % 15;
       if (t < 10) sprintf(A[i].rollno, "0%d", t);
       else sprintf(A[i].rollno, "%d", t);
       t = rand() % NDEPT;
       sprintf(A[i].rollno+2, "%s", DEPT[t]);
       A[i].rollno[4] = '1' + rand() % 9;
       A[i].rollno[5] = '0' + rand() % 10;
       A[i].rollno[6] = '0' + rand() % 10;
       A[i].rollno[7] = '0' + rand() % 10;
       A[i].rollno[8] = '0' + rand() % 10;
       A[i].rollno[9] = '\0';
       A[i].cgpa = (int)(500 + 500 * (double)rand() / (double)RAND_MAX);
       A[i].cgpa /= 100;
   }
}

void prnArr ( studrec *A, int n, char *prompt )
{
   int i;

   printf("------------------\n");
   printf("%s\n", prompt);
   printf("------------------\n");
   for (i=0; i<n; ++i) printf("%s,%.2f\n", A[i].rollno, A[i].cgpa);
   printf("------------------\n");
}

/* Counting sort with respect to the d-th digit in the roll numbers */
void digitSort ( studrec *A, studrec *B, int n, int d )
{
   int C[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
   int i, j;

   for (i=0; i<n; ++i) ++C[A[i].rollno[d] - '0'];
   for (i=9; i>0; --i) C[i] = C[i-1]; C[0] = 0;
   for (i=2; i<10; ++i) C[i] += C[i-1];
   for (i=0; i<n; ++i) {
      j = A[i].rollno[d] - '0';
      B[C[j]] = A[i];
      C[j]++;
   }
}

/* Binary search to get the department index */
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

/* Counting sort with respect to the department field */
void deptSort ( studrec *A, studrec *B, int n )
{
   int C[NDEPT] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
   int i, j;
   char depname[3];

   depname[2] = '\0';
   for (i=0; i<n; ++i) {
      depname[0] = A[i].rollno[2];
      depname[1] = A[i].rollno[3];
      j = getidx(depname);
      if (j < 0) {
         fprintf(stderr, "Invalid department code. Quiting...\n");
         return;
      }
      C[j]++;
   }
   for (i=NDEPT-1; i>0; --i) C[i] = C[i-1]; C[0] = 0;
   for (i=2; i<NDEPT; ++i) C[i] += C[i-1];
   for (i=0; i<n; ++i) {
      depname[0] = A[i].rollno[2];
      depname[1] = A[i].rollno[3];
      j = getidx(depname);
      B[C[j]] = A[i];
      C[j]++;
   }
}

int main ( int argc, char *argv[] )
{
   int n;
   studrec *A = NULL, *B = NULL;

   srand((unsigned int)time(NULL));
   if (argc == 1) n = N; else n = atoi(argv[1]);
   A = (studrec *)malloc(n * sizeof(studrec));
   B = (studrec *)malloc(n * sizeof(studrec));

   initDB(A,n); prnArr(A,n,"Initial DB");
   digitSort(A,B,n,8); prnArr(B,n,"After digit-8 sort");
   digitSort(B,A,n,7); prnArr(A,n,"After digit-7 sort");
   digitSort(A,B,n,6); prnArr(B,n,"After digit-6 sort");
   digitSort(B,A,n,5); prnArr(A,n,"After digit-5 sort");
   digitSort(A,B,n,4); prnArr(B,n,"After digit-4 sort");
   deptSort(B,A,n); prnArr(A,n,"After dept sort");
   digitSort(A,B,n,1); prnArr(B,n,"After digit-1 sort");
   digitSort(B,A,n,0); prnArr(A,n,"After digit-0 sort");

   exit(0);
}