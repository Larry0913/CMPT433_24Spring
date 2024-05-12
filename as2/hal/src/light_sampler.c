#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <time.h>

#include "threadControl.h"
#include "periodTimer.h"
#include "light_sampler.h"

static pthread_t sampling_id;
pthread_mutex_t historyMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t getterMutex = PTHREAD_MUTEX_INITIALIZER;

double currentAverage = 0;
double *history;
int historyCount = 0;
int totalCount = 0;


// Begin/end the background thread which samples light levels.
void Sampler_init()
{
    pthread_create(&sampling_id, NULL, lightSamplerThread, NULL);
}

void Sampler_wait()
{
    pthread_join(sampling_id, NULL);
}
    
void Sampler_cleanup(void)
{
    pthread_mutex_destroy(&historyMutex);
    pthread_mutex_destroy(&getterMutex);
}

// Must be called once every 1s.
// Moves the samples that it has been collecting this second into
// the history, which makes the samples available for reads (below).
void Sampler_moveCurrentDataToHistory(int buffer_index, double *buffer)
{
    pthread_mutex_lock(&historyMutex);    
    {
        memset(history, 0, BUFFER_MAX_SIZE * sizeof(double)); 
        historyCount = buffer_index; // Reset sample count for the next second
        memcpy(history, buffer, buffer_index * sizeof(double));
    }
    pthread_mutex_unlock(&historyMutex);

}

// Get the number of samples collected during the previous complete second.
int Sampler_getHistorySize(void)
{
    pthread_mutex_lock(&historyMutex);
    int size = historyCount;
    pthread_mutex_unlock(&historyMutex);
    return size;
}

// Get a copy of the samples in the sample history.
// Returns a newly allocated array and sets `size` to be the 
// number of elements in the returned array (output-only parameter).
// The calling code must call free() on the returned pointer.
// Note: It provides both data and size to ensure consistency.
double* Sampler_getHistory(int *size)
{
    pthread_mutex_lock(&historyMutex);
    *size = historyCount;
    double* ret = (double *)malloc(*size * sizeof(double));
    if (ret != NULL) 
    { 
        memcpy(ret, history, *size * sizeof(double));
    }
    pthread_mutex_unlock(&historyMutex);
    return ret;
}

// Get the average light level (not tied to the history).
double Sampler_getAverageReading(void)
{
    pthread_mutex_lock(&getterMutex);
    double avg = currentAverage;
    pthread_mutex_unlock(&getterMutex);
    return avg;
}

// Get the total number of light level samples taken so far.
long long Sampler_getNumSamplesTaken(void)
{
    pthread_mutex_lock(&getterMutex);
    long long buffer = totalCount;
    pthread_mutex_unlock(&getterMutex);

    return buffer;
}

void *lightSamplerThread(void *args)
{
    (void)args;
    int cur_reading1 = getVoltage1Reading();
    double voltage = getLightLevelVoltage1(cur_reading1); 
    //set average as first reading when first run
    if (totalCount == 0)
    {
        currentAverage = voltage;
    }

    history = malloc(BUFFER_MAX_SIZE * sizeof(double));
    int buffer_index = 0;
    //double * buffer = malloc(BUFFER_MAX_SIZE * sizeof(double));
    double buffer[BUFFER_MAX_SIZE] = {0};
    
    long long startTime = getTimeInMs();
    
    while(running_flag)
    {
        cur_reading1 = getVoltage1Reading();
        voltage = getLightLevelVoltage1(cur_reading1);

        if(running_flag == 0)
        {
            break;
        }
        
        Period_markEvent(PERIOD_EVENT_SAMPLE_LIGHT);
        buffer[buffer_index++] = voltage; 
        currentAverage = exponentialSmoothing(currentAverage, voltage, 0.001);
        totalCount++;
        sleepForMs(1);
        
        if(getTimeInMs() - startTime >= 1000)
        {
            Sampler_moveCurrentDataToHistory(buffer_index, buffer);
            buffer_index = 0; // Reset buffer index, logically clearing the buffer for the next second
            memset(buffer, 0, BUFFER_MAX_SIZE * sizeof(double));
            startTime = getTimeInMs(); 
        }
    }
    
    free(history);
    pthread_exit(NULL);
    return NULL;

}

double getLightLevelVoltage1() {
	return ((double) getVoltage1Reading() / A2D_MAX_READING) * A2D_VOLTAGE_REF_V;
}

double exponentialSmoothing(double curAvg, double nthSample, double weight)
{
    return weight * nthSample + (1 - weight) * curAvg;
}

int getVoltage1Reading()
{
    // Open file
    FILE *f = fopen(A2D_FILE_VOLTAGE1, "r");
    if (!f) {
        printf("ERROR: Unable to open voltage input file. Cape loaded?\n");
        printf(" Check /boot/uEnv.txt for correct options.\n");
        exit(-1);
    }
    // Get reading
    int a2dReading = 0;
    int itemsRead = fscanf(f, "%d", &a2dReading);
    if (itemsRead <= 0) {
        printf("ERROR: Unable to read values from voltage input file.\n");
        exit(-1);
    }
    // Close file
    fclose(f);
    return a2dReading;
}

long long getTimeInMs(void)
{
    struct timespec spec;
    clock_gettime(CLOCK_REALTIME, &spec);
    long long seconds = spec.tv_sec;
    long long nanoSeconds = spec.tv_nsec;
    long long milliSeconds = seconds * 1000 + nanoSeconds / 1000000;
    return milliSeconds;
}

void sleepForMs(long long delayInMs)
{
    const long long NS_PER_MS = 1000 * 1000;
    const long long NS_PER_SECOND = 1000000000;
    long long delayNs = delayInMs * NS_PER_MS; 
    int seconds = delayNs / NS_PER_SECOND;
    int nanoseconds = delayNs % NS_PER_SECOND;
    struct timespec reqDelay = {seconds, nanoseconds};
    nanosleep(&reqDelay, (struct timespec *) NULL);
}
