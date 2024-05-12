#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <time.h> 

#include "accelerometer.h"
#include "audioGeneration.h"
#include "audioMixer_template.h"
#include "threadController.h"
#include "periodTimer.h"
#include "print.h"

static pthread_t print_id;
Period_statistics_t pStat;

void print_init()
{
    pthread_create(&print_id, NULL, printDataThread, NULL);
}

void print_wait()
{
    pthread_join(print_id, NULL);
}

void *printDataThread(void *args)
{
    (void)args;
    // long long startTimeStamp = getTimeInMs();
    // long long endTimeStamp = 0;

    while (running_flag)
    {
        // endTimeStamp = getTimeInMs();
        // if(endTimeStamp - startTimeStamp >= 1000)
        // {
        sleepForMs(500);

        printf("M%d %dbpm, vol:%d  ", getMode(), getBpmValue(), AudioMixer_getVolume());

        if (running_flag)
        {
            Period_getStatisticsAndClear(PERIOD_EVENT_SAMPLE_AUDIO, &pStat);
            printf("Audio[%.3lf, %.3lf] avg %.3f/%d  ", pStat.minPeriodInMs, pStat.maxPeriodInMs, pStat.avgPeriodInMs, pStat.numSamples);

            Period_getStatisticsAndClear(PERIOD_EVENT_SAMPLE_ACCELEROMETER, &pStat);
            printf("Accel[%.3lf, %.3lf] avg %.3f/%d\n", pStat.minPeriodInMs, pStat.maxPeriodInMs, pStat.avgPeriodInMs, pStat.numSamples);
        }
        
        sleepForMs(500);
        //     startTimeStamp = getTimeInMs();
        // }
    
    }
    pthread_exit(NULL);
    return NULL;
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
