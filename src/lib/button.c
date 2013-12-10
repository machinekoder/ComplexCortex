#include "button.h"

static CircularBuffer buttonBuffer;
static volatile ButtonValue buttonBufferData[BUTTON_BUFFER_SIZE];

typedef struct {
    uint8 id;
    uint8 port;
    uint8 pin;
    uint8 unset;
    uint8 type;
} ButtonConfig;

static volatile ButtonConfig    buttons[BUTTON_MAX_COUNT];
static volatile ButtonValue     val[BUTTON_MAX_COUNT];
static volatile uint8           buttonCount = 0u;

static volatile uint32 maxunset;

/** reads the button values
 */
void valueButton(void);

/** stores the values and counts the matches.
 *@param i is the number of the row of buttons.
 *@param var is the value of the button.
 */
void putVal(uint8 i, uint8 pressed);

int8 Button_initialize(uint32 khz, uint32 sampleInterval, uint32 timeoutInterval, Timer timer)
{
    if (Cb_initialize(&buttonBuffer, BUTTON_BUFFER_SIZE, sizeof(ButtonValue), (void*)(&buttonBufferData)) == (int8)(-1))
        return (int8)(-1);
    
    if (Timer_initialize(timer, khz, sampleInterval) == (int8)(-1))
        return (int8)(-1);
    
    maxunset = (uint32)(timeoutInterval/sampleInterval);
    
    Timer_connectFunction(timer, valueButton);
    Timer_start(timer);

    return (int8)(0);
}

int8 Button_initialize2(uint32 sampleInterval, uint32 timeoutInterval)
{
    if (Cb_initialize(&buttonBuffer, BUTTON_BUFFER_SIZE, sizeof(ButtonValue), (void*)(&buttonBufferData)) == (int8)(-1))
        return (int8)(-1);
    
    maxunset = (uint32)(timeoutInterval/sampleInterval);
    
    return (int8)(0);
}

void Button_initializeButton(uint8 id, uint8 port, uint8 pin, Button_Type type)
{
    Gpio_setDirection(port, pin, Gpio_Direction_Input );    //direction 0=input
    if(type == Button_Type_LowActive)
        Pin_setMode(port, pin, Pin_Mode_PullUp);
    if(type == Button_Type_HighActive)
        Pin_setMode(port, pin, Pin_Mode_PullDown);
    
    buttons[id].id     = id;
    buttons[id].port   = port;
    buttons[id].pin    = pin;
    buttons[id].type   = type;
    buttons[id].unset  = 0u;
    
    val[id].count = 0u;
    val[id].id = id;

    buttonCount++;
}

void valueButton(void){
    uint8 i;
    
    for(i = 0u; i < buttonCount; i++)
    {
        if(buttons[i].type == Button_Type_LowActive)
        {
            Gpio_read(buttons[i].port,buttons[i].pin) ? putVal(i,0u): putVal(i,1u);
        }
        else if(buttons[i].type == Button_Type_HighActive)
        {
            Gpio_read(buttons[i].port,buttons[i].pin) ? putVal(i,1u): putVal(i,0u);
        }
	}
}

void putVal(uint8 i,uint8 pressed)
{
    
    if(pressed && (buttons[i].unset != 0u))
    {
        val[i].count = val[i].count + 1u;
        buttons[i].unset = 0u;
    }
    else
    {
        if(!pressed && buttons[i].unset <= maxunset) 
        {
            buttons[i].unset = buttons[i].unset + 1u;
        }
        if((buttons[i].unset > maxunset) && (val[i].count != 0u) )
        {
            Cb_put(&buttonBuffer,(void*)(&val[i]));
            val[i].count = 0u;
            buttons[i].unset = 0u;
        }
    }
}

int8 Button_getPress(ButtonValue *value)
{
    return Cb_get(&buttonBuffer, (void*)value);
}

uint8 Button_read(uint8 id)
{
    if(buttons[id].type == Button_Type_LowActive)
    {
        return (!Gpio_read(buttons[id].port,buttons[id].pin));
    }
    else if(buttons[id].type == Button_Type_HighActive)
    {
        return Gpio_read(buttons[id].port,buttons[id].pin);
    }
    
    return 0u;
}

void Button_task()
{
    valueButton();
}
