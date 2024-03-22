/**
 * @file a5.c
 * @author Robert Krency
 * @brief Assignment 5 - CMSC 4000 Operating Systems
 * @date 2024-03-21
 * 
 */

#include <stdbool.h>
#include <stdio.h>
#include <sys/time.h>
#include <pthread.h>

#define MIN_NUM 1723453000
#define MAX_NUM 1723454200

#define BUFFER_SIZE 2

/**
 * @brief The information for a consumer thread.
 * 
 */
struct consumer_info {
    pthread_t threadId;
    int threadNum;
    int primesFound;
    int primes[(MAX_NUM - MIN_NUM)];
};

/**
 * @brief The information for a producer thread.
 * 
 */
struct producer_info {
    pthread_t threadId;
    int threadNum;
};

/* Function Defs */
bool check_prime(int n);
void * consume();
void * produce();
void run_threaded_primes(int numConsumers);

/* Output Files */
FILE *resultsFile;
FILE *primesFile;
bool primesWritten = false;

/* The buffer containing the numbers to be checked for primality.
    The buffer is filled by the producer, and consumber by the
    consumers.
 */
int numsBuffer[BUFFER_SIZE];
bool allNumsChecked = false;
pthread_mutex_t bufferMutex = PTHREAD_MUTEX_INITIALIZER;

/**
 * @brief The access point of the program
 * 
 * @return int Error code upon exit. 0 for success.
 */
int main() {

    // Setup the output file
    resultsFile = fopen("results_c.csv", "w");
    primesFile = fopen("primes.txt", "w");

    // Run tests
    for (int i = 0; i < 30; i++) {
        run_threaded_primes(2);
        run_threaded_primes(3);
        run_threaded_primes(4);
        run_threaded_primes(8);
    }

    // Exit Successfully
    return 0;
}

/**
 * @brief Runs a full test case of the threaded primes program.
 * 
 * @param numConsumers The number of consumer threads to use.
 * @param resultsFile 
 */
void run_threaded_primes(int numConsumers) {
    printf("[MAIN] Running tests with %d consumers.\n", numConsumers);
    
    // Init Data
    struct timeval start, end;
    pthread_t threads[numConsumers + 1];
    struct producer_info prodInfo;
    struct consumer_info consumerInfo[numConsumers];
    int threadStatus;

    // Init Buffer
    for (int i=0; i < BUFFER_SIZE; i++)
        numsBuffer[i] = 0;

    // Start the test timer
    gettimeofday(&start, NULL);

    // Setup the Producer Thread
    threadStatus = pthread_create(&threads[numConsumers],
                NULL,
                produce, 
                (void *) &prodInfo);

    // Setup the Consumer Threads
    for (int i=0; i < numConsumers; i++) {
        struct consumer_info *info = &consumerInfo[i];
        info->threadNum = i+1;
        info->primesFound = 0;

        threadStatus = pthread_create( &threads[i],
                    NULL,
                    consume,
                    (void *) &consumerInfo[i]);
    }

    // Wait for all threads to finish
    for (int i=0; i < numConsumers + 1; i++)
        pthread_join(threads[i], NULL);

    // End the test timer
    gettimeofday(&end, NULL);

    // Get the total number of primes found
    int primes = 0;
    for (int i=0; i < numConsumers; i++)
    {
        primes += consumerInfo[i].primesFound;
        printf("[CONS %d] # of Primes: %d\n", i+1, consumerInfo[i].primesFound);
    }
    printf("[MAIN] Total # of Primes: %d\n", primes);

    // Print the primes to the primes file
    if (!primesWritten) {
        for (int i=0; i < numConsumers; i++) {
            for (int j=0; j < consumerInfo[i].primesFound; j++) {
                fprintf(primesFile, "%d\n", consumerInfo[i].primes[j]);
            }
        }
        primesWritten = true;
    }

    // Print this run's data to the results file
    double msec;
    msec = (end.tv_sec - start.tv_sec) * 1000;
    msec += (end.tv_usec - start.tv_usec) / 1000; 
    fprintf(resultsFile, "%d, %f, %d\n", numConsumers, msec, primes);
    
    // Finish Test Run
    printf("[MAIN] Test has finished.\n\n");
}


/**
 * @brief Checks if a number is prime
 * 
 * @param n The input number
 * @return true Number is prime
 * @return false Number is not prime
 */
bool check_prime(int n) {
    if (n == 2 || n == 3)
        return true;

    if (n <= 1 || n % 2 == 0 || n % 3 == 0)
        return false;

    for (int i = 5; i * i < n ; i += 6)
    {
        if (n % i == 0 || n % (i+2) == 0)
            return false;
    }

    return true;
}


void * produce( void *infoPtr ) {
    // Convert thread info to struct
    struct producer_info *info = (struct producer_info *) infoPtr;

    // Print Initialization Confirmation
    printf("[PROD] Producer has been initialized.\n");

    // Fill the buffer with all numbers in the range specified
    int currentNumber = MIN_NUM;
    allNumsChecked = false;
    while ( currentNumber <= MAX_NUM ) {

        // Get a lock on the buffer
        pthread_mutex_lock(&bufferMutex);
        
        // Refill the buffer if needed
        for (int i=0; i < BUFFER_SIZE; i++) {
            if (numsBuffer[i] == 0 && currentNumber <= MAX_NUM) {
                numsBuffer[i] = currentNumber;
                currentNumber += 1;
            }
        }

        // Free the lock on the buffer
        pthread_mutex_unlock(&bufferMutex);
    }

    // Finish Producing numbers
    allNumsChecked = true;
    printf("[PROD] Producer has finished.\n");
}

void * consume(void *infoPtr ) {
    // Convert thread info to struct
    struct consumer_info *info = (struct consumer_info *) infoPtr;

    // Print initialization confirmation
    printf("[CONS %d] Consumer %d has been initialized.\n", info->threadNum, info->threadNum);

    int num;
    while ( !allNumsChecked ) {
        
        // Get a lock on the buffer
        pthread_mutex_lock(&bufferMutex);

        // Get a number from the buffer
        num = 0;
        for (int i=0; i < BUFFER_SIZE; i++) {
            if (numsBuffer[i] != 0) {
                num = numsBuffer[i];
                numsBuffer[i] = 0;
                break;
            }
        }

        // Free the lock on the buffer
        pthread_mutex_unlock(&bufferMutex);

        // Check if the number is prime
        if ( num != 0 && check_prime(num)) {
            printf("[CONS %d] Prime found: %d\n", info->threadNum, num);
            info->primes[info->primesFound] = num;
            info->primesFound += 1;
        }
    }

    // Close out of consumer
    printf("[CONS %d] Consumer %d has finished.\n", info->threadNum, info->threadNum);
}
