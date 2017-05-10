#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N_DFT 10
#define C_DFT 1000
#define B_MIN 5
#define B_MAX 25
#define S_MIN 5
#define S_MAX 25

typedef struct {
   int minidx;    /* index of the day with the lowest buying rate */
   int maxidx;    /* index of the day with the highest selling rate */
   int buyidx;    /* index of the optimal buying day */
   int sellidx;   /* index of the optimal selling day */
   int profit;    /* optimal profit */
} profitdata;

void genprices ( int *B, int *S, int n )
{
   int i;

   printf("+++ Buying prices  :");
   for (i=0; i<n; ++i) printf(" %2d", (B[i] = B_MIN + rand() % (B_MAX - B_MIN + 1)));
   printf("\n+++ Selling prices :");
   for (i=0; i<n; ++i) printf(" %2d", (S[i] = S_MIN + rand() % (S_MAX - S_MIN + 1)));
   printf("\n");
}

void prnprofit ( profitdata opt, int *B, int *S )
{
   printf("    Buying  date = %2d, Buying  rate = %2d\n", opt.buyidx, B[opt.buyidx]);
   printf("    Selling date = %2d, Selling rate = %2d\n", opt.sellidx, S[opt.sellidx]);
   printf("\n    Maximum profit = %d\n", opt.profit);
}

profitdata singletrans1 ( int *B, int *S, int n, int C )
{
   profitdata lopt, ropt, opt;
   int lsize, rsize, lrprofit;
   int i, minidx, maxidx;

   if (n == 1) {
      opt.minidx = opt.maxidx = -1;               /* Not used in this function */
      opt.buyidx = opt.sellidx = 0;
      opt.profit = (C / B[0]) * (S[0] - B[0]);
      return opt;
   }

   lsize = (n+1)/2; rsize = n/2;                  /* size of the two halves */

   lopt = singletrans1(B,S,lsize,C);              /* recursive computation on left half */
   ropt = singletrans1(B+lsize,S+lsize,rsize,C);  /* recursive computation on right half */

   ropt.buyidx += lsize; ropt.sellidx += lsize;   /* update indices in ropt */

   /* Compute the minimum buying rate in the left half */
   minidx = 0;
   for (i=1; i<lsize; ++i) if (B[i] < B[minidx]) minidx = i;

   /* Compute the maximum selling rate in the right half */
   maxidx = lsize;
   for (i=lsize+1; i<n; ++i) if (S[i] > S[maxidx]) maxidx = i;

   lrprofit = (C / B[minidx]) * (S[maxidx] - B[minidx]);
   if ((lrprofit > lopt.profit) && (lrprofit > ropt.profit)) {  /* lrprofit is best */
      opt.minidx = opt.maxidx = -1;
      opt.buyidx = minidx; opt.sellidx = maxidx;
      opt.profit = lrprofit;
   } else
   if (lopt.profit > ropt.profit) opt = lopt;                  /* lopt is best */
   else opt = ropt;                                            /* ropt is best */

   return opt;
}

profitdata singletrans2 ( int *B, int *S, int n, int C )
{
   profitdata lopt, ropt, opt;
   int lsize, rsize, lrprofit;

   if (n == 1) {
      opt.minidx = opt.maxidx = opt.buyidx = opt.sellidx = 0;
      opt.profit = (C / B[0]) * (S[0] - B[0]);
      return opt;
   }

   lsize = (n+1)/2; rsize = n/2;                  /* size of the two halves */

   lopt = singletrans2(B,S,lsize,C);              /* recursive computation on left half */
   ropt = singletrans2(B+lsize,S+lsize,rsize,C);  /* recursive computation on right half */

   ropt.minidx += lsize; ropt.maxidx += lsize;    /* update indices in ropt */
   ropt.buyidx += lsize; ropt.sellidx += lsize;

   /* compute best profit if buying is in first half and selling in second half */
   lrprofit = (C / B[lopt.minidx]) * (S[ropt.maxidx] - B[lopt.minidx]);

   if ((lrprofit > lopt.profit) && (lrprofit > ropt.profit)) {  /* lrprofit is best */
      opt.buyidx = lopt.minidx; opt.sellidx = ropt.maxidx;
      opt.profit = lrprofit;
   } else if (lopt.profit > ropt.profit) {                      /* lopt is best */
      opt.buyidx = lopt.buyidx; opt.sellidx = lopt.sellidx;
      opt.profit = lopt.profit;
   } else {
      opt.buyidx = ropt.buyidx; opt.sellidx = ropt.sellidx;     /* ropt is best */
      opt.profit = ropt.profit;
   }

   /* Take the minimum of two minimums, and maximum of two maximums. This
      recursive computation changes O(n) to O(1) in the recurrence relation
      of the running time. */
   opt.minidx = (B[lopt.minidx] <= B[ropt.minidx]) ? lopt.minidx : ropt.minidx;
   opt.maxidx = (S[lopt.maxidx] >= S[ropt.maxidx]) ? lopt.maxidx : ropt.maxidx;

   return opt;
}

profitdata singletrans3 ( int *B, int *S, int n, int C )
{
   int minidx, buyidx, sellidx, profit, maxprofit, i;
   profitdata opt;

   /* minidx stores the day with the minimum buying rate seen so far,
      and maxprofit the maximum profit located so far. For i=0, only
      one day is considered to initialize these two values. */
   minidx = buyidx = sellidx = 0;
   maxprofit = (C / B[0]) * (S[0] - B[0]);

   /* Loop for the remaining days */
   for (i=1; i<n; ++i) {
      /* Update minidx if a lower buying rate is located */
      if (B[i] < B[minidx]) minidx = i;

      /* Compute the profit if items are bought on the minidx-th day and
         sold on the i-th day */
      profit = (C / B[minidx]) * (S[i] - B[minidx]);

      /* If a better profit is found, record it. */
      if (profit > maxprofit) {
         maxprofit = profit;
         buyidx = minidx;
         sellidx = i;
      }
   }

   opt.minidx = minidx; opt.maxidx = -1;
   opt.buyidx = buyidx; opt.sellidx = sellidx; opt.profit = maxprofit;

   return opt;
}

/* This is similar to singletrans3 except that the array is read from end to
   beginning, and the maximum selling rate is remembered. This idea is used
   in multitrans(). */
profitdata singletrans4 ( int *B, int *S, int n, int C )
{
   int maxidx, buyidx, sellidx, profit, maxprofit, i;
   profitdata opt;

   maxidx = buyidx = sellidx = n-1;
   maxprofit = (C / B[n-1]) * (S[n-1] - B[n-1]);

   for (i=n-2; i>=0; --i) {
      if (S[i] > S[maxidx]) maxidx = i;
      profit = (C / B[i]) * (S[maxidx] - B[i]);
      if (profit > maxprofit) {
         maxprofit = profit;
         buyidx = i;
         sellidx = maxidx;
      }
   }

   opt.minidx = -1; opt.maxidx = maxidx;
   opt.buyidx = buyidx; opt.sellidx = sellidx; opt.profit = maxprofit;

   return opt;
}

/* Recursive printing of optimal transactions computed by multitrans1 */
int prnhistory1 ( int *B, int *S, int C, int *buyidx, int *sellidx, int idx )
{
   int i, j;

   i = buyidx[idx]; j = sellidx[idx];
   if (i == -1) {
      printf("\n    Initial capital = %d\n\n", C);
      return C;
   }
   C = prnhistory1(B,S,C,buyidx,sellidx,i);
   C += (C / B[i]) * (S[j] - B[i]);
   printf("    Buying  date = %2d, Buying  rate = %2d\n", i, B[i]);
   printf("    Selling date = %2d, Selling rate = %2d\n", j, S[j]);
   printf("    Current capital = %d\n\n", C);
   return C;
}

void multitrans1 ( int *B, int *S, int n, int C )
{
   int i, j, maxidx, *maxcap, thiscap, *buyidx, *sellidx;

   /* maxcap[i] stores the maximum capital at the beginning of Day i. This
      is the same as the maximum capital at the end of Day i-1. */
   maxcap = (int *)malloc((n+1) * sizeof(int));

   /* buyidx[i] and sellidx[i] stores the last transaction dates in order
      to have capital maxcap[i] at the beginning of Day i */
   buyidx = (int *)malloc((n+1) * sizeof(int));
   sellidx = (int *)malloc((n+1) * sizeof(int));

   /* At the beginning of Day 0, we have only the initial capital, and no
      transactions have been made so far. */
   maxcap[0] = C; buyidx[0] = sellidx[0] = -1;

   /* Optimize the maximum capital at the beginning of Day 1 through Day n,
      that is, at the end of Day 0 through Day n-1. */
   for (i=1; i<=n; ++i) {
      /* Initialize the i-th entries assuming that the no buying or selling is
         made in Day i-1. */
      maxcap[i] = maxcap[i-1];
      buyidx[i] = buyidx[i-1];
      sellidx[i] = sellidx[i-1];

      /* In the loop below, maxidx stores the index of the highest selling
         rate in the range j...i-1 */
      maxidx = i-1;

      /* The loop assumes that the last buying is done on Day j */
      for (j=i-1; j>=0; --j) {
         /* If a larger selling rate is found, update maxidx */
         if (S[j] >= S[maxidx]) maxidx = j;

         /* Start with the maximum capital at the beginning of Day j.
            Buy (maxcap[j] / B[j]) items on Day j. Sell on the maxidx-th
            day which offers the highest selling rate. */
         thiscap = maxcap[j] + (maxcap[j] / B[j]) * (S[maxidx] - B[j]);

         /* If this way we get a better capital at the beginning of Day i,
            update the optimal values */
         if (thiscap > maxcap[i]) {
            maxcap[i] = thiscap;
            buyidx[i] = j;
            sellidx[i] = maxidx;
         }
      }
   }

   /* Recursively print the optimal transaction history */
   prnhistory1(B,S,C,buyidx,sellidx,n);

   /* The maximum capital is stored in maxcap[n]. It can also be obtained
      from the return value of prnhistory(). Subtract the initial capital
      to obtain the overall profit. */
   printf("    Maximum profit = %d\n", maxcap[n] - C);

   /* Book-keeping */
   free(buyidx); free(sellidx); free(maxcap);
}

/* Recursive transaction printing function for multitrans2 */
void prnhistory2 ( int *B, int *S, int *maxcap, int *maxitm, int *maxbal, int j )
{
   int i;

   /* As long as the maximum capital did not change, decrement j. No selling
      must have taken place on these days. If the loop terminates with some
      j > 0, we have found the last selling date (j-1). */
   while ((j > 0) && (maxcap[j] == maxcap[j-1])) --j;

   if (j == 0) return; /* Capital could never be increased. Stop. */

   /* Now that some selling happened on Day j-1, items must have been bought
      on some Day i-1 with i <= j. Keep decrementing i until a day is found
      when buying increased the number of items or the available balance
      (without decreasing the number of items). When the loop terminates with
      some i > 0, we have located the last buying date (i-1). Notice that
      in this loop, we do not need to add the condition (i > 0), because a
      capital increase on Day j-1 can happen only if some buying has happened
      before the selling on Day j-1. */
   i = j;
   while ((maxitm[i] == maxitm[i-1]) && (maxbal[i] == maxbal[i-1])) --i;


   /* Here, you have got the last transaction:
      buying at Day i-1, and selling at Day j-1, with i <= j. */

   /* First make a recursive call to print the previous transactions */
   prnhistory2(B,S,maxcap,maxitm,maxbal,i-1);

   /* Then print the last transaction */
   printf("\n    Buying  date = %2d, Buying  rate = %2d\n", i-1, B[i-1]);
   printf("    Selling date = %2d, Selling rate = %2d\n", j-1, S[j-1]);
   printf("    Current capital = %d\n", maxcap[j]);
}

/* This O(n)-time O(n)-space algorithm was suggested by ADDANKI SURYA TEJA */
void multitrans2 ( int *B, int *S, int n, int C )
{
   int *maxcap, *maxitm, *maxbal, nitem, bal, cap, i;

   /* The following three arrays are indexed 0,1,...,n, and stores a best
      value at the beginning of Day i (which is the same as the corresponding
      value at the end of Day i-1). */

   /* maxcap[i] stores the maximum capital that we may have as Day i begins */
   maxcap = (int *)malloc((n + 1)*sizeof(int));

   /* maxitm[i] stores the maximum number of items that we may have as Day i begins */
   maxitm = (int *)malloc((n + 1)*sizeof(int));

   /* maxbal[i] stores the maximum balance (leftover money from the previous
      purchase) that we may have as Day i begins */
   maxbal = (int *)malloc((n + 1)*sizeof(int));

   /* At the beginning of Day 0, we have initial capital C, and no transactions
      have taken place so far. */
   maxcap[0] = C; maxitm[0] = 0; maxbal[0] = 0;

   /* Update the array entries from the beginning of Day i to the end of Day i
      (that is, to the beginning of Day i+1). */
   for (i=0; i<n; ++i) {
      /* If the i-th day is a no-activity day, the array entries are just copied */
      maxcap[i+1] = maxcap[i]; maxitm[i+1] = maxitm[i]; maxbal[i+1] = maxbal[i];

      /* Look at the possibility of increasing the number of items by buying
         items with the maximum available capital. If the number of items
         remains the same, but the balance increases by this purchase, go
         ahead with it. */
      nitem = maxcap[i] / B[i]; bal = maxcap[i] % B[i];
      if ((nitem > maxitm[i]) || ((nitem == maxitm[i]) && (bal > maxbal[i]))) {
         maxitm[i+1] = nitem; maxbal[i+1] = bal;
      }

      /* Now see if you can sell the maximum possible number of items that you
         have in order to increase the capital */
      cap = maxitm[i+1] * S[i] + maxbal[i+1];
      if (cap > maxcap[i]) maxcap[i+1] = cap;
   }

   /* Recursively print the transactions */
   prnhistory2(B,S,maxcap,maxitm,maxbal,n);

   printf("\n    Maximum profit = %d\n", maxcap[n]-C);

   free(maxcap); free(maxitm); free(maxbal);
}

int main ( int argc, char *argv[] )
{
   int n, C;
   int *B, *S;
   profitdata opt;

   srand((unsigned int)time(NULL));

   n = (argc >= 2) ? atoi(argv[1]) : N_DFT;
   printf("+++ n = %d\n", n);
   B = (int *)malloc(n * sizeof(int));
   S = (int *)malloc(n * sizeof(int));
   genprices(B,S,n);
   C = (argc >= 3) ? atoi(argv[2]) : C_DFT;
   printf("+++ C = %d\n", C);

   printf("\n+++ Single transaction: O(n log n) time, O(log n) space\n\n");
   opt = singletrans1(B,S,n,C);
   prnprofit(opt,B,S);

   printf("\n+++ Single transaction: O(n) time, O(log n) space\n\n");
   opt = singletrans2(B,S,n,C);
   prnprofit(opt,B,S);

   printf("\n+++ Single transaction: O(n) time, O(1) space\n\n");
   opt = singletrans3(B,S,n,C);
   prnprofit(opt,B,S);

   printf("\n+++ Single transaction: O(n) time, O(1) space\n\n");
   opt = singletrans4(B,S,n,C);
   prnprofit(opt,B,S);

   printf("\n+++ Multiple transactions: O(n^2) time, O(n) space\n");
   multitrans1(B,S,n,C);

   printf("\n+++ Multiple transactions: O(n) time, O(n) space\n");
   multitrans2(B,S,n,C);

   free(B); free(S);
   exit(0);
}