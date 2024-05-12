#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>

#include "threadControl.h"
#include "periodTimer.h"
#include "samplerDips.h"
#include "light_sampler.h"
#include "PWM.h"

static pthread_t Dip_id;
int dipCount = 0;
pthread_mutex_t dipMutex = PTHREAD_MUTEX_INITIALIZER;
Period_statistics_t pStats;

void Dip_init()
{
    //printf("dip count starting!\n");
    pthread_create(&Dip_id, NULL, DipCounterThread, NULL);
}

void Dip_wait()
{
    pthread_join(Dip_id, NULL);
}

void Dip_cleanup(void)
{
    pthread_mutex_destroy(&dipMutex);
}

void *DipCounterThread(void *args)
{
    (void)args;
    bool aboveThreshold = true;
    int numOfSamples;
    double curAvg;
    double *current_history;

    while (running_flag)
    {
        current_history = Sampler_getHistory(&numOfSamples); 
        for (int i = 0; i < numOfSamples; i++) 
        {
            // Update the current average using exponential smoothing
            curAvg = Sampler_getAverageReading();

            // Check for a dip
            if (aboveThreshold && current_history[i] < (curAvg) - THRESHOLD) {
                pthread_mutex_lock(&dipMutex);
                {
                    dipCount++;
                }
                pthread_mutex_unlock(&dipMutex);
                aboveThreshold = false; // The light level has dipped below the threshold
            } 
            // Check if the light level has risen back up to re-enable dip detection
            else if (!aboveThreshold && current_history[i] > (curAvg) - THRESHOLD + HYSTERESIS) {
                aboveThreshold = true;
            }
        }

        sleepForMs(1000);
        if(running_flag == 1)
        {
            Period_getStatisticsAndClear(PERIOD_EVENT_SAMPLE_LIGHT, &pStats);
            printData();
        }
        
        //printf("current dip is %d\n", dipCount);
        //reset dip to 0 per second
        pthread_mutex_lock(&dipMutex);
        {
            dipCount = 0;
        }
        pthread_mutex_unlock(&dipMutex);
    
    }

    free(current_history);
    pthread_exit(NULL);
    return NULL;

}

int getDipCount()
{
	int count = 0;
	pthread_mutex_lock(&dipMutex);
    {
        count = dipCount;
    }
	pthread_mutex_unlock(&dipMutex);
	return count;
}

void printData()
{
    
    int numOfSamples;
    double *current_history = Sampler_getHistory(&numOfSamples);
    int POT_value = getPotVoltageReading();
    int frequency = getPotHz();
    double avg = Sampler_getAverageReading();
    int dipTempCount = getDipCount();

    //Line1
    //printf("#Smpl/s =  %d    POT @ %d => 20Hz   avg = %.3fV    dips =   %d\n", numOfSamples, POT_value, avg, dipTempCount);
    
    if(numOfSamples > 0)
    {
        //Line1
        printf("#Smpl/s =  %d    POT @ %d => %dHz   avg = %.3fV    dips =   %d    Smpl ms[ %.3f,  %.3f] avg %.3f/%d\n", numOfSamples, POT_value, frequency, avg, dipTempCount, pStats.minPeriodInMs, pStats.maxPeriodInMs, pStats.avgPeriodInMs, pStats.numSamples);
    
        int disPlayCount = 0;
        //Line2
        int interval = numOfSamples / 10;
        for (int i = 0; i < numOfSamples && disPlayCount < 10; i += interval)
        {
            printf("  %d:%.3f ", i, current_history[i]);
            disPlayCount++;
        }
        printf("\n");
    }

    free(current_history);

}
