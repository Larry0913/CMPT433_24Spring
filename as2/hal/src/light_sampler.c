#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#include "light_sampler.h"
#include "periodTimer.h"

static pthread_t sampling_id;
pthread_mutex_t historyMutex = PTHREAD_MUTEX_INITIALIZER;

bool running_flag = 1;
static double currentAverage = 0;
static double* history;
static int historyCount = 0;
static int sampleCount = 0;
static int totalCount = 0;

double getLightLevelVoltage1();
int getPotVoltageReading();
int getVoltage1Reading();
void *lightSamplerThread();

static long long getTimeInMs(void);
static void sleepForMs(long long delayInMs);


// Begin/end the background thread which samples light levels.
void Sampler_init(void)
{
    pthread_create(&sampling_id, NULL, lightSamplerThread, NULL);
}

void Sampler_cleanup(void)
{
    running_flag = 0;
    pthread_join(sampling_id, NULL);
}

// Must be called once every 1s.
// Moves the samples that it has been collecting this second into
// the history, which makes the samples available for reads (below).
void Sampler_moveCurrentDataToHistory(void)
{

}

// Get the number of samples collected during the previous complete second.
int Sampler_getHistorySize(void)
{

}

// Get a copy of the samples in the sample history.
// Returns a newly allocated array and sets `size` to be the 
// number of elements in the returned array (output-only parameter).
// The calling code must call free() on the returned pointer.
// Note: It provides both data and size to ensure consistency.
double* Sampler_getHistory(int *size)
{
    double* ret;
    pthread_mutex_lock(&historyMutex);
    {
        ret = history;
    }
    pthread_mutex_unlock(&historyMutex);
    return ret;
}

// Get the average light level (not tied to the history).
double Sampler_getAverageReading(void)
{

}

// Get the total number of light level samples taken so far.
long long Sampler_getNumSamplesTaken(void)
{
    pthread_mutex_lock(&historyMutex);
    long long buffer = historyCount;
    pthread_mutex_unlock(&historyMutex);

    return buffer;
}

void *lightSamplerThread()
{
    int cur_reading1 = getVoltage1Reading();
    double voltage = getLightLevelVoltage1(cur_reading1); 
    //set average as first reading when first run
    if (sampleCount == 0)
    {
        currentAverage = voltage;
    }

    int buffer_index = 0;
    double * buffer = malloc(INITIAL_BUFFER_SIZE * sizeof(double));

    int flag = 1;
    long long startTime = getTimeInMs();
    
    while(running_flag)
    {
        if (flag == 0)
        {
            startTime = getTimeInMs();
        }

        cur_reading1 = getVoltage1Reading();
        voltage = getLightLevelVoltage1(cur_reading1);
        Period_markEvent(PERIOD_EVENT_SAMPLE_LIGHT);

        if (buffer_index < INITIAL_BUFFER_SIZE) 
        {
            buffer[buffer_index] = voltage; 
        }
        buffer_index++;

        if(getTimeInMs() - startTime < 1000)
        {
            pthread_mutex_lock(&historyMutex);    
            {
                memset(history, 0, historyCount); 
                historyCount = buffer_index;// Reset sample count for the next second
                memcpy(history, buffer, historyCount * sizeof(double));
                historyCount = 0;
                buffer_index = 0; // Reset buffer index, logically clearing the buffer for the next second
                memset(buffer, 0, INITIAL_BUFFER_SIZE); 
           
                // Optionally, physically clear buffer if necessary for your application
            }
            pthread_mutex_unlock(&historyMutex);

        }

    }

    currentAverage = exponentialSmoothing(currentAverage, voltage, 0.1);




}

double getLightLevelVoltage1() {
	return ((double) getVoltage1Reading() / A2D_MAX_READING) * A2D_VOLTAGE_REF_V;
}

static double exponentialSmoothing(double curAvg, double nthSample, double weight)
{
    return weight * nthSample + (1 - weight) * curAvg;
}

int getPotVoltageReading()
{
    // Open file
    FILE *f = fopen(A2D_FILE_VOLTAGE0, "r");
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

static long long getTimeInMs(void)
{
    struct timespec spec;
    clock_gettime(CLOCK_REALTIME, &spec);
    long long seconds = spec.tv_sec;
    long long nanoSeconds = spec.tv_nsec;
    long long milliSeconds = seconds * 1000 + nanoSeconds / 1000000;
    return milliSeconds;
}

static void sleepForMs(long long delayInMs)
{
    const long long NS_PER_MS = 1000 * 1000;
    const long long NS_PER_SECOND = 1000000000;
    long long delayNs = delayInMs * NS_PER_MS; 
    int seconds = delayNs / NS_PER_SECOND;
    int nanoseconds = delayNs % NS_PER_SECOND;
    struct timespec reqDelay = {seconds, nanoseconds};
    nanosleep(&reqDelay, (struct timespec *) NULL);
}