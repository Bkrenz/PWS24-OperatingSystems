#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/time.h>
#include <pthread.h>

#define MIN_NUM 1723453000
#define MAX_NUM 1723454200
#define NUM_TESTS 30

struct thread_info {
    pthread_t thread_id;
    int threadNum;
    int startNum, endNum;
    int primesFound;
    int primes[(MAX_NUM - MIN_NUM)];
    struct timeval startTime, endTime;
};

bool is_prime(int n);
void * count_primes_in_range(void *prime_range);
void run_threaded_primes(int num_threads, FILE* resultsFile);

void main()
{
    FILE *resultsfile;
    resultsfile = fopen("results.csv", "w");
    int time;
    for (int i = 0; i < NUM_TESTS; i++){
        run_threaded_primes(1, resultsfile);
        run_threaded_primes(2, resultsfile);
        run_threaded_primes(3, resultsfile);
        run_threaded_primes(4, resultsfile);
        run_threaded_primes(8, resultsfile);
    }
    fclose(resultsfile);
}

void run_threaded_primes(int num_threads, FILE* resultsFile) {
    printf("[MAIN] Running test with %d threads.\n", num_threads);
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
    
    // Print the info to the results file.
    fprintf(resultsFile, "%d,%.0f", num_threads, msec);
    for(int i = 0; i < num_threads; i++) {
        start = threadInfo[i].startTime;
        end = threadInfo[i].endTime;
        msec = (end.tv_sec - start.tv_sec) * 1000;
        msec += (end.tv_usec - start.tv_usec) / 1000; 
        fprintf(resultsFile, ",%d,%.0f", threadInfo[i].primesFound, msec);
        printf("[MAIN] Time taken by Thread %d: %6.0f milliseconds. Primes found: %d\n", i, msec, threadInfo[i].primesFound);
    }
    fprintf(resultsFile, "\n");
}

bool is_prime(int n)
{
    if (n == 2 || n == 3)
        return true;

    if (n <= 1 || n % 2 == 0 || n % 3 == 0)
        return false;

    for (int i = 5; i < n - 2; i += 6)
    {
        if (n % i == 0 || n % (i+2) == 0)
            return false;
    }

    return true;
}

void * count_primes_in_range(void *infoPtr) {

    // Convert thread info to struct
    struct thread_info *info = (struct thread_info*) infoPtr;
    gettimeofday(&(info->startTime), NULL);

    // Check Primes
    for( int i = info->startNum; i <= info->endNum; i++)
    {
        if (is_prime(i))
        {
            info->primesFound += 1;
            printf("[Thread %d] Prime found: %d; %d found by this thread.\n", info->threadNum, i, info->primesFound);
            info->primes[info->primesFound - 1] = i;
        }
    }
    gettimeofday(&(info->endTime), NULL);
    
    // Exit Thread
    pthread_exit(NULL);
}