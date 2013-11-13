#include "pwm.h"

int8 Pwm_initialize(uint32 freq,float duty, uint8 ch)
{
    PWM_ENABLE_POWER();
    PWM_SET_CORE_CLK();
    
    PWM_PR(0);                      // set Prescaler to 0

    switch (ch)
    {
        case 0u: PWM_ENABLE_PWM1_1();break;  //Enable PWM1.1
        case 1u: PWM_ENABLE_PWM1_2();break;  //Enable PWM1.2
        case 2u: PWM_ENABLE_PWM1_3();break;  //Enable PWM1.3
        case 3u: PWM_ENABLE_PWM1_4();break;  //Enable PWM1.4
        case 4u: PWM_ENABLE_PWM1_5();break;  //Enable PWM1.5
        case 5u: PWM_ENABLE_PWM1_6();break;  //Enable PWM1.6
    
        default: break;
    }

    PWM1_ENABLE_LATCH(0);
    PWM1_ENABLE_LATCH(1);

    PWM1_SET_MR0(freq);
    PWM1_SET_MR1(freq);

    PWM1_START;

    return (int8)(0);
}

inline void Pwm_start(uint8 ch)
{
    PWM1_ENABLE_OUTPUT(ch);
    return;
}

inline void Pwm_stop(uint8 ch)
{
    PWM1_SET_0;                 //funtioniert nur für alle ausgänge auf einmal oder??
    PWM1_DISABLE_OUTPUT(ch);
    return;
}

inline void Pwm_toggle(uint8 ch)
{
    PWM1_TOGGLE_OUTPUT(ch);
    return;
}
