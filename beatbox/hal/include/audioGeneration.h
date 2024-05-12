// Generate audio in real-time from a C/C++/Rust program using the ALSA API1, and play 
// that audio through the Zen cape's head-phone output.
// Generate at least the following three different drum beats (“modes”). You may optionally 
// generate more.
// Control the beat's tempo (in beats-per-minute) in range [40, 300] BPM (inclusive); default 
// 120 BPM. See next section for how to control each of these.
// Control the output volume in range [0, 100] (inclusive), default 80.
// Play additional drum sounds when needed (i.e., have functions that other modules can call 
// to playback drum sounds when needed).
// Audio playback must be smooth, consistent, and with low latency (low delay between 
// asking to play a sound and the sound playing).

#ifndef _AUDIO_GENERATION_H_
#define _AUDIO_GENERATION_H_

#define baseDrumhard "beatbox-wav-files/100051__menegass__gui-drum-bd-hard.wav"
#define hiHatsoft "beatbox-wav-files/100053__menegass__gui-drum-cc.wav"
#define snaresoft "beatbox-wav-files/100059__menegass__gui-drum-snare-soft.wav"

#define baseDrumsoft "beatbox-wav-files/100052__menegass__gui-drum-bd-soft.wav"
#define hiHathard "beatbox-wav-files/100054__menegass__gui-drum-ch.wav"
#define snarehard "beatbox-wav-files/100058__menegass__gui-drum-snare-hard.wav"

//configure and init
void Audio_init();

//join thread
void Audio_wait();

//clean up malloc variable
void Audio_cleanup(void);

//thread function
void *audioGenThread(void *args);

//sleep for half beat
long long HalfBeatTime();

//set mode
void setMode(int setMode);

//get mode
int getMode();

//get bpm
int getBpmValue();

//set bpm
void setBpmValue(int newBpm);

//default mode: nobeat
void noBeat();

//play standard rock
void rock();

//customize
void customize();

//play sing sound
void playSound(int number);



#endif