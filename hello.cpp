// File: Pthread_Monte_Carlo.c
// Purpose: Parallelize Monte_carlo program using pthreads
// Author: Jonathan M. Kelly
// Date: 12 December 2016
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define BILLION 1E9;

long long int num_threads;
long long int total_iterations;

void Get_input(long long int* number_of_threads, long long int* total_its);
void *Monte_carlo(void* threadID);

double randNumGen(){
  int rand_val = rand();
  double unit_rand;
  unit_rand = 2*(rand_val / (double)RAND_MAX) - 1.0; // between -1 and 1
  return unit_rand;
}

void Get_input (long long int* number_of_threads, long long int* total_iterations) {

    printf("Enter the total number of tosses\n");
    scanf("%lld", number_of_threads);
    printf("Enter the total number of iterations\n");
    scanf("%lld", total_iterations);  
} /* Get_input */

void *Monte_carlo(void *threadID){
  long long t_ID;
  t_ID = (long long)threadID;

  int tID = (int)t_ID;

  // malloc so that variable remains after thread call
  
  double *number_in_circle = (double *)malloc(sizeof(double));
  *number_in_circle = 0;

  // get the total iterations for the thread

  long long int total_i = total_iterations/ num_threads;

  int counter = 0;

  // calculation

  for (; counter < total_i; counter++){
    double x = randNumGen();
    double y = randNumGen();

    double distance_squared = x * x + y*y;

    if (distance_squared <= 1){
    number_in_circle++;
    }
  }

  // get the remaining iterations calculated by indexed thread
  if(tID == 0){
    double remainder = total_iterations % num_threads;

    for (counter = 0; counter < remainder; counter++){
    double x = randNumGen();
    double y = randNumGen();

    double distance_squared = x * x + y*y;

        if (distance_squared <= 1){
	  number_in_circle++; // number in unit circle
	}
    }
  pthread_exit((void *)number_in_circle); // return the num in circle
  }
} /* Monte_carlo */

int main(void)
{
  pthread_t *threads;
  int threadVar;
  long long int i;
  void *status;
  double total_num_in_circle = 0;
  
  Get_input(&num_threads, &total_iterations);

  threads = malloc(num_threads*sizeof(long long int));
  // Calculate time for a request
  struct timespec requestStart, requestEnd;
  clock_gettime(CLOCK_REALTIME, &requestStart);
  
  for(i = 0; i < num_threads; i++){
    threadVar = pthread_create(&threads[i], NULL, Monte_carlo, NULL);
    if (threadVar){
      printf("There was an error when creating your pthread. The return code is %d/n", threadVar);
      exit(0);
    }
  }

  // join the threads
  for (i = 0; i < num_threads; i++){
    pthread_join(threads[i], &status);

    total_num_in_circle += *(double*)status;
  }

  clock_gettime(CLOCK_REALTIME, &requestEnd);
  // Calculate total time

  double time = (requestEnd.tv_sec - requestStart.tv_sec) + (requestEnd.tv_nsec - requestStart.tv_nsec) / BILLION;

  double pi_estimate = 4*(total_num_in_circle/total_iterations);
  
  printf("Total duration: %f\n", time);
  printf("Pi estimate: %f \n", pi_estimate);

  // exit pthread
  pthread_exit(NULL);

  return 0;
}  /* main */
