//PWM.h Linux PWM: Zen Cape Buzzer

#define PWM_FILE_ENABLE "/dev/bone/pwm/0/a/enable"
#define PWM_FILE_PERIOD "/dev/bone/pwm/0/a/period"
#define PWM_FILE_DUTYCYCLE "/dev/bone/pwm/0/a/duty_cycle"

// Frequency values for notes, in hertz
#define A4  440.00
#define B4  493.88
#define C4  261.63
#define E4  329.63
#define F4  349.23
#define G4  392.00
#define C5  523.25

#ifndef _PWM_H_
#define _PWM_H_

typedef enum {
    SOUND_NONE,
    SOUND_HIT,
    SOUND_MISS
} SoundType;

void PWM_init();
void PWM_wait();
void PWM_cleanup(void);

void *PWMBuzzerThread(void *args);

void turnOffSound();

void setHitSoundPlay();
void setMissSoundPlay();


#endif