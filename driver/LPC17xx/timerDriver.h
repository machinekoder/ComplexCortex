#ifndef _TIMERDRIVER_H_
#define _TIMERDRIVER_H_

#define SYSTEMCLK SystemCoreClock
#define OPTIMIZE_PRESCALER_US()         (SYSTEMCLK/1000000u)-1u
#define OPTIMIZE_PRESCALER_MS()         (SYSTEMCLK/1000u)-1u

#define PROZESSOR_SLEEP()               LPC_SC->PCON = 0x00u;
#define PROZESSOR_WAIT_FOR_IRQ()        __WFI()

#define LOW_IRQ_PRIORITY                1u
#define HIGH_IRQ_PRIORITY               0u

#define TIMER_ENABLE_POWER(x)                   TIMER_ENABLE_POWER_##x
#define TIMER_SET_CORE_CLK_DIVISOR_1(x)         TIMER_SET_CORE_CLK_DIVISOR_1_##x
#define TIMER_SET_CORE_CLK_DIVISOR_2(x)         TIMER_SET_CORE_CLK_DIVISOR_2_##x
#define TIMER_SET_CORE_CLK_DIVISOR_4(x)         TIMER_SET_CORE_CLK_DIVISOR_4_##x
#define TIMER_SET_CORE_CLK_DIVISOR_8(x)         TIMER_SET_CORE_CLK_DIVISOR_8_##x
#define TIMER_RESET(x)                          LPC_TIM##x->TCR = 0x02u
#define TIMER_START(x)                          LPC_TIM##x->TCR = 0x01u
#define TIMER_STOP(x)                           LPC_TIM##x->TCR = 0x00u
#define TIMER_RUNNING(x)                        TIMER_RUNNING_##x
#define TIMER_SET_PRESCALER(y,x)                LPC_TIM##y->PR = (uint32)(x)
#define TIMER_GET_PRESCALER(x)                  LPC_TIM##x->PR
#define TIMER_SET_KHZ(y,x)                      TIMER_SET_PRESCALER(y,(uint32)((SYSTEMCLK)/(x*1000u))-1u)
#define TIMER_SET_MHZ(y,x)                      TIMER_SET_PRESCALER(y,(uint32)((SYSTEMCLK)/(x*1000000u))-1u)
#define TIMER_RESET_IRQS(x)                     LPC_TIM##x->IR  = 0xFFu
#define TIMER_SET_INTERVAL_MS(y,x)              LPC_TIM##y->MR0 = (uint32)(x*(SYSTEMCLK/1000u/(LPC_TIM##y->PR+1u)))-1u
#define TIMER_SET_INTERVAL_US(y,x)              LPC_TIM##y->MR0 = (uint32)(x*(SYSTEMCLK/1000000u/(LPC_TIM##y->PR+1u)))-1u
#define TIMER_SET_MATCH_REGISTER_0(y,x)         LPC_TIM##y->MR0 = (uint32)(x)
#define TIMER_ENABLE_IRQ(x)                     TIMER_ENABLE_IRQ_##x
#define TIMER_DISABLE_IRQ(x)                    TIMER_DISABLE_IRQ_##x
#define TIMER_SET_IRQ_PRIORITY(y,x)             TIMER_SET_IRQ_PRIORITY_##y(x)
#define TIMER_RESET_AND_IRQ_ON_MATCH(x)         LPC_TIM##x->MCR = (1 << 0) | (1 << 1)
#define TIMER_RESET_STOP_AND_IRQ_ON_MATCH(x)    LPC_TIM##x->MCR = (1 << 0) | (1 << 1) | (1 << 2)
#define TIMER_STOP_AND_IRQ_ON_MATCH(x)          LPC_TIM##x->MCR = (1 << 0) | (1 << 2)
#define TIMER_IRQHANDLER(x)                     TIMER_IRQHANDLER_##x
#define TIMER_COUNTER_VALUE(x)                  LPC_TIM##x->TC

#define TIMER_ENABLE_POWER_0                    LPC_SC->PCONP |= (1 << 1)
#define TIMER_SET_CORE_CLK_DIVISOR_1_0          LPC_SC->PCLKSEL0 &= ~(0b11 << 2); LPC_SC->PCLKSEL0 |= (0b01 << 2)
#define TIMER_SET_CORE_CLK_DIVISOR_2_0          LPC_SC->PCLKSEL0 &= ~(0b11 << 2); LPC_SC->PCLKSEL0 |= (0b10 << 2)
#define TIMER_SET_CORE_CLK_DIVISOR_4_0          LPC_SC->PCLKSEL0 &= ~(0b11 << 2)
#define TIMER_SET_CORE_CLK_DIVISOR_8_0          LPC_SC->PCLKSEL0 |= (0b11 << 2)
#define TIMER_RUNNING_0                         (LPC_TIM0->TCR & 0x01u)  // Here the PCONP should not be checked, but why???
#define TIMER_ENABLE_IRQ_0                      NVIC_EnableIRQ(TIMER0_IRQn)
#define TIMER_DISABLE_IRQ_0                     NVIC_DisableIRQ(TIMER0_IRQn)
#define TIMER_SET_IRQ_PRIORITY_0(x)             NVIC_SetPriority(TIMER0_IRQn,x)
#define TIMER_IRQHANDLER_0                      TIMER0_IRQHandler(void)

#define TIMER_ENABLE_POWER_1                    LPC_SC->PCONP |= (1 << 2)
#define TIMER_SET_CORE_CLK_DIVISOR_1_1          LPC_SC->PCLKSEL0 &= ~(0b11 << 4); LPC_SC->PCLKSEL0 |= (0b01 << 4)
#define TIMER_SET_CORE_CLK_DIVISOR_2_1          LPC_SC->PCLKSEL0 &= ~(0b11 << 4); LPC_SC->PCLKSEL0 |= (0b10 << 4)
#define TIMER_SET_CORE_CLK_DIVISOR_4_1          LPC_SC->PCLKSEL0 &= ~(0b11 << 4)
#define TIMER_SET_CORE_CLK_DIVISOR_8_1          LPC_SC->PCLKSEL0 |= (0b11 << 4)
#define TIMER_RUNNING_1                         (LPC_TIM1->TCR & 0x01u) // Here the PCONP should not be checked, but why???
#define TIMER_ENABLE_IRQ_1                      NVIC_EnableIRQ(TIMER1_IRQn)
#define TIMER_DISABLE_IRQ_1                     NVIC_DisableIRQ(TIMER1_IRQn)
#define TIMER_SET_IRQ_PRIORITY_1(x)             NVIC_SetPriority(TIMER1_IRQn,x)
#define TIMER_IRQHANDLER_1                      TIMER1_IRQHandler(void)

#define TIMER_ENABLE_POWER_2                    LPC_SC->PCONP |= (1 << 22)
#define TIMER_SET_CORE_CLK_DIVISOR_1_2          LPC_SC->PCLKSEL1 &= ~(0b11 << 12); LPC_SC->PCLKSEL1 |= (0b01 << 12)
#define TIMER_SET_CORE_CLK_DIVISOR_2_2          LPC_SC->PCLKSEL1 &= ~(0b11 << 12); LPC_SC->PCLKSEL1 |= (0b10 << 12)
#define TIMER_SET_CORE_CLK_DIVISOR_4_2          LPC_SC->PCLKSEL1 &= ~(0b11 << 12)
#define TIMER_SET_CORE_CLK_DIVISOR_8_2          LPC_SC->PCLKSEL1 |= (0b11 << 12)
#define TIMER_RUNNING_2                         (LPC_SC->PCONP & (1 << 22)) && (LPC_TIM2->TCR & 0x01u)   // Here the PCONP must be checked, but why???
#define TIMER_ENABLE_IRQ_2                      NVIC_EnableIRQ(TIMER2_IRQn)
#define TIMER_DISABLE_IRQ_2                     NVIC_DisableIRQ(TIMER2_IRQn)
#define TIMER_SET_IRQ_PRIORITY_2(x)             NVIC_SetPriority(TIMER2_IRQn,x)
#define TIMER_IRQHANDLER_2                      TIMER2_IRQHandler(void)

#define TIMER_ENABLE_POWER_3                    LPC_SC->PCONP |= (1 << 23)
#define TIMER_SET_CORE_CLK_DIVISOR_1_3          LPC_SC->PCLKSEL1 &= ~(0b11 << 14); LPC_SC->PCLKSEL1 |= (0b01 << 14)
#define TIMER_SET_CORE_CLK_DIVISOR_2_3          LPC_SC->PCLKSEL1 &= ~(0b11 << 14); LPC_SC->PCLKSEL1 |= (0b10 << 14)
#define TIMER_SET_CORE_CLK_DIVISOR_4_3          LPC_SC->PCLKSEL1 &= ~(0b11 << 14)
#define TIMER_SET_CORE_CLK_DIVISOR_8_3          LPC_SC->PCLKSEL1 |= (0b11 << 14)
#define TIMER_RUNNING_3                         (LPC_SC->PCONP & (1 << 23)) && (LPC_TIM3->TCR & 0x01u)
#define TIMER_ENABLE_IRQ_3                      NVIC_EnableIRQ(TIMER3_IRQn)
#define TIMER_DISABLE_IRQ_3                     NVIC_DisableIRQ(TIMER3_IRQn)
#define TIMER_SET_IRQ_PRIORITY_3(x)             NVIC_SetPriority(TIMER3_IRQn,x)
#define TIMER_IRQHANDLER_3                      TIMER3_IRQHandler(void)

/*#define CAPCOM_ENABLE_PIN0()   LPC_PINCON->PINSEL3  |= (0b11 << 20) // ; LPC_PINCON->PINMODE3 |= (0b10 << 20)  // CAP0.0   P1.26
#define CAPCOM_ENABLE_PIN1()   LPC_PINCON->PINSEL3  |= (0b11 <<  4) // ; LPC_PINCON->PINMODE3 |= (0b10 <<  4)  // CAP1.0   P1.18
#define CAPCOM_ENABLE_PIN2()   LPC_PINCON->PINSEL0  |= (0b11 <<  8) // ; LPC_PINCON->PINMODE0 |= (0b10 <<  8)  // CAP2.0   P0.4
#define CAPCOM_ENABLE_PIN3()   LPC_PINCON->PINSEL1  |= (0b11 << 14) // ; LPC_PINCON->PINMODE1 |= (0b10 << 14)  // CAP3.0   P0.23
#define CAPCOM_ENABLE_PIN4()   LPC_PINCON->PINSEL3  |= (0b11 << 22) // ; LPC_PINCON->PINMODE3 |= (0b10 << 22)  // CAP0.1   P1.27
#define CAPCOM_ENABLE_PIN5()   LPC_PINCON->PINSEL3  |= (0b11 <<  6) // ; LPC_PINCON->PINMODE3 |= (0b10 <<  6)  // CAP1.1   P1.19
#define CAPCOM_ENABLE_PIN6()   LPC_PINCON->PINSEL0  |= (0b11 << 10) // ; LPC_PINCON->PINMODE0 |= (0b10 << 10)  // CAP2.1   P0.5
#define CAPCOM_ENABLE_PIN7()   LPC_PINCON->PINSEL1  |= (0b11 << 16) // ; LPC_PINCON->PINMODE1 |= (0b10 << 16)  // CAP3.1   P0.24

#define CAPCOM0_ENABLE_CAP0()  LPC_TIM0->CCR |= (1 << 0) | (1 << 1) | (1 << 2)   //CAP0 on rising and falling edge and interrupt
#define CAPCOM0_ENABLE_CAP1()  LPC_TIM0->CCR |= (1 << 3) | (1 << 4) | (1 << 5)   //CAP1 on rising and falling edge and interrupt

#define CAPCOM1_ENABLE_CAP0()  LPC_TIM1->CCR |= (1 << 0) | (1 << 1) | (1 << 2)   //CAP0 on rising and falling edge and interrupt
#define CAPCOM1_ENABLE_CAP1()  LPC_TIM1->CCR |= (1 << 3) | (1 << 4) | (1 << 5)   //CAP1 on rising and falling edge and interrupt

#define CAPCOM2_ENABLE_CAP0()  LPC_TIM2->CCR |= (1 << 0) | (1 << 1) | (1 << 2)   //CAP0 on rising and falling edge and interrupt
#define CAPCOM2_ENABLE_CAP1()  LPC_TIM2->CCR |= (1 << 3) | (1 << 4) | (1 << 5)   //CAP1 on rising and falling edge and interrupt

#define CAPCOM3_ENABLE_CAP0()  LPC_TIM3->CCR |= (1 << 0) | (1 << 1) | (1 << 2)   //CAP0 on rising and falling edge and interrupt
#define CAPCOM3_ENABLE_CAP1()  LPC_TIM3->CCR |= (1 << 3) | (1 << 4) | (1 << 5)   //CAP1 on rising and falling edge and interrupt*/

#include <LPC17xx.h>

#endif
