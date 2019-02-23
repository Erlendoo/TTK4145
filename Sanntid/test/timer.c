#include <stdio.h>
#include <sys/time.h>

//Borrowed almost verbatim from previous project

double timer_getTime(void){
    struct timeval time;
    gettimeofday(&time, NULL);
    return (double)time.tv_sec + (double)time.tv_usec * .000001;
}

static  double          timerEndTime;
static  int             timerActive;

void timer_start(double duration){
    timerEndTime    = timer_getTime() + duration;
    timerActive     = 1;
}

void timer_stop(void){
    timerActive = 0;
}

int timer_timedOut(void){
    return (timerActive  &&  timer_getTime() > timerEndTime);
}