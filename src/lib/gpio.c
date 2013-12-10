#include "gpio.h"

static void (* functionPointers0[GPIO0_INT_PIN_COUNT])(void);
static void (* functionPointers2[GPIO2_INT_PIN_COUNT])(void);

inline void Gpio_setDirection(uint8 port, uint8 pin, Gpio_Direction direction)
{
    switch (port)
    {
        case 0u: GPIO0_CLEAR_DIR(pin);
                GPIO0_SET_DIR(direction,pin);
                return;
        case 1u: GPIO1_CLEAR_DIR(pin);
                GPIO1_SET_DIR(direction,pin);
                return;
        case 2u: GPIO2_CLEAR_DIR(pin);
                GPIO2_SET_DIR(direction,pin);
                return;
        case 3u: GPIO3_CLEAR_DIR(pin);
                GPIO3_SET_DIR(direction,pin);
                return;
        case 4u: GPIO4_CLEAR_DIR(pin);
                GPIO4_SET_DIR(direction,pin);
                return;
        default: return;
    }
    return;
}

inline uint8 Gpio_read(uint8 port, uint8 pin)
{
    switch (port)
    {
        case 0u: return GPIO0_READ(pin);
        case 1u: return GPIO1_READ(pin);
        case 2u: return GPIO2_READ(pin);
        case 3u: return GPIO3_READ(pin);
        case 4u: return GPIO3_READ(pin);
        default: return 0u;
    }
    return 0u;
}

inline void Gpio_write(uint8 port, uint8 pin, uint8 value)
{
    switch (port)
    {
        case 0u: (value ? (GPIO0_SET(pin)) : (GPIO0_CLEAR(pin)));
                return;
        case 1u: (value ? (GPIO1_SET(pin)) : (GPIO1_CLEAR(pin)));
                return;
        case 2u: (value ? (GPIO2_SET(pin)) : (GPIO2_CLEAR(pin)));
                return;
        case 3u: (value ? (GPIO3_SET(pin)) : (GPIO3_CLEAR(pin)));
                return;
        case 4u: (value ? (GPIO4_SET(pin)) : (GPIO4_CLEAR(pin)));
                return;
        default: return;
    }
    return;
}

inline void Gpio_set(uint8 port, uint8 pin)
{
    switch (port)
    {
        case 0u: GPIO0_SET(pin);
                return;
        case 1u: GPIO1_SET(pin);
                return;
        case 2u: GPIO2_SET(pin);
                return;
        case 3u: GPIO3_SET(pin);
                return;
        case 4u: GPIO4_SET(pin);
                return;
        default: return;
    }
    return;
}

inline void Gpio_clear(uint8 port, uint8 pin)
{
    switch (port)
    {
        case 0u: GPIO0_CLEAR(pin);
                return;
        case 1u: GPIO1_CLEAR(pin);
                return;
        case 2u: GPIO2_CLEAR(pin);
                return;
        case 3u: GPIO3_CLEAR(pin);
                return;
        case 4u: GPIO4_CLEAR(pin);
                return;
        default: return;
    }
    return;
}

inline void Gpio_toggle(uint8 port, uint8 pin)
{
    switch (port)
    {
        case 0u: (GPIO0_READ(pin) ? (GPIO0_CLEAR(pin)) : (GPIO0_SET(pin)));
                return;
        case 1u: (GPIO1_READ(pin) ? (GPIO1_CLEAR(pin)) : (GPIO1_SET(pin)));
                return;
        case 2u: (GPIO2_READ(pin) ? (GPIO2_CLEAR(pin)) : (GPIO2_SET(pin)));
                return;
        case 3u: (GPIO3_READ(pin) ? (GPIO3_CLEAR(pin)) : (GPIO3_SET(pin)));
                return;
        case 4u: (GPIO4_READ(pin) ? (GPIO4_CLEAR(pin)) : (GPIO4_SET(pin)));
                return;
        default: return;
    }
    return;
}

void Gpio_enableInterrupt(uint8 port, uint8 pin, Gpio_Interrupt type, void (* func)(void))
{
    switch (port)
    {
        case 0u: if ((type == Gpio_Interrupt_RisingEdge) || (type == Gpio_Interrupt_FallingAndRisingEdge))
                {
                   GPIO0_ENABLE_IRQ_RISING_EDGE(pin);
                }
                if ((type == Gpio_Interrupt_FallingEdge) || (type == Gpio_Interrupt_FallingAndRisingEdge))
                {
                    GPIO0_ENABLE_IRQ_FALLING_EDGE(pin);
                }
                GPIO0_CLEAR_IRQ(pin);
                functionPointers0[pin] = func;
                break;
        case 2u: if ((type == Gpio_Interrupt_RisingEdge) || (type == Gpio_Interrupt_FallingAndRisingEdge))
                {
                    GPIO2_ENABLE_IRQ_RISING_EDGE(pin);
                }
                if ((type == Gpio_Interrupt_FallingEdge) || (type == Gpio_Interrupt_FallingAndRisingEdge))
                {
                    GPIO2_ENABLE_IRQ_FALLING_EDGE(pin);
                }
                GPIO2_CLEAR_IRQ(pin);
                functionPointers2[pin] = func;
                break;
        default: return;
    }
    GPIO_ENABLE_IRQS();
    return;
}

void Gpio_disableInterrupt(uint8 port, uint8 pin)
{
    switch (port)
    {
        case 0u: GPIO0_DISABLE_IRQ_RISING_EDGE(pin);
                GPIO0_DISABLE_IRQ_FALLING_EDGE(pin);
                GPIO0_CLEAR_IRQ(pin);
                return;
        case 2u: GPIO2_DISABLE_IRQ_RISING_EDGE(pin);
                GPIO2_DISABLE_IRQ_FALLING_EDGE(pin);
                GPIO2_CLEAR_IRQ(pin);
                return;
        default: return;
    }
    return;
}

#if (GPIO_IRQ_ENABLED == 1)
void GPIO_IRQHANDLER()
{
    uint8 i;
    
    if (GPIO0_IRQ_PENDING())
    {
        for (i = 0u; i < GPIO0_INT_PIN_COUNT; i++)
        {
            if ((GPIO0_RISING_IRQ_PENDING(i)) || (GPIO0_FALLING_IRQ_PENDING(i)))
            {
                GPIO0_CLEAR_IRQ(i);
                (*functionPointers0[i])();
            }
        }

    }
    else if (GPIO2_IRQ_PENDING())
    {
        for (i = 0u; i < GPIO2_INT_PIN_COUNT; i++)
        {
            if ((GPIO2_RISING_IRQ_PENDING(i)) || (GPIO2_FALLING_IRQ_PENDING(i)))
            {
                GPIO2_CLEAR_IRQ(i);
                (*functionPointers2[i])();
            }
        }
    }
}
#endif
