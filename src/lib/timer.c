#include "timer.h"

static void ((* functionPointer[4u])(void));

static int8 initializeTimer0(uint32 khz, uint32 intervalUs);
static int8 initializeTimer1(uint32 khz, uint32 intervalUs);
static int8 initializeTimer2(uint32 khz, uint32 intervalUs);
static int8 initializeTimer3(uint32 khz, uint32 intervalUs);

static int8 deinitializeTimer0(void);
static int8 deinitializeTimer1(void);
static int8 deinitializeTimer2(void);
static int8 deinitializeTimer3(void);

static void startTimer0(void);
static void startTimer1(void);
static void startTimer2(void);
static void startTimer3(void);

static void stopTimer0(void);
static void stopTimer1(void);
static void stopTimer2(void);
static void stopTimer3(void);

static void resetTimer0(void);
static void resetTimer1(void);
static void resetTimer2(void);
static void resetTimer3(void);

static void setIntervalUsTimer0(uint32 us);
static void setIntervalUsTimer1(uint32 us);
static void setIntervalUsTimer2(uint32 us);
static void setIntervalUsTimer3(uint32 us);

static void setIntervalMsTimer0(uint32 ms);
static void setIntervalMsTimer1(uint32 ms);
static void setIntervalMsTimer2(uint32 ms);
static void setIntervalMsTimer3(uint32 ms);

static void setPriorityTimer0(uint8 priority);
static void setPriorityTimer1(uint8 priority);
static void setPriorityTimer2(uint8 priority);
static void setPriorityTimer3(uint8 priority);

static void connectFunctionTimer0(void (* func)(void));
static void connectFunctionTimer1(void (* func)(void));
static void connectFunctionTimer2(void (* func)(void));
static void connectFunctionTimer3(void (* func)(void));

static int8 delayUsTimer0(uint32 us);
static int8 delayUsTimer1(uint32 us);
static int8 delayUsTimer2(uint32 us);
static int8 delayUsTimer3(uint32 us);

static int8 delayMsTimer0(uint32 ms);
static int8 delayMsTimer1(uint32 ms);
static int8 delayMsTimer2(uint32 ms);
static int8 delayMsTimer3(uint32 ms);

static int8 singleShotTimer0(uint32 ms, void (* func)(void));
static int8 singleShotTimer1(uint32 ms, void (* func)(void));
static int8 singleShotTimer2(uint32 ms, void (* func)(void));
static int8 singleShotTimer3(uint32 ms, void (* func)(void));

static uint32 getCounterValueTimer0();
static uint32 getCounterValueTimer1();
static uint32 getCounterValueTimer2();
static uint32 getCounterValueTimer3();

int8 initializeTimer0(uint32 khz, uint32 intervalUs)                                          
{                                                                                               
    uint32 pclk;                                                                                
    uint32 pr;                                                                                  
    uint32 targetfreq;                                                                                 
    uint32 mr0;                                                                                 
                                                                                                
    targetfreq = khz*1000u;                                                                     
                                                                                                
    if (TIMER_RUNNING(0))                       /* if timer is already in use return -1 */      
        return (int8)(-1);                                                                              
                                                                                                
    TIMER_ENABLE_POWER(0);                      /* Enable power */                              
    TIMER_SET_CORE_CLK_DIVISOR_1(0);                                                            
                                                                                                
    pclk = SystemCoreClock;                                                                     
    pr = (uint32)((pclk/targetfreq)-1u);        /* TODO: why +1? not needed I think */          
    mr0 = intervalUs/(1000000u/(pclk/(pr+1u)));                                                 
                                                                                                
    TIMER_SET_PRESCALER(0, pr);                 /* Set the clock prescaler */                   
    TIMER_SET_MATCH_REGISTER_0(0, mr0);         /* Set the match register */                    
                                                                                                
    TIMER_RESET(0);                             /* Reset Timer Counter and Prescale Counter */  
    TIMER_RESET_IRQS(0);                        /* Reset all interrupts */                      
    TIMER_RESET_AND_IRQ_ON_MATCH(0);            /* Reset the TC and generate an interrupt */    
    TIMER_ENABLE_IRQ(0);                        /* Enable IRQ for Timer_32_0) */                
    TIMER_SET_IRQ_PRIORITY(0, LOW_IRQ_PRIORITY);/* Set interrupt priority to allow delays in interrupt routine (not recommended) */  
                                                                                                
    functionPointer[0] = NULL;                                                                  
                                                                                                
    return (int8)(0);                                                                                   
}

int8 deinitializeTimer0(void)                                                             
{                                                                                           
    if (TIMER_RUNNING(0))                        /* if timer is already in use return -1 */ 
        return (int8)(-1);                                                                          
                                                                                            
    TIMER_DISABLE_IRQ(0);                        /* Disable timer interrupt */              
                                                                                            
    return (int8)(0);                                                                               
}

void startTimer0(void)                
{                                       
    TIMER_START(0); /* start timer */   
                                        
    return;                             
}

void stopTimer0(void)                 
{                                       
    TIMER_STOP(0); /* stop timer */     
                                        
    return;                             
}

void resetTimer0(void)                
{                                       
    TIMER_RESET(0); /* reset timer */   
    TIMER_START(0); /* start timer */   
                                        
    return;                             
}

void connectFunctionTimer0(void (* func)(void))   
{                                                   
    functionPointer[0] = func;                      
                                                    
    return;                                         
}

void setIntervalUsTimer0(uint32 us)       
{                                           
    uint32 pclk;                            
    uint32 pr;                              
    uint32 mr0;                             
                                            
    uint8 wasRunning = TIMER_RUNNING(0);    
                                            
    pclk = SystemCoreClock;                 
    pr = TIMER_GET_PRESCALER(0);            
    mr0 = us/(1000000u/(pclk/(pr+1u)));     
                                            
    TIMER_RESET(0);                         
    TIMER_SET_MATCH_REGISTER_0(0, mr0);     
                                            
    if (wasRunning)                         
        TIMER_START(0);                     
                                            
    return;                                 
}
           
void setIntervalMsTimer0(uint32 ms)       
{                                           
    uint32 pclk;                            
    uint32 pr;                              
    uint32 mr0;                             
                                            
    uint8 wasRunning = TIMER_RUNNING(0);    
                                            
    pclk = SystemCoreClock;                 
    pr = TIMER_GET_PRESCALER(0);            
    mr0 = ms/(1000u/(pclk/(pr+1u)));        
                                            
    TIMER_RESET(0);                         
    TIMER_SET_MATCH_REGISTER_0(0, mr0);     
                                            
    if (wasRunning)                         
        TIMER_START(0);                     
                                            
    return;                                 
}

void setPriorityTimer0(uint8 priority)    
{                                           
    TIMER_SET_IRQ_PRIORITY(0, priority);    
                                            
    return;                                 
}

int8 delayUsTimer0(uint32 us)                                                                 
{                                                                                               
    if (TIMER_RUNNING(0))                       /* if timer is already in use return -1 */      
        return (int8)(-1);                                                                              
                                                                                                
    TIMER_ENABLE_POWER(0);                                                                      
    TIMER_SET_CORE_CLK_DIVISOR_1(0);                                                            
                                                                                                
    TIMER_RESET(0);                                         /* Reset the timer */               
    TIMER_SET_PRESCALER(0, OPTIMIZE_PRESCALER_US());        /* Set prescaler to 2 */            
    TIMER_SET_INTERVAL_US(0, us);                           /* Set timer interval */            
    TIMER_RESET_IRQS(0);                                    /* Reset all interrupts */          
    TIMER_ENABLE_IRQ(0);                                    /* Enable timer interrupt */        
    TIMER_SET_IRQ_PRIORITY(0, HIGH_IRQ_PRIORITY);           /* Set interrupt priority to allow delays in interrupt routine (not recommended) */ 
    TIMER_STOP_AND_IRQ_ON_MATCH(0);                         /* Stop timer on match */           
                                                                                                
    functionPointer[0] = NULL;                                                                  
                                                                                                
    TIMER_START(0);                                         /* Start timer */                   
                                                                                                
    do {                                                                                        
        PROZESSOR_SLEEP();                              /* set prozessor into sleep mode */     
        PROZESSOR_WAIT_FOR_IRQ();                       /* Wait for interrupt */                
    }                                                                                           
    while(TIMER_RUNNING(0));                            /* Wait until the timer has finished */ 
                                                                                                
    TIMER_DISABLE_IRQ(0);                               /* Disable timer interrupt */           
                                                                                                
    return (int8)(0);                                                                                   
}
                                                                       
int8 delayMsTimer0(uint32 ms)                                                                 
{                                                                                               
    if (TIMER_RUNNING(0))                       /* if timer is already in use return -1 */      
        return (int8)(-1);                                                                              
                                                                                                
    TIMER_ENABLE_POWER(0);                                                                      
    TIMER_SET_CORE_CLK_DIVISOR_1(0);                                                            
                                                                                                
    TIMER_RESET(0);                                     /* Reset the timer */                   
    TIMER_SET_PRESCALER(0, OPTIMIZE_PRESCALER_MS());    /* Set prescaler to 2 */                
    TIMER_SET_INTERVAL_MS(0, ms);                       /* Set timer interval */                
    TIMER_RESET_IRQS(0);                                /* Reset all interrupts */              
    TIMER_ENABLE_IRQ(0);                                /* Enable timer interrupt */            
    TIMER_SET_IRQ_PRIORITY(0, HIGH_IRQ_PRIORITY);       /* Set interrupt priority to allow delays in interrupt routine (not recommended) */ 
    TIMER_STOP_AND_IRQ_ON_MATCH(0);                     /* Stop timer on match */               
                                                                                                
    functionPointer[0] = NULL;                                                                  
                                                                                                
    TIMER_START(0);                                     /* Start timer */                       
                                                                                                
    do {                                                                                        
        PROZESSOR_SLEEP();                              /* set prozessor into sleep mode */     
        PROZESSOR_WAIT_FOR_IRQ();                       /* Wait for interrupt */                
    }                                                                                           
    while(TIMER_RUNNING(0));                            /* Wait until the timer has finished */ 
                                                                                                
    TIMER_DISABLE_IRQ(0);                               /* Disable timer interrupt */           
                                                                                                
    return (int8)(0);                                                                                   
}

int8 singleShotTimer0(uint32 ms, void (* func)(void))                                         
{                                                                                               
    if (TIMER_RUNNING(0))                           /* if timer is already in use return 1 */   
        return (int8)(-1);                                                                              
                                                                                                
    TIMER_ENABLE_POWER(0);                                                                      
    TIMER_SET_CORE_CLK_DIVISOR_1(0);                                                            
                                                                                                
    TIMER_RESET(0);                                     /* Reset the timer */                   
    TIMER_SET_PRESCALER(0, OPTIMIZE_PRESCALER_MS());    /* Set prescaler to 0 */                
    TIMER_SET_INTERVAL_MS(0, ms);                       /* Set timer interval */                
    TIMER_RESET_IRQS(0);                                /* Reset all interrupts */              
    TIMER_ENABLE_IRQ(0);                                /* Enable timer interrupt */            
    TIMER_SET_IRQ_PRIORITY(0, LOW_IRQ_PRIORITY);        /* Set interrupt priority to allow delays in interrupt routine (not recommended) */ 
    TIMER_RESET_STOP_AND_IRQ_ON_MATCH(0);               /* Reset the TC and generate an interrupt */    
                                                                                                
    functionPointer[0] = func;                                                                  
                                                                                                
    TIMER_START(0);                                     /* Start timer */                       
                                                                                                
    return (int8)(0);                                                                                   
}

inline uint32 getCounterValueTimer0()     
{                                           
    return TIMER_COUNTER_VALUE(0);          
}

void TIMER_IRQHANDLER_0
{
    TIMER_RESET_IRQS(0);                    /* clear interrupt flag */      
                                                                        
    if (functionPointer[0] != NULL)                                         
        (*functionPointer[0])();                                            
                                                                        
    return;
}

int8 initializeTimer1(uint32 khz, uint32 intervalUs)                                          
{                                                                                               
    uint32 pclk;                                                                                
    uint32 pr;                                                                                  
    uint32 targetfreq;                                                                                 
    uint32 mr0;                                                                                 
                                                                                                
    targetfreq = khz*1000u;                                                                     
                                                                                                
    if (TIMER_RUNNING(1))                       /* if timer is already in use return -1 */      
        return (int8)(-1);                                                                              
                                                                                                
    TIMER_ENABLE_POWER(1);                      /* Enable power */                              
    TIMER_SET_CORE_CLK_DIVISOR_1(1);                                                            
                                                                                                
    pclk = SystemCoreClock;                                                                     
    pr = (uint32)((pclk/targetfreq)-1u);        /* TODO: why +1? not needed I think */          
    mr0 = intervalUs/(1000000u/(pclk/(pr+1u)));                                                 
                                                                                                
    TIMER_SET_PRESCALER(1, pr);                 /* Set the clock prescaler */                   
    TIMER_SET_MATCH_REGISTER_0(1, mr0);         /* Set the match register */                    
                                                                                                
    TIMER_RESET(1);                             /* Reset Timer Counter and Prescale Counter */  
    TIMER_RESET_IRQS(1);                        /* Reset all interrupts */                      
    TIMER_RESET_AND_IRQ_ON_MATCH(1);            /* Reset the TC and generate an interrupt */    
    TIMER_ENABLE_IRQ(1);                        /* Enable IRQ for Timer_32_0) */                
    TIMER_SET_IRQ_PRIORITY(1, LOW_IRQ_PRIORITY);/* Set interrupt priority to allow delays in interrupt routine (not recommended) */  
                                                                                                
    functionPointer[1] = NULL;                                                                  
                                                                                                
    return (int8)(0);                                                                                   
}

int8 deinitializeTimer1(void)                                                             
{                                                                                           
    if (TIMER_RUNNING(1))                        /* if timer is already in use return -1 */ 
        return (int8)(-1);                                                                          
                                                                                            
    TIMER_DISABLE_IRQ(1);                        /* Disable timer interrupt */              
                                                                                            
    return (int8)(0);                                                                               
}

void startTimer1(void)                
{                                       
    TIMER_START(1); /* start timer */   
                                        
    return;                             
}

void stopTimer1(void)                 
{                                       
    TIMER_STOP(1); /* stop timer */     
                                        
    return;                             
}

void resetTimer1(void)                
{                                       
    TIMER_RESET(1); /* reset timer */   
    TIMER_START(1); /* start timer */   
                                        
    return;                             
}

void connectFunctionTimer1(void (* func)(void))   
{                                                   
    functionPointer[1] = func;                      
                                                    
    return;                                         
}

void setIntervalUsTimer1(uint32 us)       
{                                           
    uint32 pclk;                            
    uint32 pr;                              
    uint32 mr0;                             
                                            
    uint8 wasRunning = TIMER_RUNNING(1);    
                                            
    pclk = SystemCoreClock;                 
    pr = TIMER_GET_PRESCALER(1);            
    mr0 = us/(1000000u/(pclk/(pr+1u)));     
                                            
    TIMER_RESET(1);                         
    TIMER_SET_MATCH_REGISTER_0(1, mr0);     
                                            
    if (wasRunning)                         
        TIMER_START(1);                     
                                            
    return;                                 
}
           
void setIntervalMsTimer1(uint32 ms)       
{                                           
    uint32 pclk;                            
    uint32 pr;                              
    uint32 mr0;                             
                                            
    uint8 wasRunning = TIMER_RUNNING(1);    
                                            
    pclk = SystemCoreClock;                 
    pr = TIMER_GET_PRESCALER(1);            
    mr0 = ms/(1000u/(pclk/(pr+1u)));        
                                            
    TIMER_RESET(1);                         
    TIMER_SET_MATCH_REGISTER_0(1, mr0);     
                                            
    if (wasRunning)                         
        TIMER_START(1);                     
                                            
    return;                                 
}

void setPriorityTimer1(uint8 priority)    
{                                           
    TIMER_SET_IRQ_PRIORITY(1, priority);    
                                            
    return;                                 
}

int8 delayUsTimer1(uint32 us)                                                                 
{                                                                                               
    if (TIMER_RUNNING(1))                       /* if timer is already in use return -1 */      
        return (int8)(-1);                                                                              
                                                                                                
    TIMER_ENABLE_POWER(1);                                                                      
    TIMER_SET_CORE_CLK_DIVISOR_1(1);                                                            
                                                                                                
    TIMER_RESET(1);                                         /* Reset the timer */               
    TIMER_SET_PRESCALER(1, OPTIMIZE_PRESCALER_US());        /* Set prescaler to 2 */            
    TIMER_SET_INTERVAL_US(1, us);                           /* Set timer interval */            
    TIMER_RESET_IRQS(1);                                    /* Reset all interrupts */          
    TIMER_ENABLE_IRQ(1);                                    /* Enable timer interrupt */        
    TIMER_SET_IRQ_PRIORITY(1, HIGH_IRQ_PRIORITY);           /* Set interrupt priority to allow delays in interrupt routine (not recommended) */ 
    TIMER_STOP_AND_IRQ_ON_MATCH(1);                         /* Stop timer on match */           
                                                                                                
    functionPointer[1] = NULL;                                                                  
                                                                                                
    TIMER_START(1);                                         /* Start timer */                   
                                                                                                
    do {                                                                                        
        PROZESSOR_SLEEP();                              /* set prozessor into sleep mode */     
        PROZESSOR_WAIT_FOR_IRQ();                       /* Wait for interrupt */                
    }                                                                                           
    while(TIMER_RUNNING(1));                            /* Wait until the timer has finished */ 
                                                                                                
    TIMER_DISABLE_IRQ(1);                               /* Disable timer interrupt */           
                                                                                                
    return (int8)(0);                                                                                   
}
                                                                       
int8 delayMsTimer1(uint32 ms)                                                                 
{                                                                                               
    if (TIMER_RUNNING(1))                       /* if timer is already in use return -1 */      
        return (int8)(-1);                                                                              
                                                                                                
    TIMER_ENABLE_POWER(1);                                                                      
    TIMER_SET_CORE_CLK_DIVISOR_1(1);                                                            
                                                                                                
    TIMER_RESET(1);                                     /* Reset the timer */                   
    TIMER_SET_PRESCALER(1, OPTIMIZE_PRESCALER_MS());    /* Set prescaler to 2 */                
    TIMER_SET_INTERVAL_MS(1, ms);                       /* Set timer interval */                
    TIMER_RESET_IRQS(1);                                /* Reset all interrupts */              
    TIMER_ENABLE_IRQ(1);                                /* Enable timer interrupt */            
    TIMER_SET_IRQ_PRIORITY(1, HIGH_IRQ_PRIORITY);       /* Set interrupt priority to allow delays in interrupt routine (not recommended) */ 
    TIMER_STOP_AND_IRQ_ON_MATCH(1);                     /* Stop timer on match */               
                                                                                                
    functionPointer[1] = NULL;                                                                  
                                                                                                
    TIMER_START(1);                                     /* Start timer */                       
                                                                                                
    do {                                                                                        
        PROZESSOR_SLEEP();                              /* set prozessor into sleep mode */     
        PROZESSOR_WAIT_FOR_IRQ();                       /* Wait for interrupt */                
    }                                                                                           
    while(TIMER_RUNNING(1));                            /* Wait until the timer has finished */ 
                                                                                                
    TIMER_DISABLE_IRQ(1);                               /* Disable timer interrupt */           
                                                                                                
    return (int8)(0);                                                                                   
}

int8 singleShotTimer1(uint32 ms, void (* func)(void))                                         
{                                                                                               
    if (TIMER_RUNNING(1))                           /* if timer is already in use return 1 */   
        return (int8)(-1);                                                                              
                                                                                                
    TIMER_ENABLE_POWER(1);                                                                      
    TIMER_SET_CORE_CLK_DIVISOR_1(1);                                                            
                                                                                                
    TIMER_RESET(1);                                     /* Reset the timer */                   
    TIMER_SET_PRESCALER(1, OPTIMIZE_PRESCALER_MS());    /* Set prescaler to 0 */                
    TIMER_SET_INTERVAL_MS(1, ms);                       /* Set timer interval */                
    TIMER_RESET_IRQS(1);                                /* Reset all interrupts */              
    TIMER_ENABLE_IRQ(1);                                /* Enable timer interrupt */            
    TIMER_SET_IRQ_PRIORITY(1, LOW_IRQ_PRIORITY);        /* Set interrupt priority to allow delays in interrupt routine (not recommended) */ 
    TIMER_RESET_STOP_AND_IRQ_ON_MATCH(1);               /* Reset the TC and generate an interrupt */    
                                                                                                
    functionPointer[1] = func;                                                                  
                                                                                                
    TIMER_START(1);                                     /* Start timer */                       
                                                                                                
    return (int8)(0);                                                                                   
}

inline uint32 getCounterValueTimer1()     
{                                           
    return TIMER_COUNTER_VALUE(1);          
}

void TIMER_IRQHANDLER_1
{
    TIMER_RESET_IRQS(1);                    /* clear interrupt flag */      
                                                                        
    if (functionPointer[1] != NULL)                                         
        (*functionPointer[1])();                                            
                                                                        
    return;
}

int8 initializeTimer2(uint32 khz, uint32 intervalUs)                                          
{                                                                                               
    uint32 pclk;                                                                                
    uint32 pr;                                                                                  
    uint32 targetfreq;                                                                                 
    uint32 mr0;                                                                                 
                                                                                                
    targetfreq = khz*1000u;                                                                     
                                                                                                
    if (TIMER_RUNNING(2))                       /* if timer is already in use return -1 */      
        return (int8)(-1);                                                                              
                                                                                                
    TIMER_ENABLE_POWER(2);                      /* Enable power */                              
    TIMER_SET_CORE_CLK_DIVISOR_1(2);                                                            
                                                                                                
    pclk = SystemCoreClock;                                                                     
    pr = (uint32)((pclk/targetfreq)-1u);        /* TODO: why +1? not needed I think */          
    mr0 = intervalUs/(1000000u/(pclk/(pr+1u)));                                                 
                                                                                                
    TIMER_SET_PRESCALER(2, pr);                 /* Set the clock prescaler */                   
    TIMER_SET_MATCH_REGISTER_0(2, mr0);         /* Set the match register */                    
                                                                                                
    TIMER_RESET(2);                             /* Reset Timer Counter and Prescale Counter */  
    TIMER_RESET_IRQS(2);                        /* Reset all interrupts */                      
    TIMER_RESET_AND_IRQ_ON_MATCH(2);            /* Reset the TC and generate an interrupt */    
    TIMER_ENABLE_IRQ(2);                        /* Enable IRQ for Timer_32_0) */                
    TIMER_SET_IRQ_PRIORITY(2, LOW_IRQ_PRIORITY);/* Set interrupt priority to allow delays in interrupt routine (not recommended) */  
                                                                                                
    functionPointer[2] = NULL;                                                                  
                                                                                                
    return (int8)(0);                                                                                   
}

int8 deinitializeTimer2(void)                                                             
{                                                                                           
    if (TIMER_RUNNING(2))                        /* if timer is already in use return -1 */ 
        return (int8)(-1);                                                                          
                                                                                            
    TIMER_DISABLE_IRQ(2);                        /* Disable timer interrupt */              
                                                                                            
    return (int8)(0);                                                                               
}

void startTimer2(void)                
{                                       
    TIMER_START(2); /* start timer */   
                                        
    return;                             
}

void stopTimer2(void)                 
{                                       
    TIMER_STOP(2); /* stop timer */     
                                        
    return;                             
}

void resetTimer2(void)                
{                                       
    TIMER_RESET(2); /* reset timer */   
    TIMER_START(2); /* start timer */   
                                        
    return;                             
}

void connectFunctionTimer2(void (* func)(void))   
{                                                   
    functionPointer[2] = func;                      
                                                    
    return;                                         
}

void setIntervalUsTimer2(uint32 us)       
{                                           
    uint32 pclk;                            
    uint32 pr;                              
    uint32 mr0;                             
                                            
    uint8 wasRunning = TIMER_RUNNING(2);    
                                            
    pclk = SystemCoreClock;                 
    pr = TIMER_GET_PRESCALER(2);            
    mr0 = us/(1000000u/(pclk/(pr+1u)));     
                                            
    TIMER_RESET(2);                         
    TIMER_SET_MATCH_REGISTER_0(2, mr0);     
                                            
    if (wasRunning)                         
        TIMER_START(2);                     
                                            
    return;                                 
}
           
void setIntervalMsTimer2(uint32 ms)       
{                                           
    uint32 pclk;                            
    uint32 pr;                              
    uint32 mr0;                             
                                            
    uint8 wasRunning = TIMER_RUNNING(2);    
                                            
    pclk = SystemCoreClock;                 
    pr = TIMER_GET_PRESCALER(2);            
    mr0 = ms/(1000u/(pclk/(pr+1u)));        
                                            
    TIMER_RESET(2);                         
    TIMER_SET_MATCH_REGISTER_0(2, mr0);     
                                            
    if (wasRunning)                         
        TIMER_START(2);                     
                                            
    return;                                 
}

void setPriorityTimer2(uint8 priority)    
{                                           
    TIMER_SET_IRQ_PRIORITY(2, priority);    
                                            
    return;                                 
}

int8 delayUsTimer2(uint32 us)                                                                 
{                                                                                               
    if (TIMER_RUNNING(2))                       /* if timer is already in use return -1 */      
        return (int8)(-1);                                                                              
                                                                                                
    TIMER_ENABLE_POWER(2);                                                                      
    TIMER_SET_CORE_CLK_DIVISOR_1(2);                                                            
                                                                                                
    TIMER_RESET(2);                                         /* Reset the timer */               
    TIMER_SET_PRESCALER(2, OPTIMIZE_PRESCALER_US());        /* Set prescaler to 2 */            
    TIMER_SET_INTERVAL_US(2, us);                           /* Set timer interval */            
    TIMER_RESET_IRQS(2);                                    /* Reset all interrupts */          
    TIMER_ENABLE_IRQ(2);                                    /* Enable timer interrupt */        
    TIMER_SET_IRQ_PRIORITY(2, HIGH_IRQ_PRIORITY);           /* Set interrupt priority to allow delays in interrupt routine (not recommended) */ 
    TIMER_STOP_AND_IRQ_ON_MATCH(2);                         /* Stop timer on match */           
                                                                                                
    functionPointer[2] = NULL;                                                                  
                                                                                                
    TIMER_START(2);                                         /* Start timer */                   
                                                                                                
    do {                                                                                        
        PROZESSOR_SLEEP();                              /* set prozessor into sleep mode */     
        PROZESSOR_WAIT_FOR_IRQ();                       /* Wait for interrupt */                
    }                                                                                           
    while(TIMER_RUNNING(2));                            /* Wait until the timer has finished */ 
                                                                                                
    TIMER_DISABLE_IRQ(2);                               /* Disable timer interrupt */           
                                                                                                
    return (int8)(0);                                                                                   
}
                                                                       
int8 delayMsTimer2(uint32 ms)                                                                 
{                                                                                               
    if (TIMER_RUNNING(2))                       /* if timer is already in use return -1 */      
        return (int8)(-1);                                                                              
                                                                                                
    TIMER_ENABLE_POWER(2);                                                                      
    TIMER_SET_CORE_CLK_DIVISOR_1(2);                                                            
                                                                                                
    TIMER_RESET(2);                                     /* Reset the timer */                   
    TIMER_SET_PRESCALER(2, OPTIMIZE_PRESCALER_MS());    /* Set prescaler to 2 */                
    TIMER_SET_INTERVAL_MS(2, ms);                       /* Set timer interval */                
    TIMER_RESET_IRQS(2);                                /* Reset all interrupts */              
    TIMER_ENABLE_IRQ(2);                                /* Enable timer interrupt */            
    TIMER_SET_IRQ_PRIORITY(2, HIGH_IRQ_PRIORITY);       /* Set interrupt priority to allow delays in interrupt routine (not recommended) */ 
    TIMER_STOP_AND_IRQ_ON_MATCH(2);                     /* Stop timer on match */               
                                                                                                
    functionPointer[2] = NULL;                                                                  
                                                                                                
    TIMER_START(2);                                     /* Start timer */                       
                                                                                                
    do {                                                                                        
        PROZESSOR_SLEEP();                              /* set prozessor into sleep mode */     
        PROZESSOR_WAIT_FOR_IRQ();                       /* Wait for interrupt */                
    }                                                                                           
    while(TIMER_RUNNING(2));                            /* Wait until the timer has finished */ 
                                                                                                
    TIMER_DISABLE_IRQ(2);                               /* Disable timer interrupt */           
                                                                                                
    return (int8)(0);                                                                                   
}

int8 singleShotTimer2(uint32 ms, void (* func)(void))                                         
{                                                                                               
    if (TIMER_RUNNING(2))                           /* if timer is already in use return 1 */   
        return (int8)(-1);                                                                              
                                                                                                
    TIMER_ENABLE_POWER(2);                                                                      
    TIMER_SET_CORE_CLK_DIVISOR_1(2);                                                            
                                                                                                
    TIMER_RESET(2);                                     /* Reset the timer */                   
    TIMER_SET_PRESCALER(2, OPTIMIZE_PRESCALER_MS());    /* Set prescaler to 0 */                
    TIMER_SET_INTERVAL_MS(2, ms);                       /* Set timer interval */                
    TIMER_RESET_IRQS(2);                                /* Reset all interrupts */              
    TIMER_ENABLE_IRQ(2);                                /* Enable timer interrupt */            
    TIMER_SET_IRQ_PRIORITY(2, LOW_IRQ_PRIORITY);        /* Set interrupt priority to allow delays in interrupt routine (not recommended) */ 
    TIMER_RESET_STOP_AND_IRQ_ON_MATCH(2);               /* Reset the TC and generate an interrupt */    
                                                                                                
    functionPointer[2] = func;                                                                  
                                                                                                
    TIMER_START(2);                                     /* Start timer */                       
                                                                                                
    return (int8)(0);                                                                                   
}

inline uint32 getCounterValueTimer2()     
{                                           
    return TIMER_COUNTER_VALUE(2);          
}

void TIMER_IRQHANDLER_2
{
    TIMER_RESET_IRQS(2);                    /* clear interrupt flag */      
                                                                        
    if (functionPointer[2] != NULL)                                         
        (*functionPointer[2])();                                            
                                                                        
    return;
}

int8 initializeTimer3(uint32 khz, uint32 intervalUs)                                          
{                                                                                               
    uint32 pclk;                                                                                
    uint32 pr;                                                                                  
    uint32 targetfreq;                                                                                 
    uint32 mr0;                                                                                 
                                                                                                
    targetfreq = khz*1000u;                                                                     
                                                                                                
    if (TIMER_RUNNING(3))                       /* if timer is already in use return -1 */      
        return (int8)(-1);                                                                              
                                                                                                
    TIMER_ENABLE_POWER(3);                      /* Enable power */                              
    TIMER_SET_CORE_CLK_DIVISOR_1(3);                                                            
                                                                                                
    pclk = SystemCoreClock;                                                                     
    pr = (uint32)((pclk/targetfreq)-1u);        /* TODO: why +1? not needed I think */          
    mr0 = intervalUs/(1000000u/(pclk/(pr+1u)));                                                 
                                                                                                
    TIMER_SET_PRESCALER(3, pr);                 /* Set the clock prescaler */                   
    TIMER_SET_MATCH_REGISTER_0(3, mr0);         /* Set the match register */                    
                                                                                                
    TIMER_RESET(3);                             /* Reset Timer Counter and Prescale Counter */  
    TIMER_RESET_IRQS(3);                        /* Reset all interrupts */                      
    TIMER_RESET_AND_IRQ_ON_MATCH(3);            /* Reset the TC and generate an interrupt */    
    TIMER_ENABLE_IRQ(3);                        /* Enable IRQ for Timer_32_0) */                
    TIMER_SET_IRQ_PRIORITY(3, LOW_IRQ_PRIORITY);/* Set interrupt priority to allow delays in interrupt routine (not recommended) */  
                                                                                                
    functionPointer[3] = NULL;                                                                  
                                                                                                
    return (int8)(0);                                                                                   
}

int8 deinitializeTimer3(void)                                                             
{                                                                                           
    if (TIMER_RUNNING(3))                        /* if timer is already in use return -1 */ 
        return (int8)(-1);                                                                          
                                                                                            
    TIMER_DISABLE_IRQ(3);                        /* Disable timer interrupt */              
                                                                                            
    return (int8)(0);                                                                               
}

void startTimer3(void)                
{                                       
    TIMER_START(3); /* start timer */   
                                        
    return;                             
}

void stopTimer3(void)                 
{                                       
    TIMER_STOP(3); /* stop timer */     
                                        
    return;                             
}

void resetTimer3(void)                
{                                       
    TIMER_RESET(3); /* reset timer */   
    TIMER_START(3); /* start timer */   
                                        
    return;                             
}

void connectFunctionTimer3(void (* func)(void))   
{                                                   
    functionPointer[3] = func;                      
                                                    
    return;                                         
}

void setIntervalUsTimer3(uint32 us)       
{                                           
    uint32 pclk;                            
    uint32 pr;                              
    uint32 mr0;                             
                                            
    uint8 wasRunning = TIMER_RUNNING(3);    
                                            
    pclk = SystemCoreClock;                 
    pr = TIMER_GET_PRESCALER(3);            
    mr0 = us/(1000000u/(pclk/(pr+1u)));     
                                            
    TIMER_RESET(3);                         
    TIMER_SET_MATCH_REGISTER_0(3, mr0);     
                                            
    if (wasRunning)                         
        TIMER_START(3);                     
                                            
    return;                                 
}
           
void setIntervalMsTimer3(uint32 ms)       
{                                           
    uint32 pclk;                            
    uint32 pr;                              
    uint32 mr0;                             
                                            
    uint8 wasRunning = TIMER_RUNNING(3);    
                                            
    pclk = SystemCoreClock;                 
    pr = TIMER_GET_PRESCALER(3);            
    mr0 = ms/(1000u/(pclk/(pr+1u)));        
                                            
    TIMER_RESET(3);                         
    TIMER_SET_MATCH_REGISTER_0(3, mr0);     
                                            
    if (wasRunning)                         
        TIMER_START(3);                     
                                            
    return;                                 
}

void setPriorityTimer3(uint8 priority)    
{                                           
    TIMER_SET_IRQ_PRIORITY(3, priority);    
                                            
    return;                                 
}

int8 delayUsTimer3(uint32 us)                                                                 
{                                                                                               
    if (TIMER_RUNNING(3))                       /* if timer is already in use return -1 */      
        return (int8)(-1);                                                                              
                                                                                                
    TIMER_ENABLE_POWER(3);                                                                      
    TIMER_SET_CORE_CLK_DIVISOR_1(3);                                                            
                                                                                                
    TIMER_RESET(3);                                         /* Reset the timer */               
    TIMER_SET_PRESCALER(3, OPTIMIZE_PRESCALER_US());        /* Set prescaler to 2 */            
    TIMER_SET_INTERVAL_US(3, us);                           /* Set timer interval */            
    TIMER_RESET_IRQS(3);                                    /* Reset all interrupts */          
    TIMER_ENABLE_IRQ(3);                                    /* Enable timer interrupt */        
    TIMER_SET_IRQ_PRIORITY(3, HIGH_IRQ_PRIORITY);           /* Set interrupt priority to allow delays in interrupt routine (not recommended) */ 
    TIMER_STOP_AND_IRQ_ON_MATCH(3);                         /* Stop timer on match */           
                                                                                                
    functionPointer[3] = NULL;                                                                  
                                                                                                
    TIMER_START(3);                                         /* Start timer */                   
                                                                                                
    do {                                                                                        
        PROZESSOR_SLEEP();                              /* set prozessor into sleep mode */     
        PROZESSOR_WAIT_FOR_IRQ();                       /* Wait for interrupt */                
    }                                                                                           
    while(TIMER_RUNNING(3));                            /* Wait until the timer has finished */ 
                                                                                                
    TIMER_DISABLE_IRQ(3);                               /* Disable timer interrupt */           
                                                                                                
    return (int8)(0);                                                                                   
}
                                                                       
int8 delayMsTimer3(uint32 ms)                                                                 
{                                                                                               
    if (TIMER_RUNNING(3))                       /* if timer is already in use return -1 */      
        return (int8)(-1);                                                                              
                                                                                                
    TIMER_ENABLE_POWER(3);                                                                      
    TIMER_SET_CORE_CLK_DIVISOR_1(3);                                                            
                                                                                                
    TIMER_RESET(3);                                     /* Reset the timer */                   
    TIMER_SET_PRESCALER(3, OPTIMIZE_PRESCALER_MS());    /* Set prescaler to 2 */                
    TIMER_SET_INTERVAL_MS(3, ms);                       /* Set timer interval */                
    TIMER_RESET_IRQS(3);                                /* Reset all interrupts */              
    TIMER_ENABLE_IRQ(3);                                /* Enable timer interrupt */            
    TIMER_SET_IRQ_PRIORITY(3, HIGH_IRQ_PRIORITY);       /* Set interrupt priority to allow delays in interrupt routine (not recommended) */ 
    TIMER_STOP_AND_IRQ_ON_MATCH(3);                     /* Stop timer on match */               
                                                                                                
    functionPointer[3] = NULL;                                                                  
                                                                                                
    TIMER_START(3);                                     /* Start timer */                       
                                                                                                
    do {                                                                                        
        PROZESSOR_SLEEP();                              /* set prozessor into sleep mode */     
        PROZESSOR_WAIT_FOR_IRQ();                       /* Wait for interrupt */                
    }                                                                                           
    while(TIMER_RUNNING(3));                            /* Wait until the timer has finished */ 
                                                                                                
    TIMER_DISABLE_IRQ(3);                               /* Disable timer interrupt */           
                                                                                                
    return (int8)(0);                                                                                   
}

int8 singleShotTimer3(uint32 ms, void (* func)(void))                                         
{                                                                                               
    if (TIMER_RUNNING(3))                           /* if timer is already in use return 1 */   
        return (int8)(-1);                                                                              
                                                                                                
    TIMER_ENABLE_POWER(3);                                                                      
    TIMER_SET_CORE_CLK_DIVISOR_1(3);                                                            
                                                                                                
    TIMER_RESET(3);                                     /* Reset the timer */                   
    TIMER_SET_PRESCALER(3, OPTIMIZE_PRESCALER_MS());    /* Set prescaler to 0 */                
    TIMER_SET_INTERVAL_MS(3, ms);                       /* Set timer interval */                
    TIMER_RESET_IRQS(3);                                /* Reset all interrupts */              
    TIMER_ENABLE_IRQ(3);                                /* Enable timer interrupt */            
    TIMER_SET_IRQ_PRIORITY(3, LOW_IRQ_PRIORITY);        /* Set interrupt priority to allow delays in interrupt routine (not recommended) */ 
    TIMER_RESET_STOP_AND_IRQ_ON_MATCH(3);               /* Reset the TC and generate an interrupt */    
                                                                                                
    functionPointer[3] = func;                                                                  
                                                                                                
    TIMER_START(3);                                     /* Start timer */                       
                                                                                                
    return (int8)(0);                                                                                   
}

inline uint32 getCounterValueTimer3()     
{                                           
    return TIMER_COUNTER_VALUE(3);          
}

void TIMER_IRQHANDLER_3
{
    TIMER_RESET_IRQS(3);                    /* clear interrupt flag */      
                                                                        
    if (functionPointer[3] != NULL)                                         
        (*functionPointer[3])();                                            
                                                                        
    return;
}

inline uint32 Timer_counterValue(Timer id)
{
    if (id == 0u)
        return TIMER_COUNTER_VALUE(0);
    else if (id == 1u)
        return TIMER_COUNTER_VALUE(1);
    else if (id == 2u)
        return TIMER_COUNTER_VALUE(2);
    else if (id == 3u)
        return TIMER_COUNTER_VALUE(3);
    
    return 0u;
}

inline bool Timer_isRunning(Timer id)
{
    if (id == 0u)
        return (bool)(TIMER_RUNNING(0));
    else if (id == 1u)
        return (bool)(TIMER_RUNNING(1));
    else if (id == 2u)
        return (bool)(TIMER_RUNNING(2));
    else if (id == 3u)
        return (bool)(TIMER_RUNNING(3));
    
    return (bool)0u;
}

int8 Timer_initialize(Timer timer, uint32 khz, uint32 intervalUs)
{
    if (timer == Timer0)
    {
        return initializeTimer0(khz, intervalUs);
    }
    else if (timer == Timer1)
    {
        return initializeTimer1(khz, intervalUs);
    }
    else if (timer == Timer2)
    {
        return initializeTimer2(khz, intervalUs);
    }
    else if (timer == Timer3)
    {
        return initializeTimer3(khz, intervalUs);
    }
    else
    {
        return (int8)(-1);
    }
}

int8 Timer_initializeRandom(uint32 khz, uint32 intervalUs)
{
   if (!TIMER_RUNNING(0))
   {
      initializeTimer0(khz, intervalUs);
      return (int8)(0);
   }
   else if (!TIMER_RUNNING(1))
   {
       initializeTimer1(khz, intervalUs);
       return (int8)(1);
   }
   else if (!TIMER_RUNNING(2))
   {
       initializeTimer2(khz, intervalUs);
       return (int8)(2);
   }
   else if (!TIMER_RUNNING(3))
   {
       initializeTimer3(khz, intervalUs);
       return (int8)(3);
   }
   else
       return (int8)(-1);
}

int8 Timer_deinitialize(Timer id)
{
    if (id == Timer0)
    {
        return deinitializeTimer0();
    }
    else if (id == Timer1)
    {
        return deinitializeTimer1();
    }
    else if (id == Timer2)
    {
        return deinitializeTimer2();
    }
    else if (id == Timer3)
    {
        return deinitializeTimer3();
    }
    return (int8)(0);
}

void Timer_start(Timer id)
{
    if (id == Timer0)
    {
        return startTimer0();
    }
    else if (id == Timer1)
    {
        return startTimer1();
    }
    else if (id == Timer2)
    {
        return startTimer2();
    }
    else if (id == Timer3)
    {
        return startTimer3();
    }
    return;
}

void Timer_stop(Timer id)
{
    if (id == Timer0)
    {
        return stopTimer0();
    }
    else if (id == Timer1)
    {
        return stopTimer1();
    }
    else if (id == Timer2)
    {
        return stopTimer2();
    }
    else if (id == Timer3)
    {
        return stopTimer3();
    }
    return;
}

void Timer_reset(Timer id)
{
    if (id == Timer0)
    {
        return resetTimer0();
    }
    else if (id == Timer1)
    {
        return resetTimer1();
    }
    else if (id == Timer2)
    {
        return resetTimer2();
    }
    else if (id == Timer3)
    {
        return resetTimer3();
    }
    return;
}

void Timer_setIntervalUs(Timer id, uint32 us)
{
    if (id == Timer0)
    {
        return setIntervalUsTimer0(us);
    }
    else if (id == Timer1)
    {
        return setIntervalUsTimer1(us);
    }
    else if (id == Timer2)
    {
        return setIntervalUsTimer2(us);
    }
    else if (id == Timer3)
    {
        return setIntervalUsTimer3(us);
    }
    return;
}

void Timer_setIntervalMs(Timer id, uint32 ms)
{
    if (id == Timer0)
    {
        return setIntervalMsTimer0(ms);
    }
    else if (id == Timer1)
    {
        return setIntervalMsTimer1(ms);
    }
    else if (id == Timer2)
    {
        return setIntervalMsTimer2(ms);
    }
    else if (id == Timer3)
    {
        return setIntervalMsTimer3(ms);
    }
    return;
}

void Timer_connectFunction(Timer id, void (* func)(void))
{
    if (id == Timer0)
    {
        return connectFunctionTimer0(func);
    }
    else if (id == Timer1)
    {
        return connectFunctionTimer1(func);
    }
    else if (id == Timer2)
    {
        return connectFunctionTimer2(func);
    }
    else if (id == Timer3)
    {
        return connectFunctionTimer3(func);
    }
    return;
}

void Timer_setPriority(Timer id, uint8 priority)
{
    if (id == Timer0)
    {
        setPriorityTimer0(priority);
    }
    else if (id == Timer1)
    {
        setPriorityTimer1(priority);
    }
    else if (id == Timer2)
    {
        setPriorityTimer2(priority);
    }
    else if (id == Timer3)
    {
        setPriorityTimer3(priority);
    }
    return;
}

int8 Timer_delayUs(uint32 us)
{
   if (delayUsTimer0(us) == (int8)(0))
   {
      return (int8)(0);
   }
   else if (delayUsTimer1(us) == (int8)(0))
   {
       return (int8)(1);
   }
   else if (delayUsTimer2(us) == (int8)(0))
   {
       return (int8)(2);
   }
   else if (delayUsTimer3(us) == (int8)(0))
   {
       return (int8)(3);
   }
   else
       return (int8)(-1);
}

int8 Timer_delayMs(uint32 ms)
{
    if (!TIMER_RUNNING(0))
   {
      delayMsTimer0(ms);
      return (int8)(0);
   }
   else if (!TIMER_RUNNING(1))
   {
       delayMsTimer1(ms);
       return (int8)(1);
   }
   else if (!TIMER_RUNNING(2))
   {
       delayMsTimer2(ms);
       return (int8)(2);
   }
   else if (!TIMER_RUNNING(3))
   {
       delayMsTimer3(ms);
       return (int8)(3);
   }
   else
       return (int8)(-1);
}

int8 Timer_singleShot(uint32 ms, void (* func)(void))
{
    if (!TIMER_RUNNING(0))
   {
      singleShotTimer0(ms, func);
      return (int8)(0);
   }
   else if (!TIMER_RUNNING(1))
   {
       singleShotTimer1(ms, func);
       return (int8)(1);
   }
   else if (!TIMER_RUNNING(2))
   {
       singleShotTimer2(ms, func);
       return 2;
   }
   else if (!TIMER_RUNNING(3))
   {
       singleShotTimer3(ms, func);
       return 3;
   }
   else
       return -1;
}


#if 0
int8 initializeCapCom0(uint32 khz, uint8 pin, void (* func)(void))
{
    if (TIMER_RUNNING(0))                       /* if timer is already in use return -1 */
        return -1;
        
//    TIMER0_ENABLE_CLK();                        /* Enable clock */
    
    TIMER_RESET(0);                             /* Reset Timer Counter and Prescale Counter */
    TIMER_SET_KHZ(0, khz);                        /* Configure the timer to run with the given frequency */
    TIMER_SET_INTERVAL_MS(0, 1000u);
    TIMER_RESET_IRQS(0);                        /* Reset all interrupts */
    TIMER_ENABLE_IRQ(0);                        /* Enable IRQ for Timer_32_0) */
    TIMER_SET_IRQ_PRIORITY(0, LOW_IRQ_PRIORITY);  /* Set interrupt priority to allow delays in interrupt routine (not recommended) */
    
    
    switch (pin)
    {
        case 0u: CAPCOM_ENABLE_PIN0();  // Enable pin 0
                CAPCOM0_ENABLE_CAP0(); // set CAP0 events
                break;
        case 1u: CAPCOM_ENABLE_PIN1();  //Enable pin 1
                CAPCOM0_ENABLE_CAP0(); // set CAP0 events
                break;
        case 2u: CAPCOM_ENABLE_PIN2();  //Enable pin 2
                CAPCOM0_ENABLE_CAP0(); // set CAP0 events
                break;
        case 3u: CAPCOM_ENABLE_PIN3();  //Enable pin 3
                CAPCOM0_ENABLE_CAP0(); // set CAP0 events
                break;
        case 4u: CAPCOM_ENABLE_PIN4();  //Enable pin 4
                CAPCOM0_ENABLE_CAP1(); // set CAP1 events
                break;
        case 5u: CAPCOM_ENABLE_PIN5();  //Enable pin 5
                CAPCOM0_ENABLE_CAP1(); // set CAP1 events
                break;
        case 6u: CAPCOM_ENABLE_PIN6();  //Enable pin 6
                CAPCOM0_ENABLE_CAP1(); // set CAP1 events
                break;
        case 7u: CAPCOM_ENABLE_PIN7();  //Enable pin 7
                CAPCOM0_ENABLE_CAP1(); // set CAP1 events
                break;
        default: break;
    }
    
    functionPointer0 = func;  
    
    return 0;
}

int8 initializeCapCom3(uint32 khz, uint8 pin, void (* func)(void))
{
    //if (TIMER3_RUNNING())                       /* if timer is already in use return -1 */
    //    return -1;
        
    //TIMER3_ENABLE_CLK();                        /* Enable clock */
    
    CAPCOM_ENABLE_PIN5();  // Enable pin 0
    
    TIMER_SET_KHZ(3,khz);                        /* Configure the timer to run with the given frequency */
    TIMER_RESET(3);                             /* Reset Timer Counter and Prescale Counter */
    //TIMER3_SET_INTERVAL_MS(1000);
    //TIMER3_RESET_AND_IRQ_ON_MATCH();
    //TIMER3_RESET_ON_MATCH();            /* Reset the TC and generate an interrupt */
    //TIMER3_RESET_IRQS();                        /* Reset all interrupts */
    //TIMER3_SET_IRQ_PRIORITY(LOW_IRQ_PRIORITY);  /* Set interrupt priority to allow delays in interrupt routine (not recommended) */
    CAPCOM3_ENABLE_CAP0();
    CAPCOM3_ENABLE_CAP1(); // set CAP0 events
    TIMER_START(3); /* start timer */
    
    
   /* switch (pin)
    {
        case 0: CAPCOM_ENABLE_PIN0();  // Enable pin 0
                CAPCOM3_ENABLE_CAP0(); // set CAP0 events
                break;
        case 1: CAPCOM_ENABLE_PIN1();  //Enable pin 1
                CAPCOM3_ENABLE_CAP0(); // set CAP0 events
                break;
        case 2: CAPCOM_ENABLE_PIN2();  //Enable pin 2
                CAPCOM3_ENABLE_CAP0(); // set CAP0 events
                break;
        case 3: CAPCOM_ENABLE_PIN3();  //Enable pin 3
                CAPCOM3_ENABLE_CAP0(); // set CAP0 events
                break;
        case 4: CAPCOM_ENABLE_PIN4();  //Enable pin 4
                CAPCOM3_ENABLE_CAP1(); // set CAP1 events
                break;
        case 5: CAPCOM_ENABLE_PIN5();  //Enable pin 5
                CAPCOM3_ENABLE_CAP1(); // set CAP1 events
                break;
        case 6: CAPCOM_ENABLE_PIN6();  //Enable pin 6
                CAPCOM3_ENABLE_CAP1(); // set CAP1 events
                break;
        case 7: CAPCOM_ENABLE_PIN7();  //Enable pin 7
                CAPCOM3_ENABLE_CAP1(); // set CAP1 events
                break;
        default: break;
    }*/
    
    NVIC_SetPriority(TIMER3_IRQn, ((0x01<<3)|0x01));
    TIMER_ENABLE_IRQ(3);                        /* Enable IRQ for Timer_32_0) */
    
    
    functionPointer3 = func;  
    
    return 0;
}
#endif

#if 0
int8 initializeTimer0(uint32 khz, uint32 intervalUs)
{
    uint32 pclk;
    uint32 pr;
    uint32 targetfreq;
    //uint32 divisor;
    uint32 mr0;
    
    targetfreq = khz*1000u;
    
    if (TIMER_RUNNING(0))                       /* if timer is already in use return -1 */
        return -1;
    
    TIMER_ENABLE_POWER(0);                      /* Enable power */
    TIMER_SET_CORE_CLK_DIVISOR_1(0);
    
    /*pr = 0xFFFFFFFF/2-1;    // 2^32/2+1
    divisor = SystemCoreClock / targetfreq / (pr+1);
    
    if (divisor < 8)
        {
            if (divisor < 4)
            {
                if (divisor < 2)
                {
                    TIMER0_SET_CORE_CLK_DIVISOR_1();
                    pclk = SystemCoreClock;
                }
                else
                {
                    TIMER0_SET_CORE_CLK_DIVISOR_2();
                    pclk = SystemCoreClock/2;
                }
            }
            else
            {
                TIMER0_SET_CORE_CLK_DIVISOR_4();
                pclk = SystemCoreClock/4;
            }
        }
        else
        {
            TIMER0_SET_CORE_CLK_DIVISOR_8();
            pclk = SystemCoreClock/8;
        }*/
        
    pclk = SystemCoreClock;
    
    pr = (uint32)((pclk/targetfreq)-1u);   //TODO: why +1? not needed I think
    
    mr0 = intervalUs/(1000000u/(pclk/(pr+1u)));
    
    TIMER_SET_PRESCALER(0, pr);                   /* Set the clock prescaler */
    TIMER_SET_MATCH_REGISTER_0(0, mr0);           /* Set the match register */
     
    TIMER_RESET(0);                             /* Reset Timer Counter and Prescale Counter */
    TIMER_RESET_IRQS(0);                        /* Reset all interrupts */
    TIMER_RESET_AND_IRQ_ON_MATCH(0);            /* Reset the TC and generate an interrupt */
    TIMER_ENABLE_IRQ(0);                        /* Enable IRQ for Timer_32_0) */
    TIMER_SET_IRQ_PRIORITY(0, LOW_IRQ_PRIORITY);  /* Set interrupt priority to allow delays in interrupt routine (not recommended) */
    
    functionPointer[0u] = NULL;

    return 0;
}
#endif
