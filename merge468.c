/* 
 * Written by AJ Patalinghog
 *
 * This is an implementation of merge sort assuming the
 * data is an array a[] of length N, and N is a power of 2.
 *
 * The main() program uses a function called merge() which
 * merges subarrays in sorted order.  Rewrite the program
 * so that it uses threads to do the merging of the
 * subarrays.  In particular, each call to merge() creates
 * a thread.  
 *
 * The merging is done in a for-loop.  Each
 * pass through the for-loop causes subarrays of the same size
 * to be merged together.  The mergings within a pass should 
 * be done in parallel using one thread per merge. However,
 * the main program must wait for all the threads to complete
 * before doing another pass of the for-loop.  Thus, mergings
 * of subarrays of size 1 can be done in parallel, then
 * mergings of subarrays of size 2 can be done in parallel and
 * so forth.
 *
 * Use the pthread API
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define N 32 

// Global variables -- be careful
int a[N];     // Array to sort
int temp[N];  // Temporary storage

void genvalues(int n);     // Initializes array a[]
void prnvalues(int n);     // Prints array a[]
void * merge(void * args); // Merges subarrays

struct Info {
   pthread_t thread;
   int index;
   int arrsize;
};

int main() {
   int arrsize = 1;  // Size of subarrays to merge
   printf("Initial values:\n");
   genvalues(N);     // Initialize a[] with random values
   prnvalues(N);     // Display the values

   while (arrsize < N) {
      printf("*** Merging subarrays of size %d\n", arrsize);
      arrsize = 2 * arrsize; // merge subarrays to double subarray size
      int numThreads = N / arrsize;
      struct Info info[numThreads];
      for(int i = 0; i < numThreads; i++) {
         // printf("Creating thread %d of %d\n", i + 1, numThreads);
         info[i].index = i;
         info[i].arrsize = arrsize;
         if(pthread_create(&(info[i].thread), NULL, merge, (void *)(&info[i])) != 0) {
            perror("Error with creating threads.\n");
            exit(1);
         }
      }
      for(int i = 0; i < numThreads; i++) {
         pthread_join(info[i].thread, NULL);
      }
   }

   printf("\nOutput:\n");
   prnvalues(N); // Display the sorted values
}

/*
 * Merges subarrays (a[first], ..., a[midpt-1])
 *    and (a[midpt],..., a[last-1]) into
 *    subarray temp[first],..., temp[last-1] in increasing order.
 *    Then it copies temp[first],..., temp[last-1] back into
 *    a[first],..., a[last-1].
 */

void * merge(void * args) {
   int leftptr;   // Pointers used in array a[ ]
   int rightptr;
   int k;         // pointer used in array temp[ ]
   int delay;
   int first;     // Parameters used to merge two subarrays
   int midpt;     //   The first subarray starts at "first" and ends 
   int last;      //   at "first+midpt-1".  The second subarray starts
                  //   at "first+midpt" and ends at "last-1"

   struct Info * info = (struct Info *)args;
   first = info->index * info->arrsize;
   midpt = first + (info->arrsize / 2);
   last = (first + info->arrsize < N) ? first + info->arrsize : N;

   /*
    * Do not delete the next three lines.  They cause the function to
    * delay by a amount that is proportional to the subarray it is merging
    */
   
   delay = (last - first > 0) ? last - first : 1;
   usleep(delay * 250000);

   leftptr = first;
   rightptr = midpt; 

   // Merge values in the two arrays of a[] into temp[]
   for(k = first; k < last; k++) {
      if(leftptr >= midpt) temp[k] = a[rightptr++];
      else if(rightptr >= last) temp[k] = a[leftptr++];
      else if(a[leftptr] < a[rightptr]) temp[k] = a[leftptr++];
      else if(a[leftptr] >= a[rightptr]) temp[k] = a[rightptr++];
      else printf("There's a bug \n");
   }

   // Copy temp[] back to a[]
   for(k = first; k < last; k++) a[k] = temp[k];
}

// Initializes array a[] with random values.
void genvalues(int n) {
   int k = 2 * n;
   int current = 0;
   for(int i = 0; i < n; i++) {
      current = (current * 73 + 19) % k;
      a[i] = current;
   }
}

// Prints the values in the array a[]
void prnvalues(int n) {
   for(int i = 0; i < n; i++) {
      printf(" %d ", a[i]);
      if((i + 1) % 10 == 0) printf("\n");
   }
   printf("\n");
}

