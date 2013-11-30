/**
 * @file pwm.h
 * @author Johannes Wimmer & Alexander Rössler
 * @brief PWM Library
 * @date 16-10-2012
 */
/** @defgroup PWM PWM (Pulse Width Modulation)
 * @ingroup Peripheral
 * @{
 */
#pragma once

#include <types.h>
#include <pwmDriver.h>

typedef enum {
    Pwm0 = 0u,
    Pwm1 = 1u,
    Pwm2 = 2u,
    Pwm3 = 3u,
    Pwm4 = 4u,
    Pwm5 = 5u
} Pwm;

/** Initializes the pulsewith generator.
 *@param freq  Is the frequency for the gernerated signal.
 *@param duty  Is the duty-cicle for the signal.
 *@param ch   Says which channel(1-6) should be taken.
 */
int8 Pwm_initialize(uint32 freq,float duty, uint8 ch);

/** Startes the PWM mode and the counter.
 *@param ch Says which channel(1-5) should be taken.
 */
void Pwm_start(uint8 ch);
/** Stop the PWM and set signal to 0.
 *@param ch Says which channel(1-5) should be taken.
 */
void Pwm_stop(uint8 ch);
/** Togles the signal-output.
 *@param ch Says which channel(1-5) should be taken.
 */
void Pwm_toggle(uint8 ch);

/**
 * @}
 */
