//PWM.h PWM an LeD

#define PWM_FILE_ENABLE "/dev/bone/pwm/0/b/enable"
#define PWM_FILE_PERIOD "/dev/bone/pwm/0/b/period"
#define PWM_FILE_DUTYCYCLE "/dev/bone/pwm/0/b/duty_cycle"

#ifndef _PWM_H_
#define _PWM_H_

void PWM_init();
void PWM_wait();
void PWM_cleanup(void);

void *PWMLEDThread(void *args);
int getPotHz();
int setPWMFrequency(int frequency);

int getPotVoltageReading(); //raw value for voltage0

int runCommand(char* command);
void writeTOFile(char *path, long value);

#endif