#include "adc.h"

#define ADC_MAX_COUNT 8u

static volatile uint8 adcBurstEnabled;
static volatile uint32 adcMask;
static volatile uint32 adcValue[ADC_MAX_COUNT];
static volatile uint8 adcIntDone;

int8 Adc_initialize(uint32 clk, Adc_BurstMode burstMode)
{
    adcIntDone = 0u;
    adcMask = 0u;
    adcBurstEnabled = burstMode;
    
    ADC_ENABLE_POWER();           // Power on the ADC
    ADC_SET_CORE_CLK();           // Set the ADC core clock
    
    ADC_CLEAR_PINSEL();           // clear pin selection bits
    
    ADC_SET_CLK(clk);           // Set ADC clock
    ADC_CONFIGURE();            // Configure ADC specific settings
    ADC_ENABLE();               // Enable the ADC
    
    if (adcBurstEnabled == Adc_BurstMode_Enabled)
    {
        ADC_SET_BURSTMODE();
    }

    ADC_ENABLE_IRQ();               // Enable ADC interrupt
 
    if (burstMode == Adc_BurstMode_Enabled)
    {
        ADC_SET_ALL_IRQS_BURST();   // Enable all interrupts
    }
    else 
    {
        ADC_SET_ALL_IRQS();         // Enable all interrupts
    }
    
    return (int8)(0);
}

int8 Adc_enablePin(Adc_Pin pin)
{
    if (pin > (ADC_MAX_COUNT-1u))
    {
        return (int8)(-1);
    }
    
    ADC_SET_PIN(pin);     // set sample pin
    adcValue[pin] = 0u;
    adcMask |= (1u << pin);
    
    return (int8)(0);
}

int8 Adc_disablePin(Adc_Pin pin)
{
    if (pin > (ADC_MAX_COUNT-1u))
    {
        return (int8)(-1);
    }
    
    ADC_CLEAR_PIN(pin);     // clear sample pin
    adcMask &= ~(1u << pin);
    
    return (int8)(0);
}

int8 Adc_deinitialize(void)
{ 
    ADC_DISABLE();          // Disable the ADC
    ADC_DISABLE_POWER();    // Power off the ADC
    ADC_DISABLE_IRQ();      // Disable ADC interrupt
    
    return (int8)(0);
}

int8 Adc_read(Adc_Pin pin, uint16 *value)
{
    if (adcBurstEnabled == 0u)
    {
        adcIntDone = 0u;
        ADC_START();                    // start conversion now
    }
    
    while (adcIntDone != 1u)             // wait until finished
        ;
    
    if (ADC_HAS_OVERRUN(adcValue[pin]))      // overrun error
        return (int8)(-1);
    
    *value = (uint16)ADC_GET_VALUE(adcValue[pin]);
    
    return (int8)(0);
}

void ADC_IRQHandler(void)
{   
    uint32 data;
    
    if (adcMask & (1u << Adc_Pin_0))
    {
        data = ADC_READ_PIN0();
        if (ADC_IS_DONE(data))
        {
            adcValue[Adc_Pin_0] = data;
        }
    }
    if (adcMask & (1u << Adc_Pin_1))
    {
        data = ADC_READ_PIN1();
        if (ADC_IS_DONE(data))
        {
            adcValue[Adc_Pin_1] = data;
        }
    }
    if (adcMask & (1u << Adc_Pin_2))
    {
        data = ADC_READ_PIN2();
        if (ADC_IS_DONE(data))
        {
            adcValue[Adc_Pin_2] = data;
        }
    }
    if (adcMask & (1u << Adc_Pin_3))
    {
        data = ADC_READ_PIN3();
        if (ADC_IS_DONE(data))
        {
            adcValue[Adc_Pin_3] = data;
        }
    }
    if (adcMask & (1u << Adc_Pin_4))
    {
        data = ADC_READ_PIN4();
        if (ADC_IS_DONE(data))
        {
            adcValue[Adc_Pin_4] = data;
        }
    }
    if (adcMask & (1u << Adc_Pin_5))
    {
        data = ADC_READ_PIN5();
        if (ADC_IS_DONE(data))
        {
            adcValue[Adc_Pin_5] = data;
        }
    }
    if (adcMask & (1u << Adc_Pin_6))
    {
        data = ADC_READ_PIN6();
        if (ADC_IS_DONE(data))
        {
            adcValue[Adc_Pin_6] = data;
        }
    }
    if (adcMask & (1u << Adc_Pin_7))
    {
        data = ADC_READ_PIN7();
        if (ADC_IS_DONE(data))
        {
            adcValue[Adc_Pin_7] = data;
        }
    }

    if (!adcBurstEnabled)
    {
        ADC_STOP();     // stop adc
    }
    
    adcIntDone = 1u;
}
