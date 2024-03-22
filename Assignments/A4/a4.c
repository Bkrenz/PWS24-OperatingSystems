typedef int semaphore;
semaphore dbMutex;
semaphore readQueueMutex = 1, readActiveMutex = 1;
int readQueueCount = 0, readActiveCount = 0;
semaphore writeQueueMutex = 1;
int writeQueueCount = 0;

void reader(void) {
    while (TRUE) {
        // Queue up a Reader
        down(&readQueueMutex);
        readQueueCount += 1;
        up(&readQueueMutex);

        // Activate a reader when allowed
        down(&readActiveMutex);
        readActiveCount += 1;
        if (readActiveCount == 1) 
            down(&dbMutex);
        readQueueCount -= 1;
        up(&readActiveMutex);

        // Do the work
        read_data();

        // Release Resources
        down(&readActiveMutex);
        readActiveCount -= 1;
        if (readActiveCount == 0)
            up(&dbMutex);
        up(&readActiveCount);

        // Use the read data
        use_data();

    }
}

void writer(void) {
    while (TRUE) {
        think_up_data();

        // Queue up a writer
        down(&writeQueueMutex);
        writeQueueCount += 1;
        if (writeQueueCount == 1) // Prevent readers from queueing.
            down(&readQueueMutex);
        up(&writeQueueMutex);

        // Do the work of a writer
        down(&dbMutex);
        write_data();
        up(&dbMutex);

        // Release Resources
        down(&writeQueueMutex);
        writeQueueCount -= 1;
        if (writeQueueCount == 0) // Prevent readers from queueing.
            up(&readQueueMutex);
        up(&writeQueueMutex);
    }
}