/*** Implementation of the Pagh–Rodler Cuckoo Hashing Algorithm (2001)       ***/
/*** This version is slightly different from what is asked in the assignment ***/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INIT_T 3
#define MAX_ITER_CNT 10
#define MAX_REC_LEVEL 4
#define FAILURE 0
#define SUCCESS 1
#define PRESENT 2
#define DOUBLE_SIZE 3
#define CHANGE_PARAMS 4

typedef struct {
   unsigned int s;     /* Allocation size */
   unsigned int t;     /* log2(s) */
   unsigned int n;     /* Number of elements stored */
   unsigned int a, b;  /* Parameters for generating two hash values */
   char **data;        /* One-dimensional table of strings */
} htbl;

/* Function prototypes. Needed only for insert() and rehash() which call
   each other. */
htbl init ( );
unsigned int hash ( char *, unsigned int, unsigned int );
unsigned int search ( htbl, char *);
htbl insertatpos ( htbl, char *, unsigned int, int * );
htbl insert ( htbl, char *, int *, int );
htbl rehash ( htbl, int, int * );
htbl delete ( htbl, char *, int * );
htbl readfile ( char *, int * );

/* Return an empty table with s = 8, t = 3, n = 0, a = 1, and b = 3 */
htbl init ( )
{
   htbl T;
   unsigned int i;

   T.s = (1U << INIT_T); T.t = INIT_T; T.n = 0;
   T.a = 1; T.b= 3;
   T.data = (char **)malloc((T.s) * sizeof(char *));
   /* NULL pointer indicates empty location */
   for (i=0; i<T.s; ++i) T.data[i] = NULL;
   return T;
}

/* Return the hash of a string str governed by parameters a and t */
unsigned int hash ( char *str, unsigned int a, unsigned int t )
{
   unsigned int h = 0;

   if (!str) return h;

   /* First, convert str to a 32-bit unsigned integer */
   while (*str) {
      h = (h << 16) + (h << 8) - h + (unsigned int)(*str);
      ++str;
   }

   /* Multiplying by a gives a family of universal hash functions for integers */
   h *= a;

   /* The return value should an index in the range [0...s-1] = [0...2^t-1].
      The most significant t bits of the product are used as the return value. */
   return h >> (32 - t);
}

/* Search at the two possible indices where str may stay */
unsigned int search ( htbl T, char *str )
{
   unsigned int h, status = FAILURE;

   h = hash(str, T.a, T.t); printf("(%u,", h); fflush(stdout);
   if ((T.data[h]) && (!strcmp(T.data[h],str))) status = SUCCESS;
   h = hash(str, T.b, T.t); printf("%u): ", h); fflush(stdout);
   if ((T.data[h]) && (!strcmp(T.data[h],str))) status = SUCCESS;
   return status;
}

/* Helper function for insert(). Assumes that T.data[h] is NULL, that is,
   it is allowed to insert str at the empty index h. */
htbl insertatpos ( htbl T, char *str, unsigned int h, int *status )
{
   T.data[h] = (char *)malloc((1 + strlen(str)) * sizeof(char));  /* Allocate memory */
   strcpy(T.data[h],str);                                         /* Store */
   *status = SUCCESS; printf("insert at index %u: success\n", h);

   /* The number of elements increases by one. If the load factor exceeds half,
      rehashing is necessary in order to double the size of the data array.
      The call of rehash() sets the status (success/failure). */
   ++T.n;
   if (T.n > (T.s >> 1)) {
      printf("\n+++ High load factor (n = %u, s = %u). Rehashing necessary...\n", T.n, T.s);
      T = rehash(T,DOUBLE_SIZE,status);
   }
   return T;
}

/* Insert str in T. PRESENT/SUCCESS/FAILURE is stored in *status. The
   recursion level indicates whether further rehashing attempt will be
   made if the main loop fails. */
htbl insert ( htbl T, char *str, int *status, int reclevel )
{
   unsigned int h1, h2, i;
   char *newstr, *tmpstr;

   printf("--- Insert(%s): ", str); fflush(stdout);

   /* If str is already present in T, do nothing. */
   if (search(T,str) == SUCCESS) {
      printf("already present\n");
      *status = PRESENT;
      return T;
   }

   /* Make a copy of str to newstr */
   newstr = (char *)malloc((1 + strlen(str)) * sizeof(char));
   strcpy(newstr,str);

   /* The main insertion loop */
   for (i=0; i<MAX_ITER_CNT; ++i) {
      h1 = hash(newstr, T.a, T.t); /* First hash index */
      if (T.data[h1] == NULL) {    /* Empty position */
         T = insertatpos(T,newstr,h1,status);
         free(newstr);
         return T;
      }

      h2 = hash(newstr, T.b, T.t); /* Second hash index */
      if (T.data[h2] == NULL) {    /* Empty location */
         T = insertatpos(T,newstr,h2,status);
         free(newstr);
         return T;
      }

      /* If both the indices are occupied, forcibly kick out the string
         sitting at the first index. Insert newstr there. The dislocated
         string is now renamed as newstr for insertion in the next iteration. */
      tmpstr = T.data[h1]; T.data[h1] = newstr; newstr = tmpstr;
   }

   /* Here, newstr stores a string that could not be inserted in the main loop. */

   if (reclevel < MAX_REC_LEVEL) {  /* Carry out a reorganization */
      printf("temporary failure\n");

      T = rehash(T,CHANGE_PARAMS,status);  /* First try with new parameters (a,b) */
      if (*status == FAILURE) {            /* If that does not succeed, */
         T = rehash(T,DOUBLE_SIZE,status); /* try by doubling the size of the hash table. */
      }
      if (*status == FAILURE) {            /* If the second attempt fails too, */
         printf("permanent failure, giving up...\n"); /* abort the insertion effort. */
      } else {
         T = insert(T,newstr,status,reclevel+1);      /* unfinished business */
      }
   } else {  /* If too many insert-rehash calls are recursively made, abort. */
      printf("permanent failure, giving up...\n");
   }

   free(newstr);

   return T;
}

/* The rehash function to reorganize T. There are two types of rehashing:
   CHANGE_PARAMS and DOUBLE_SIZE. SUCCESS/FAILURE is reported via status. */
htbl rehash ( htbl T, int type, int *status )
{
   int i;
   htbl N;

   if (type == CHANGE_PARAMS) {
      /* Increment a and b by 2 each */
      N.a = T.b; N.b = N.a + 2; N.s = T.s; N.t = T.t;
      printf("\n+++ Rehashing with changed parameters (%u,%u)\n", N.a,N.b);
   } else if (type == DOUBLE_SIZE) {
      /* Double the allocation size */
      N.a = T.a; N.b = T.b; N.s = (T.s << 1); N.t = T.t + 1;
      printf("\n+++ Rehashing with doubled size %u\n", T.s);
   } else {
      printf("\n*** Unknown rehashing type\n");
      *status = FAILURE;
      return T;
   }

   N.n = 0;
   N.data = (char **)malloc((N.s)*sizeof(char *));
   for (i=0; i < N.s; ++i) N.data[i] = NULL;

   /* Copy strings from N to T */
   for (i=0; i<T.s; ++i) if (T.data[i]) {
      printf("    Entry %3d: Relocating (%s): ", i, T.data[i]);

      N = insert(N,T.data[i],status,MAX_REC_LEVEL);

      /* If any of the insertions fails, rehashing is unsuccessful. */
      if (*status == FAILURE) break;
   }

   if (i == T.s) {
      printf("    Rehashing successful...\n\n");
      for (i=0; i<T.s; ++i) if (T.data[i]) free(T.data[i]);
      free(T.data);
      return N;
   } else {
      printf("    Rehashing failed...\n\n");
      for (i=0; i<N.s; ++i) if (N.data[i]) free(N.data[i]);
      free(N.data);
      return T;
   }
}

/* Delete str from T, and report SUCCESS/FAILURE via status */
htbl delete ( htbl T, char *str, int *status )
{
   unsigned int h, didx = T.s;

   *status = FAILURE;

   h = hash(str,T.a,T.t); printf("(%u,", h); /* First hash index for str */
   if ((T.data[h]) && (!strcmp(T.data[h],str))) {
      free(T.data[h]); T.data[h]= NULL; --(T.n);
      *status = SUCCESS; didx = h;
   }

   h = hash(str,T.b,T.t); printf("%u): ", h); /* Second hash index for str */
   if ((T.data[h]) && (!strcmp(T.data[h],str))) {
      free(T.data[h]); T.data[h]= NULL; --(T.n);
      *status = SUCCESS; didx = h;
   }

   if (didx < T.s) printf("deletion at index %u: ", didx);

   return T;
}

int main ( int argc, char *argv[] )
{
   htbl T;
   char *fname, str[1024];
   int n, status = SUCCESS;
   FILE *fp;

   if (argc >= 2) fname = argv[1];
   else {
      printf("Input file name: ");
      fname = (char *)malloc(1024 * sizeof(char));
      scanf("%s", fname);
   }

   /*** Initialize to empty table ***/
   T = init();

   fp = (FILE *)fopen(fname,"r");
   if (fp == NULL) exit(1);

   /*** Insert ***/
   fscanf(fp,"%d",&n); fscanf(fp,"%c",str);
   while (n--) {
      fgets(str,1024,fp);          /* Read an entire line */
      str[strlen(str)-1] = '\0';   /* Delete the trailing new line character */
      T = insert(T,str,&status,0); /* Insert */
   }

   /*** Search ***/
   printf("\n");
   fscanf(fp,"%d",&n); fscanf(fp,"%c",str);
   while (n--) {
      fgets(str,1024,fp);
      str[strlen(str)-1] = '\0';
      printf("+++ Search(%s): ", str);
      status = search(T,str);
      printf("%s\n", status ? "SUCCESS" : "FAILURE");
   }

   /*** Delete ***/
   printf("\n");
   fscanf(fp,"%d",&n); fscanf(fp,"%c",str);
   while (n--) {
      fgets(str,1024,fp);
      str[strlen(str)-1] = '\0';
      printf("+++ Delete(%s): ", str);
      T = delete(T,str,&status);
      printf("%s\n", status ? "SUCCESS" : "FAILURE");
   }

   fclose(fp);

   exit(0);
}