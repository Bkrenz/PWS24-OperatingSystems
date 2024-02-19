#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/time.h>
#include <pthread.h>

#define MIN_NUM 1723453000
#define MAX_NUM 1723454200

struct thread_info {
    pthread_t thread_id;
    int threadNum;
    int startNum, endNum;
    int primesFound;
    int primes[(MAX_NUM - MIN_NUM)];
};

bool is_prime(int n);
void * count_primes_in_range(void *prime_range);
int run_threaded_primes(int num_threads);

void main()
{
    FILE *resultsfile;
    resultsfile = fopen("results.csv", "w");
    fprintf("threads,time\n");
    int time;
    for (int i = 0; i < 10; i++){
        // Run test with 1 thread
        printf("Running test %d with %d thread.\n", i+1, 1);
        time = run_threaded_primes(1);
        fprintf(resultsfile, "1,%d\n", time);

        // run test with 2 threads
        printf("Running test %d with %d threads.\n", i+1, 2);
        time = run_threaded_primes(2);
        fprintf(resultsfile, "2,%d\n", time);

        // Run test with 3 threads
        printf("Running test %d with %d threads.\n", i+1, 3);
        time = run_threaded_primes(3);
        fprintf(resultsfile, "3,%d\n", time);
        
        // Run test with 4 threads
        printf("Running test %d with %d threads.\n", i+1, 4);
        time = run_threaded_primes(4);
        fprintf(resultsfile, "4,%d\n", time);

        // Run test with 8 threads
        printf("Running test %d with %d threads.\n", i+1, 8);
        time = run_threaded_primes(8);
        fprintf(resultsfile, "8,%d\n", time);

    }
    fclose(resultsfile);
}

int run_threaded_primes(int num_threads) {
    int numPrimes=0;
    int rangeSize, status;
    pthread_t threads[num_threads];
    struct thread_info threadInfo[num_threads];
    struct timeval start, end;

    gettimeofday(&start, NULL);
    rangeSize = (MAX_NUM - MIN_NUM) / num_threads;
    for (int i=0; i < num_threads; i++) {
        // Setup Thread Info Data
        struct thread_info *info = &threadInfo[i];
        info->startNum = MIN_NUM + rangeSize * i + 1;
        info->endNum = MIN_NUM + rangeSize * (i+1);
        info->threadNum = i+1;
        info->primesFound = 0;

        // Create Thread
        status = pthread_create(&threads[i], NULL, count_primes_in_range, (void *)&threadInfo[i]);
        if (status != 0) {
            printf("Oops, pthread_create returned error code %d\n", status);
            exit(-1);
        }
    }

    for (int i=0; i < num_threads; i++)
        pthread_join(threads[i], NULL);
    
    gettimeofday(&end, NULL);
 

    // Print Info
    double msec;
    msec = (end.tv_sec - start.tv_sec) * 1000;
    msec += (end.tv_usec - start.tv_usec) / 1000; 

    for(int i = 0; i < num_threads; i++)
        numPrimes += threadInfo[i].primesFound;

    return (int) msec;
}

bool is_prime(int n)
{
    if (n == 2 || n == 3)
        return true;

    if (n <= 1 || n % 2 == 0 || n % 3 == 0)
        return false;

    for (int i = 5; i < n; i += 1)
    {
        if (n % i == 0)
            return false;
    }

    return true;
}

void * count_primes_in_range(void *infoPtr) {

    // Convert thread info to struct
    struct thread_info *info = (struct thread_info*) infoPtr;

    // Check Primes
    for( int i = info->startNum; i <= info->endNum; i++)
    {
        if (is_prime(i))
        {
            info->primesFound += 1;
            info->primes[info->primesFound - 1] = i;
        }
    }
    
    // Exit Thread
    pthread_exit(NULL);
}