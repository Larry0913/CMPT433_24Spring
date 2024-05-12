#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <time.h> 

#include "audioGeneration.h"
#include "audioMixer_template.h"
#include "threadController.h"
#include "periodTimer.h"
#include "accelerometer.h"

static pthread_t audioGen_id;
static int default_bpm = 120;
static int mode = 1;

pthread_mutex_t modeMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t bpmMutex = PTHREAD_MUTEX_INITIALIZER;

wavedata_t baseDrum1;
wavedata_t hiHat1;
wavedata_t snare1;

wavedata_t baseDrum2;
wavedata_t hiHat2;
wavedata_t snare2;

void Audio_init()
{
    AudioMixer_init();

    AudioMixer_readWaveFileIntoMemory(baseDrumhard, &baseDrum1);
    AudioMixer_readWaveFileIntoMemory(hiHatsoft, &hiHat1);
    AudioMixer_readWaveFileIntoMemory(snaresoft, &snare1);

    AudioMixer_readWaveFileIntoMemory(baseDrumsoft, &baseDrum2);
    AudioMixer_readWaveFileIntoMemory(hiHathard, &hiHat2);
    AudioMixer_readWaveFileIntoMemory(snarehard, &snare2);

    pthread_create(&audioGen_id, NULL, audioGenThread, NULL);
}

void Audio_wait()
{
    pthread_join(audioGen_id, NULL);
}

void Audio_cleanup(void)
{

    AudioMixer_freeWaveFileData(&baseDrum1);
    AudioMixer_freeWaveFileData(&hiHat1);
    AudioMixer_freeWaveFileData(&snare1);

    AudioMixer_freeWaveFileData(&baseDrum2);
    AudioMixer_freeWaveFileData(&hiHat2);
    AudioMixer_freeWaveFileData(&snare2);

    AudioMixer_cleanup();
    
    pthread_mutex_destroy(&modeMutex);
    pthread_mutex_destroy(&bpmMutex);
}

void *audioGenThread(void *args)
{
    (void)args;
    int16_t *accel;

    while (running_flag)
    {
        accel = getAccel();
        if (accel[0] == -1 || accel[0] == 1)
        {
            //printf("snare\n");
            playSound(2);
        }
        else if (accel[1] == -1 || accel[1] == 1)
        {
            //printf("hihat\n");
            playSound(1);
        }
        else if (accel[2] == -1 || accel[2] == 1)
        {
            //printf("base\n");
            playSound(0);
        }
        else if (mode == 1)
        {
            //printf("rock\n");
            rock();
        }
        else if (mode == 2)
        {
            //printf("customize\n");
            customize();
        }
        else if (mode == 0) 
        {
            //printf("no beat\n");
            noBeat();
        }
    }
    pthread_exit(NULL);
    return NULL;
}

long long HalfBeatTime()
{
    long long time = (60.0 / default_bpm / 2.0) * 1000;
    return time;
}

void setMode(int setMode)
{
    pthread_mutex_lock(&modeMutex);
    {
        mode = setMode;
    }
    pthread_mutex_unlock(&modeMutex);
}

int getMode()
{
    return mode;
}

void setBpmValue(int newBpm)
{
    if (newBpm < 40)
    {
        printf("BPM must be greater than 40\n");
        return;
    }
    else if (newBpm > 300)
    {
        printf("BPM must be less than 300\n");
        return;
    }
    else
    {
        pthread_mutex_lock(&bpmMutex);
        {
            default_bpm = newBpm;
        }
        pthread_mutex_unlock(&bpmMutex);
        
    }
}

int getBpmValue()
{
    return default_bpm;
}

void noBeat()
{
    return;
}

void rock()
{
    AudioMixer_queueSound(&hiHat1);
    AudioMixer_queueSound(&baseDrum1);
    sleepForMs(HalfBeatTime());

    AudioMixer_queueSound(&hiHat1);
    sleepForMs(HalfBeatTime());

    AudioMixer_queueSound(&hiHat1);
    AudioMixer_queueSound(&snare1);
    sleepForMs(HalfBeatTime());

    AudioMixer_queueSound(&hiHat1);
    sleepForMs(HalfBeatTime());

    AudioMixer_queueSound(&hiHat1);
    AudioMixer_queueSound(&baseDrum1);
    sleepForMs(HalfBeatTime());

    AudioMixer_queueSound(&hiHat1);
    sleepForMs(HalfBeatTime());

    AudioMixer_queueSound(&hiHat1);
    AudioMixer_queueSound(&snare1);
    sleepForMs(HalfBeatTime());

    AudioMixer_queueSound(&hiHat1);
    sleepForMs(HalfBeatTime());
}

void customize()
{
    AudioMixer_queueSound(&hiHat2);
    AudioMixer_queueSound(&baseDrum2);
    sleepForMs(HalfBeatTime());

    AudioMixer_queueSound(&hiHat2);
    sleepForMs(HalfBeatTime());

    AudioMixer_queueSound(&hiHat2);
    AudioMixer_queueSound(&snare2);
    sleepForMs(HalfBeatTime());

    AudioMixer_queueSound(&hiHat2);
    sleepForMs(HalfBeatTime());

    AudioMixer_queueSound(&hiHat2);
    AudioMixer_queueSound(&baseDrum2);
    sleepForMs(HalfBeatTime());

    AudioMixer_queueSound(&hiHat2);
    sleepForMs(HalfBeatTime());

    AudioMixer_queueSound(&hiHat2);
    AudioMixer_queueSound(&snare2);
    sleepForMs(HalfBeatTime());

    AudioMixer_queueSound(&hiHat2);
    sleepForMs(HalfBeatTime());
}

void playSound(int number){
	if(number == 0){
        AudioMixer_queueSound(&baseDrum1);
        sleepForMs(HalfBeatTime());
    }
    else if(number == 1){
        AudioMixer_queueSound(&hiHat1);
        sleepForMs(HalfBeatTime());
    }
    else if(number == 2){
        AudioMixer_queueSound(&snare1);
        sleepForMs(HalfBeatTime());
    }   
}



