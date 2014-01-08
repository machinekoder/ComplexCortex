/**
 * @file switch.h
 * @author Alexander Rössler
 * @brief Switch Library, Library to control enable switches
 * @date 16-11-2013
 */
/** @defgroup Switch Switch Library
 * @ingroup Device
 * @{
 */
#pragma once

#ifndef SWITCH_MAX_COUNT
#define SWITCH_MAX_COUNT 10u
#endif

#include <types.h>
#include <gpio.h>
#include <pincon.h>

typedef enum {
    Switch_Type_HighActive = 1u,
    Switch_Type_LowActive = 0u
} Switch_Type;

typedef enum {
    Switch1 = 0u,
    Switch2 = 1u,
    Switch3 = 2u,
    Switch4 = 3u,
    Switch5 = 4u,
    Switch6 = 5u,
    Switch7 = 6u,
    Switch8 = 7u,
    Switch9 = 8u,
    Switch10 = 9u
} Switch;

/** Initializes the Switch library
 *  Must be executed before the Switch_initializeSwitch functions
 */
void Switch_initialize(void);

/** Initializes one switch
 *  @param id           Stands for the name of the Switches
 *  @param port         Port of the switch
 *  @param pin          Pin of the switch
 *  @param lowActive    Is the switch low active or not?
 *  @param pinMode      The mode the pin should be initialized in.
 *  @param openDrain    Defines wether the switch should be used in OpenDrain mode or not.
 */
void Switch_initializeSwitch(Switch id, uint8 port, uint8 pin, Switch_Type type, Pin_Mode pinMode, Pin_OpenDrain openDrain);

/** Turns a switch on
 *  @param id Id of the Switch
 */
void Switch_on(Switch id);

/** Turns a switch off
 *  @param id Id of the Switch
 */
void Switch_off(Switch id);

/** Sets a switch to on or off
 *  @param id Id of the Switch
 *  @param value On=1 Off=0
 */
void Switch_set(Switch id, uint8 value);

/**
 * @}
 */
