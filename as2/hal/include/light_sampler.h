// sampler.h
// Module to sample light levels in the background (uses a thread).
//
// It continuously samples the light level, and stores it internally.
// It provides access to the samples it recorded during the _previous_
// complete second.
//
// The application will do a number of actions each second which must
// be synchronized (such as computing dips and printing to the screen).
// To make easy to work with the data, the app must call 
// Sampler_moveCurrentDataToHistory() each second to trigger this 
// module to move the current samples into the history.

#include <pthread.h>
#define A2D_FILE_VOLTAGE1 "/sys/bus/iio/devices/iio:device0/in_voltage1_raw"
#define A2D_FILE_VOLTAGE0 "/sys/bus/iio/devices/iio:device0/in_voltage0_raw"
#define A2D_VOLTAGE_REF_V 1.8
#define A2D_MAX_READING 4095
#define BUFFER_MAX_SIZE 1000
#ifndef _LIGHT_SAMPLER_H_
#define _LIGHT_SAMPLER_H_

// Begin/end the background thread which samples light levels.
void Sampler_init(void);
void Sampler_cleanup(void);
// Must be called once every 1s.
// Moves the samples that it has been collecting this second into
// the history, which makes the samples available for reads (below).
void Sampler_moveCurrentDataToHistory(int *buffer_index, double *buffer);
// Get the number of samples collected during the previous complete second.
int Sampler_getHistorySize(void);
// Get a copy of the samples in the sample history.
// Returns a newly allocated array and sets `size` to be the 
// number of elements in the returned array (output-only parameter).
// The calling code must call free() on the returned pointer.
// Note: It provides both data and size to ensure consistency.
double* Sampler_getHistory(int *size);
// Get the average light level (not tied to the history).
double Sampler_getAverageReading(void);
// Get the total number of light level samples taken so far.
long long Sampler_getNumSamplesTaken(void);

#endif
