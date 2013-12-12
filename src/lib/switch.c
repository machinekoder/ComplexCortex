#include "switch.h"

typedef struct {
    uint8 id;
    uint8 port;
    uint8 pin;
    uint8  type;
} SwitchConfig;

static SwitchConfig   switches[SWITCH_MAX_COUNT];
static uint8          switchCount;

void Switch_initialize(void)
{
    /* init variables */
    switchCount = 0u;
}

void Switch_initializeSwitch(Switch id, uint8 port, uint8 pin, Switch_Type type, Pin_Mode pinMode, Pin_OpenDrain openDrain)
{
    Gpio_setDirection(port, pin, Gpio_Direction_Output);
    Pin_setMode(port, pin, pinMode);
    Pin_setOpenDrainMode(port, pin, openDrain);
    
    switches[id].id     = id;
    switches[id].port   = port;
    switches[id].pin    = pin;
    switches[id].type   = (uint8)type;
    
    switchCount++;
}

void Switch_on(Switch id)
{
    if (switches[id].type == Switch_Type_HighActive)
    {
        Gpio_set(switches[id].port, switches[id].pin);
    }
    else
    {
        Gpio_clear(switches[id].port, switches[id].pin);
    }
}

void Switch_off(Switch id)
{
    if (switches[id].type == Switch_Type_HighActive)
    {
        Gpio_clear(switches[id].port, switches[id].pin);
    }
    else
    {
        Gpio_set(switches[id].port, switches[id].pin);
    }
}
