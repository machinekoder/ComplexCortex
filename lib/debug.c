#include "debug.h"

//TODO: this file

#if USE_DEBUG
/* buffers */
static volatile char g_TWBuffer[TRBUFF_LEN];
static volatile char g_TRBuffer[TWBUFF_LEN];
/* g_TConn!=0 when debugguer is connected */
static volatile uint8 g_TConn = 0u; 
#endif

/*void * Uart_putat(void * ap, const char *s, size_t n)
{
    //Uart *uart = (Uart*)ap;
    
    while (n--)
    {
        
        Uart_putchar(*uart, *s++);
    }
    
    return ap;
}*/

int8 Debug_printf(Debug_Level debugLevel, const char* pFormat, ...)
{
#if USE_DEBUG
    va_list args;
    char buff[TWBUFF_SIZE];
    uint8 nLen, nSize, nCnt;
        
    if(!g_TConn)
    {
        return (int8)(-1);
    }
    
    if (debugLevel > DEBUG_LEVEL)   // do not display unecessary messages
    {
        return (int8)(0);
    }
    
    va_start(args,pFormat);
    nSize = vsprintf(buff,pFormat,args);
    if(nSize > TWBUFF_MAX)
    nSize = TWBUFF_MAX;  
    va_end(args);    
    
    for(nCnt = 0u; nCnt<nSize; nCnt++)
    {
        while(TWBUFF_FULL())
        {
            if(!g_TConn)
            {
                return (int8)(-1);
            }
        }
        nLen = TWBUFF_TPTR;
        g_TWBuffer[nLen] = buff[nCnt];
        nLen = TWBUFF_INC(nLen);
        TWBUFF_TPTR = nLen;
    }
#endif
    
    return (int8)(0);
}

bool Debug_dataAvailable(void)
{
#if USE_DEBUG
    if(!g_TConn)
    {
        return FALSE;
    }
    
    return !TRBUFF_EMPTY(); 
#else
    return FALSE;
#endif
}
 
char Debug_getChar(void)
{
#if USE_DEBUG
    char nChar;
    uint8 nLen;
    
    while(!Debug_dataAvailable());   // wait
    
    nChar = g_TRBuffer[TRBUFF_TPTR];
    nLen  = TRBUFF_TPTR;
    nLen  = TRBUFF_INC(nLen);
    TRBUFF_TPTR = nLen;
    
    return nChar;
#else
    return '\0';
#endif
}  
