/**
 * @file button.h
 * @author Johannes Wimmer & Alexander Rössler
 * @brief Library for controlling the IAP functions
 * @date 25-01-2013
 */

/** @defgroup Button Button Library
 * @ingroup Device
 * @{
 */
#pragma once

#define BUTTON_BUFFER_SIZE 20u
#define BUTTON_MAX_COUNT 20u

#include <types.h>
#include <timer.h>
#include <gpio.h>
#include <pincon.h>
#include <circularbuffer.h>

typedef enum {
    Button_Type_HighActive = 0u,
    Button_Type_LowActive = 1u
} Button_Type;

typedef struct {
    uint8 id;
    uint8 count;
} ButtonValue;

typedef enum {
    Button1 = 0u,
    Button2 = 1u,
    Button3 = 2u,
    Button4 = 3u,
    Button5 = 4u,
    Button6 = 5u,
    Button7 = 6u,
    Button8 = 7u,
    Button9 = 8u,
    Button10 = 9u,
} Button;

/** Initializes the Button Pin und Ports.
 * @param khz Is the frequency in wich values are checked.
 * @param sampleInterval interval in µs the button should be sampled
 * @param timeoutInterval time that should be wait until a new press is recognized
 * @param timer Timer to use for button functions
 */
int8 Button_initialize(uint32 khz, uint32 sampleInterval, uint32 timeoutInterval, Timer timer);

int8 Button_initialize2(uint32 sampleInterval, uint32 timeoutInterval);

/** Initializes a button.
 * @param id stands for the name of the Buttons(B1,B2,B3,..)
 * @param port is the port of the button
 * @param pin is the pin of the button
 * @param type set "0" for low_active, or" 1" for high_active
 * */
void Button_initializeButton(uint8 id, uint8 port, uint8 pin, Button_Type type);

/** Get button presses
 * @param value a pointer to the value where the button data should be stored at
 * @return -1 if no new button press is available, 0 if a new values is available
 */
int8 Button_getPress(ButtonValue *value);

uint8 Button_read(uint8 id);

void Button_task();

/**
 * @}
 */
