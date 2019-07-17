

/*
Copyright (c) 2019, All rights reserved.

File         : timer.c
Description  : 

Contact      : fervour.cool@163.com

Revision     : 2019-07 
Description  : Primary released

*/



#include "timer.h"
#include <time.h>
#define	TIMER_CONTINUE	0
#define	TIMER_REMOVE	1
extern void taskPolling(void *data);
typedef uint64_t timerTick_t;

/*
    returns true if the tick a is after tick b.
*/
#define timerTickAfter(a, b) ((int64_t)(b) - (int64_t)(a) < 0)


struct timer{
    const char *name;// for debug purpose
    void *data;
    timerTick_t expired;
    unsigned long period;
    timerFunction_t function;
    timerOnExit_t on_exit;
    int reason;
    struct timer *next;
};

static struct timer * s_timerHead = NULL;
static int s_timerNumber = 0;

static timerTick_t timerTick(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (timerTick_t)(ts.tv_sec * 1000 + ts.tv_nsec/(1000*1000));
}

/*
    return values:
        NULL - failed to add a timer
        other values - the handle of the timer
*/

timerHandle_t timerAdd(
    const char *name, /* Timer name */
    unsigned long period_ms, /* common period in millisecond */
    unsigned long delay_ms, /* if set to 0, the timer function will call at once, other wait delay ms to call the first time */
    timerFunction_t function, 
    timerOnExit_t on_exit, 
    void *data
    )
{
    struct timer *p;

    if (s_timerNumber > TIMER_MAX_NUM)
    {
        printf( "Trying to add timer(%s) failed, too many timers added", name);
        return NULL;
    }
    
    p = (struct timer *)malloc(sizeof(struct timer));
    if (p == NULL)
    {
        printf( "malloc(%d)", sizeof(struct timer));
        return NULL;
    }

    p->name = name;
    p->data = data;
    p->period = period_ms;
    p->function = function;
    p->on_exit = on_exit;
    p->reason = REASON_DEFAULT;
    p->expired = timerTick() + delay_ms;

    p->next = s_timerHead;
    s_timerHead = p;
    s_timerNumber ++;
    
    //printf("Timer(%s) registered at %lld, expired:%lld\n", p->name, timerTick(), p->expired);
    
    return p;
}


static struct timer *timerGet(timerHandle_t handle)
{
    struct timer *p;
    p = s_timerHead;
    while(p)
    {
        if (p == handle)
        {
            return p;
        }
        p  = p->next;
    }
    return NULL;
}


/*
    return value:
    1 - the timer is running
    0 - the timer is not in queue.
*/
int timerTest(timerHandle_t handle)
{
    return timerGet(handle) ? 1 : 0;
}


void timerRemove(timerHandle_t handle)
{
    struct timer *p, *prev = NULL;

    p = s_timerHead;
    while(p)
    {        
        if (p == handle)
        {
            if (p->on_exit)
            {
                p->on_exit(p->data);
            }

            if (prev)
            {
                prev->next = p->next;
            }
            else 
            {
                s_timerHead = p->next;
            }
            
            s_timerNumber --;

            printf("Remove Timer(%s)", p->name);
            free(p); 

            return ;
        }
        else 
        {
            prev = p;
            p = p->next;
        }
    }
}


void timerReactivate(timerHandle_t handle, int msec, int reason)
{
    struct timer *p;
    p = timerGet(handle);
    if (p)
    {
        p->reason = reason;
        p->expired = timerTick() + msec;
        printf("Timer(%s) is scheduled in %d ms with reason(%d)", p->name, msec, reason);
    }
}


int timerUpdatePeriod(timerHandle_t handle, unsigned long ms)
{
    struct timer *p;

    if (ms == 0){
        return 0;
    }
    
    p = timerGet(handle);
    if (p){
        p->period = ms;
        printf("Timer(%s) update period to %lu ms", p->name, p->period);
        return 1;
    }
    return 0;
}


int timerSchedule(void)
{
    int ret, called;
    timerTick_t tick; 
    struct timer *p, *prev = NULL;
    
    p = s_timerHead;
   // tick = timerTick();
    called = 0;

   //printf( "in timer");
    
    while(p)
    {
        tick = timerTick();

       //printf( "[%d] test Timer(%s) [%lld<->%lld]", s_timerNumber, p->name, tick, p->expired);
        if (timerTickAfter(tick, p->expired))
        {
            if (p->function)
            {
                //printf("[%d]Call Timer(%s) at %lld", s_timerNumber, p->name, tick);
                ret = p->function(p->data, p->reason);
               // printf("[%d]Call Timer(%s) return %d, used %lld ms\n", s_timerNumber, p->name, ret, timerTick() - tick);
                called ++;
                
                // reset reason to default:
                p->reason = REASON_DEFAULT;

                if (ret == TIMER_CONTINUE)
                {
                    p->expired = timerTick() + p->period;
                }
                else 
                {
                    if (ret != TIMER_REMOVE)
                    {
                        printf( "Unknown return value(%d) from function of Timer(%s), disable it", ret, p->name);
                    }
                    
                    if (p->on_exit)
                    {
                        p->on_exit(p->data);
                    }
                    
                    if (prev)
                    {
                        prev->next = p->next;
                    }
                    else 
                    {
                        s_timerHead = p->next;
                    }
                    
                    s_timerNumber --;
                    
                    printf("Remove Timer(%s)", p->name);
                    free(p);

                    p = prev ? prev->next : s_timerHead;
                    
                    continue;
                }
            }
        }
        prev = p;
        p = p->next;
    }
    
    return called;
}


void timerPolling(void *data)
{
    timerSchedule();
}


void timerFreeAll(void)
{
    struct timer *p, *next = NULL;

    p = s_timerHead;
    while(p)
    {        
        next = p->next;
        printf( "Remove Timer(%s)", p->name);
        free(p); 
        p = next;    
    }    
    s_timerHead = NULL;
}
int timerDemo(void *data, int reason )
{
	 printf("---->timer runing:<----\n");
	 return TIMER_CONTINUE;  
}


void main()
{

threadAddPollingFunction("sys.timer",NULL, timerPolling);
//threadAddPollingFunction("sys.task", NULL, taskPolling);
timerAdd("timer.demo", 6*1000, 5*1000, timerDemo, NULL, NULL);

threadSchedule(1000);
 
//taskFreeAll();
timerFreeAll();
threadFreeAll();

}

