#include "timeout.h"

#define TIMEOUT_TIMER_KHZ       1u
#define TIMEOUT_TIMER_INTERVAL  ~(uint32)0u

static volatile uint8 globalTimerId;

int8 Timeout_initialize(Timer timerId)
{
    
    if (timerId == TimerRandom)
    {
        timerId = Timer_initializeRandom(TIMEOUT_TIMER_KHZ, TIMEOUT_TIMER_INTERVAL);
    }
    else
    {
        if (timerId == Timer0)
        {
            if (Timer_initialize(Timer0, TIMEOUT_TIMER_KHZ, TIMEOUT_TIMER_INTERVAL) == (int8)(-1))
            {
                return (int8)(-1);
            }
        }
        else if (timerId == Timer1)
        {
            if (Timer_initialize(Timer1, TIMEOUT_TIMER_KHZ, TIMEOUT_TIMER_INTERVAL) == (int8)(-1))
            {
                return (int8)(-1);
            }
        }
        else if (timerId == Timer2)
        {
            if (Timer_initialize(Timer2, TIMEOUT_TIMER_KHZ, TIMEOUT_TIMER_INTERVAL) == (int8)(-1))
            {
                return (int8)(-1);
            }
        }
        else if (timerId == Timer3)
        {
            if (Timer_initialize(Timer3, TIMEOUT_TIMER_KHZ, TIMEOUT_TIMER_INTERVAL) == (int8)(-1))
            {
                return (int8)(-1);
            }
        }
        else
        {
            timerId = (int8)(-1);
        }
    }
    
    if (timerId == (int8)(-1))
        return (int8)(-1);
    
    globalTimerId = timerId;
    Timer_start(timerId);
    
    return (int8)(0);
}

uint32 Timeout_msecs(void)
{
    return Timer_counterValue(globalTimerId);
}

void Timeout_reset(void)
{
    Timer_reset(globalTimerId);
}
