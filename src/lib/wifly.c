#include "wifly.h"

#define WIFLY_STANDARD_BAUD 9600

#ifndef WIFLY_COMMAND_MODE_ENTER_RETRY_ATTEMPTS
#define WIFLY_COMMAND_MODE_ENTER_RETRY_ATTEMPTS 5u
#endif
#ifndef WIFLY_COMMAND_MODE_GUARD_TIME
#define WIFLY_COMMAND_MODE_GUARD_TIME           250u     // ms
#endif
#ifndef WIFLY_REBOOT_GUARD_TIME
#define WIFLY_REBOOT_GUARD_TIME                 1000u
#endif
#ifndef WIFLY_COMMAND_SETTLE_TIME
#define WIFLY_COMMAND_SETTLE_TIME               30u
#endif
#ifndef WIFLY_VERSION_LENGTH
#define WIFLY_VERSION_LENGTH                    4u      //e.g. 4.00
#endif
#ifndef WIFLY_SET_OK
#define WIFLY_SET_OK                            "AOK"
#endif
#ifndef WIFLY_COMMAND_BUFFER_SIZE
#define WIFLY_COMMAND_BUFFER_SIZE               100u
#endif
#ifndef WIFLY_RESPONSE_BUFFER_SIZE
#define WIFLY_RESPONSE_BUFFER_SIZE              100u
#endif
#ifndef WIFLY_RESPONSE_TIMEOUT
#define WIFLY_RESPONSE_TIMEOUT                  1000u
#endif
#ifndef WIFLY_RESPONSE_MATCH_TIMEOUT
#define WIFLY_RESPONSE_MATCH_TIMEOUT            5000u
#endif
#ifndef WIFLY_RESPONSE_WAIT_TIMEOUT
#define WIFLY_RESPONSE_WAIT_TIMEOUT             2u
#endif
#ifndef WIFLY_SOFTWARE_REBOOT_RETRY_ATTEMPTS
#define WIFLY_SOFTWARE_REBOOT_RETRY_ATTEMPTS    5u
#endif
#ifndef WIFLY_TASK_BUFFER_SIZE
#define WIFLY_TASK_BUFFER_SIZE                  500u
#endif
#ifndef WIFLY_COMMAND_CHAR
#define WIFLY_COMMAND_CHAR                      13  // carriage return
#endif

static char   taskBuffer[WIFLY_TASK_BUFFER_SIZE];
static uint16 taskBufferPos;

static char commChar;
static char commCloseString[35u];
static char commOpenString[35u];
static char commRemoteString[35u];
static char wiFlyVersion[WIFLY_VERSION_LENGTH + 3u];    // <,>, and \0

static char commandBuffer[WIFLY_COMMAND_BUFFER_SIZE];
static char responseBuffer[WIFLY_RESPONSE_BUFFER_SIZE];
static uint16 responseBufferPos;
static Uart wiflyUart;

static WiFly_Status wiflyStatus;

static void (* taskFunctionPointer)(char *);
static void (* udpTaskFunctionPointer)(char *);

static WiFly_ConnectionState wiFlyState;

static int8 findInResponse(const char* toMatch, uint32 timeout);
//static int8 responseMatched(const char* toMatch);
static int8 findWiFlyVersion(uint32 timeout);
static int8 setCommand(char *command);
static int8 getCommand(char* command);
static int8 otherCommand(char *command, char* awaitedResponse);
static void internalProcessTask(char* command);
static void securityFunction(char* command);
static void processStatusMessage(char* command);

void securityFunction(char* command)
{
    // does nothing, just for security reasons
}

int8 WiFly_initialize(Uart uart, uint32 baudrate)
{
    /* intialize variables */
    commChar = '$';
    wiFlyState = WiFly_ConnectionState_Disconnected;
    
    if (Uart_initialize(uart, baudrate) == (int8)(-1))
        return (int8)(-1);                                  // if UART init fails return
    
    wiflyUart = uart;
    
    Uart_flush(wiflyUart);                                  // Trash unusefull chars
    
    if (WiFly_actionEnterCommandMode(FALSE)== (int8)(-1))   // Configure the wlan module
        return (int8)(-1);
    
    if (WiFly_setCommOpen("*OPEN*") == (int8)(-1))          // Set the open command
        return (int8)(-1);
    if (WiFly_setCommClose("*CLOS*") == (int8)(-1))         // Set the close command
        return (int8)(-1);
    //WiFly_setCommRemote("*HELLO*");                       // Set the remote command
    if (WiFly_setCommRemote("0") == (int8)(-1))             // Turn off remote message
        return (int8)(-1);
    if (WiFly_setSysPrintlvl(0x4010u) == (int8)(-1))        // Enable heartbeat messages
        return (int8)(-1);
    
    if (WiFly_actionExitCommandMode() == (int8)(-1))
        return (int8)(-1);
    
    Uart_setProcessFunction(wiflyUart, &internalProcessTask);
    taskFunctionPointer = &securityFunction;
    udpTaskFunctionPointer = &securityFunction;
    
    return (int8)(0);
}

int8 WiFly_setAdhocBeacon(uint32 ms)
{
    xsnprintf(commandBuffer, WIFLY_COMMAND_BUFFER_SIZE, "set a b %u\r", ms);
    return setCommand(commandBuffer);
}

int8 WiFly_setAdhocProbe(uint32 num)
{
    xsnprintf(commandBuffer, WIFLY_COMMAND_BUFFER_SIZE, "set a p %u\r", num);
    return setCommand(commandBuffer);
}

int8 WiFly_setBroadcastAddress(char *address)
{
    xsnprintf(commandBuffer, WIFLY_COMMAND_BUFFER_SIZE, "set b a %s\r", address);
    return setCommand(commandBuffer);
}

int8 WiFly_setBroadcastInterval(uint32 value)
{
    xsnprintf(commandBuffer, WIFLY_COMMAND_BUFFER_SIZE, "set b i %u\r", value);
    return setCommand(commandBuffer);
}

int8 WiFly_setBroadcastPort(uint16 port)
{
    xsnprintf(commandBuffer, WIFLY_COMMAND_BUFFER_SIZE, "set b p %u\r", port);
    return setCommand(commandBuffer);
}

int8 WiFly_setComm(char c)
{
    commChar = c;
    xsnprintf(commandBuffer, WIFLY_COMMAND_BUFFER_SIZE, "set c %c\r", c);
    return setCommand(commandBuffer);
}

int8 WiFly_setCommClose(char *string)
{
    strncpy(commCloseString, string, 10u);
    if (strlen(string) != 0u)
    {
        xsnprintf(commandBuffer, WIFLY_COMMAND_BUFFER_SIZE, "set c c %s\r", string);
    }
    else
    {
        strcpy(commandBuffer, "set c c 0\r");
    }
    return setCommand(commandBuffer);
}

int8 WiFly_setCommOpen(char *string)
{
    strncpy(commOpenString, string, 10u);
    if (strlen(string) != 0u)
    {
        xsnprintf(commandBuffer, WIFLY_COMMAND_BUFFER_SIZE, "set c o %s\r", string);
    }
    else
    {
        strcpy(commandBuffer, "set c o 0\r");
    }
    return setCommand(commandBuffer);
}

int8 WiFly_setCommRemote(char *string)
{
    strncpy(commRemoteString, string, 10u);
    if (strlen(string) != 0u)
    {
        xsnprintf(commandBuffer, WIFLY_COMMAND_BUFFER_SIZE, "set c r %s\r", string);
    }
    else
    {
        strcpy(commandBuffer, "set c r 0\r");
    }
    return setCommand(commandBuffer);
}

int8 WiFly_setCommIdle(uint32 secs)
{
    xsnprintf(commandBuffer, WIFLY_COMMAND_BUFFER_SIZE, "set c i %u\r", secs);
    return setCommand(commandBuffer);
}

int8 WiFly_setCommMatch(uint32 value)
{
    xsnprintf(commandBuffer, WIFLY_COMMAND_BUFFER_SIZE, "set c m %u\r", value);
    return setCommand(commandBuffer);
}

int8 WiFly_setCommSize(uint32 value)
{
    xsnprintf(commandBuffer, WIFLY_COMMAND_BUFFER_SIZE, "set c s %u\r", value);
    return setCommand(commandBuffer);
}

int8 WiFly_setCommTime(uint32 num)
{
    xsnprintf(commandBuffer, WIFLY_COMMAND_BUFFER_SIZE, "set c t %u\r", num);
    return setCommand(commandBuffer);
}

int8 WiFly_setDnsAddr(char *addr)
{
    if (strlen(addr) != 0u)
    {
        xsnprintf(commandBuffer, WIFLY_COMMAND_BUFFER_SIZE, "set d a %s\r", addr);
    }
    else
    {
        strcpy(commandBuffer, "set d a 0\r");
    }
    return setCommand(commandBuffer);
}

int8 WiFly_setDnsName(char *string)
{
    if (strlen(string) != 0u)
    {
        xsnprintf(commandBuffer, WIFLY_COMMAND_BUFFER_SIZE, "set d n %s\r", string);
    }
    else
    {
        strcpy(commandBuffer, "set d n 0\r");
    }
    return setCommand(commandBuffer);
}

int8 WiFly_setDnsBackup(char *string)
{
    if (strlen(string) != 0u)
    {
        xsnprintf(commandBuffer, WIFLY_COMMAND_BUFFER_SIZE, "set d b %s\r", string);
    }
    else
    {
        strcpy(commandBuffer, "set d b 0\r");
    }
    return setCommand(commandBuffer);
}

int8 WiFly_setFtpAddr(char *addr)
{
    if (strlen(addr) != 0u)
    {
        xsnprintf(commandBuffer, WIFLY_COMMAND_BUFFER_SIZE, "set f a %s\r", addr);
    }
    else
    {
        strcpy(commandBuffer, "set f a 0\r");
    }
    return setCommand(commandBuffer);
}

int8 WiFly_setFtpDir(char *string)
{
    if (strlen(string) != 0u)
    {
        xsnprintf(commandBuffer, WIFLY_COMMAND_BUFFER_SIZE, "set f d %s\r", string);
    }
    else
    {
        strcpy(commandBuffer, "set f d 0\r");
    }
    return setCommand(commandBuffer);
}

int8 WiFly_setFtpFilename(char *file)
{
    if (strlen(file) != 0u)
    {
        xsnprintf(commandBuffer, WIFLY_COMMAND_BUFFER_SIZE, "set f f %s\r", file);
    }
    else
    {
        strcpy(commandBuffer, "set f f 0\r");
    }
    return setCommand(commandBuffer);
}

int8 WiFly_setFtpMode(uint32 mask)
{
    xsnprintf(commandBuffer, WIFLY_COMMAND_BUFFER_SIZE, "set f m 0x%x\r", mask);
    return setCommand(commandBuffer);
}

int8 WiFly_setFtpRemote(uint32 port)
{
    xsnprintf(commandBuffer, WIFLY_COMMAND_BUFFER_SIZE, "set f r %u\r", port);
    return setCommand(commandBuffer);
}

int8 WiFly_setFtpTime(uint32 value)
{
    xsnprintf(commandBuffer, WIFLY_COMMAND_BUFFER_SIZE, "set f t %u\r", value);
    return setCommand(commandBuffer);
}

int8 WiFly_setFtpUser(char *name)
{
    if (strlen(name) != 0u)
    {
        xsnprintf(commandBuffer, WIFLY_COMMAND_BUFFER_SIZE, "set f u %s\r", name);
    }
    else
    {
        strcpy(commandBuffer, "set f u 0\r");
    }
    return setCommand(commandBuffer);
}

int8 WiFly_setFtpPass(char *pass)
{
    if (strlen(pass) != 0u)
    {
        xsnprintf(commandBuffer, WIFLY_COMMAND_BUFFER_SIZE, "set f p %s\r", pass);
    }
    else
    {
        strcpy(commandBuffer, "set f p 0\r");
    }
    return setCommand(commandBuffer);
}

int8 WiFly_setIpAddress(char *addr)
{
    if (strlen(addr) != 0u)
    {
        xsnprintf(commandBuffer, WIFLY_COMMAND_BUFFER_SIZE, "set i a %s\r", addr);
    }
    else
    {
        strcpy(commandBuffer, "set i a 0\r");
    }
    return setCommand(commandBuffer);
}

int8 WiFly_setIpBackup(char *addr)
{
    if (strlen(addr) != 0u)
    {
        xsnprintf(commandBuffer, WIFLY_COMMAND_BUFFER_SIZE, "set i b %s\r", addr);
    }
    else
    {
        strcpy(commandBuffer, "set i b 0\r");
    }
    return setCommand(commandBuffer);
}

int8 WiFly_setIpDhcp(uint32 value)
{
    xsnprintf(commandBuffer, WIFLY_COMMAND_BUFFER_SIZE, "set i d %u\r", value);
    return setCommand(commandBuffer);
}

int8 WiFly_setIpFlags(uint32 value)
{
    xsnprintf(commandBuffer, WIFLY_COMMAND_BUFFER_SIZE, "set i f 0x%x\r", value);
    return setCommand(commandBuffer);
}

int8 WiFly_setIpGateway(char *addr)
{
    if (strlen(addr) != 0u)
    {
        xsnprintf(commandBuffer, WIFLY_COMMAND_BUFFER_SIZE, "set i g %s\r", addr);
    }
    else
    {
        strcpy(commandBuffer, "set i g 0\r");
    }
    return setCommand(commandBuffer);
}

int8 WiFly_setIpHost(char *addr)
{
    if (strlen(addr) != 0u)
    {
        xsnprintf(commandBuffer, WIFLY_COMMAND_BUFFER_SIZE, "set i h %s\r", addr);
    }
    else
    {
        strcpy(commandBuffer, "set i h 0\r");
    }
    return setCommand(commandBuffer);
}

int8 WiFly_setIpLocalport(uint32 num)
{
    xsnprintf(commandBuffer, WIFLY_COMMAND_BUFFER_SIZE, "set i l %u\r", num);
    return setCommand(commandBuffer);
}

int8 WiFly_setIpNetmask(char *value)
{
    if (strlen(value) != 0u)
    {
        xsnprintf(commandBuffer, WIFLY_COMMAND_BUFFER_SIZE, "set i n %s\r", value);
    }
    else
    {
        strcpy(commandBuffer, "set i n 0\r");
    }
    return setCommand(commandBuffer);
}

int8 WiFly_setIpProtocol(uint32 value)
{
    xsnprintf(commandBuffer, WIFLY_COMMAND_BUFFER_SIZE, "set i p %u\r", value);
    return setCommand(commandBuffer);
}

int8 WiFly_setIpRemote(uint32 value)
{
    xsnprintf(commandBuffer, WIFLY_COMMAND_BUFFER_SIZE, "set i r %u\r", value);
    return setCommand(commandBuffer);
}

int8 WiFly_setIpTcpMode(uint32 mask)
{
    xsnprintf(commandBuffer, WIFLY_COMMAND_BUFFER_SIZE, "set i t 0x%x\r", mask);
    return setCommand(commandBuffer);
}

int8 WiFly_setOptJointmr(uint32 msecs)
{
    xsnprintf(commandBuffer, WIFLY_COMMAND_BUFFER_SIZE, "set o j %u\r", msecs);
    return setCommand(commandBuffer);
}

int8 WiFly_setOptFormat(uint32 value)
{
    xsnprintf(commandBuffer, WIFLY_COMMAND_BUFFER_SIZE, "set o f %u\r", value);
    return setCommand(commandBuffer);
}

int8 WiFly_setOptReplace(char character)
{
    xsnprintf(commandBuffer, WIFLY_COMMAND_BUFFER_SIZE, "set o r %c\r", character);
    return setCommand(commandBuffer);
}

int8 WiFly_setOptDeviceid(char *string)
{
    if (strlen(string) != 0u)
    {
        xsnprintf(commandBuffer, WIFLY_COMMAND_BUFFER_SIZE, "set o d %s\r", string);
    }
    else
    {
        strcpy(commandBuffer, "set o d 0\r");
    }
    return setCommand(commandBuffer);
}

int8 WiFly_setOptPassword(char *string)
{
    if (strlen(string) != 0u)
    {
        xsnprintf(commandBuffer, WIFLY_COMMAND_BUFFER_SIZE, "set o p %s\r", string);
    }
    else
    {
        strcpy(commandBuffer, "set o p 0\r");
    }
    return setCommand(commandBuffer);
}

int8 WiFly_setSysAutoconn(uint32 value)
{
    xsnprintf(commandBuffer, WIFLY_COMMAND_BUFFER_SIZE, "set s a %u\r", value);
    return setCommand(commandBuffer);
}

int8 WiFly_setSysAutosleep(uint32 value)
{
    xsnprintf(commandBuffer, WIFLY_COMMAND_BUFFER_SIZE, "set s a %u\r", value);
    return setCommand(commandBuffer);
}

int8 WiFly_setSysIofunc(uint32 value)
{
    xsnprintf(commandBuffer, WIFLY_COMMAND_BUFFER_SIZE, "set s i %u\r", value);
    return setCommand(commandBuffer);
}

int8 WiFly_setSysMask(uint32 mask)
{
    xsnprintf(commandBuffer, WIFLY_COMMAND_BUFFER_SIZE, "set s m 0x%x\r", mask);
    return setCommand(commandBuffer);
}

int8 WiFly_setSysPrintlvl(uint32 value)
{
    xsnprintf(commandBuffer, WIFLY_COMMAND_BUFFER_SIZE, "set s p %u\r", value);
    return setCommand(commandBuffer);
}

int8 WiFly_setSysOutput(uint32 value, uint32 mask)
{
    xsnprintf(commandBuffer, WIFLY_COMMAND_BUFFER_SIZE, "set s o 0x%x 0x%x\r", value, mask);
    return setCommand(commandBuffer);
}

int8 WiFly_setSysSleep(uint32 value)
{
    xsnprintf(commandBuffer, WIFLY_COMMAND_BUFFER_SIZE, "set s s %u\r", value);
    return setCommand(commandBuffer);
}

int8 WiFly_setSysTrigger(uint32 value)
{
    xsnprintf(commandBuffer, WIFLY_COMMAND_BUFFER_SIZE, "set s t %u\r", value);
    return setCommand(commandBuffer);
}

int8 WiFly_setSysValue(uint32 value)
{
    xsnprintf(commandBuffer, WIFLY_COMMAND_BUFFER_SIZE, "set s v %u\r", value);
    return setCommand(commandBuffer);
}

int8 WiFly_setSysWake(uint32 secs)
{
    xsnprintf(commandBuffer, WIFLY_COMMAND_BUFFER_SIZE, "set s w %u\r", secs);
    return setCommand(commandBuffer);
}

int8 WiFly_setTimeAddress(char *addr)
{
    if (strlen(addr) != 0u)
    {
        xsnprintf(commandBuffer, WIFLY_COMMAND_BUFFER_SIZE, "set t a %s\r", addr);
    }
    else
    {
        strcpy(commandBuffer, "set t a 0\r");
    }
    return setCommand(commandBuffer);
}

int8 WiFly_setTimePort(uint32 num)
{
    xsnprintf(commandBuffer, WIFLY_COMMAND_BUFFER_SIZE, "set t p %u\r", num);
    return setCommand(commandBuffer);
}

int8 WiFly_setTimeEnable(uint32 value)
{
    xsnprintf(commandBuffer, WIFLY_COMMAND_BUFFER_SIZE, "set t e %u\r", value);
    return setCommand(commandBuffer);
}

int8 WiFly_setTimeRaw(uint32 value)
{
    xsnprintf(commandBuffer, WIFLY_COMMAND_BUFFER_SIZE, "set t r %u\r", value);
    return setCommand(commandBuffer);
}

int8 WiFly_setUartBaud(uint32 rate)
{
    xsnprintf(commandBuffer, WIFLY_COMMAND_BUFFER_SIZE, "set u b %u\r", rate);
    return setCommand(commandBuffer);
}

int8 WiFly_setUartInstant(uint32 rate)
{
    xsnprintf(commandBuffer, WIFLY_COMMAND_BUFFER_SIZE, "set u i %u\r", rate);
    return setCommand(commandBuffer);
}

int8 WiFly_setUartRaw(uint32 rate)
{
    xsnprintf(commandBuffer, WIFLY_COMMAND_BUFFER_SIZE, "set u r %u\r", rate);
    return setCommand(commandBuffer);
}

int8 WiFly_setUartFlow(uint8 flow)
{
    xsnprintf(commandBuffer, WIFLY_COMMAND_BUFFER_SIZE, "set u f %u\r", flow);
    return setCommand(commandBuffer);
}

int8 WiFly_setUartMode(uint16 mode)
{
    xsnprintf(commandBuffer, WIFLY_COMMAND_BUFFER_SIZE, "set u m 0x%x\r", mode);
    return setCommand(commandBuffer);
}

int8 WiFly_setUartTx(uint8 enabled)
{
    xsnprintf(commandBuffer, WIFLY_COMMAND_BUFFER_SIZE, "set u t %u\r", enabled);
    return setCommand(commandBuffer);
}

int8 WiFly_setWlanAuth(uint8 value)
{
    xsnprintf(commandBuffer, WIFLY_COMMAND_BUFFER_SIZE, "set w a %u\r", value);
    return setCommand(commandBuffer);
}

int8 WiFly_setWlanChannel(uint8 value)
{
    xsnprintf(commandBuffer, WIFLY_COMMAND_BUFFER_SIZE, "set w c %u\r", value);
    return setCommand(commandBuffer);
}

int8 WiFly_setWlanExtAntenna(uint8 enabled)
{
    xsnprintf(commandBuffer, WIFLY_COMMAND_BUFFER_SIZE, "set w e %u\r", enabled);
    return setCommand(commandBuffer);
}

int8 WiFly_setWlanJoin(uint8 value)
{
    xsnprintf(commandBuffer, WIFLY_COMMAND_BUFFER_SIZE, "set w j %d\r", value);
    return setCommand(commandBuffer);
}

int8 WiFly_setWlanHide(uint8 enabled)
{
    xsnprintf(commandBuffer, WIFLY_COMMAND_BUFFER_SIZE, "set w h %u\r", enabled);
    return setCommand(commandBuffer);
}

int8 WiFly_setWlanKey(char *value)
{
    if (strlen(value) != 0u)
    {
        xsnprintf(commandBuffer, WIFLY_COMMAND_BUFFER_SIZE, "set w k %s\r", value);
    }
    else
    {
        strcpy(commandBuffer, "set w k 0\r");
    }
    return setCommand(commandBuffer);
}

int8 WiFly_setWlanLinkmon(uint32 value)
{
    xsnprintf(commandBuffer, WIFLY_COMMAND_BUFFER_SIZE, "set w l %u\r", value);
    return setCommand(commandBuffer);
}

int8 WiFly_setWlanMask(uint32 mask)
{
    xsnprintf(commandBuffer, WIFLY_COMMAND_BUFFER_SIZE, "set w m 0x%x\r", mask);
    return setCommand(commandBuffer);
}

int8 WiFly_setWlanNum(uint8 value)
{
    xsnprintf(commandBuffer, WIFLY_COMMAND_BUFFER_SIZE, "set w n %u\r", value);
    return setCommand(commandBuffer);
}

int8 WiFly_setWlanPhrase(char *string)
{
    if (strlen(string) != 0u)
    {
        xsnprintf(commandBuffer, WIFLY_COMMAND_BUFFER_SIZE, "set w p %s\r", string);
    }
    else
    {
        strcpy(commandBuffer, "set w p 0\r");
    }
    return setCommand(commandBuffer);
}

int8 WiFly_setWlanRate(uint8 value)
{
    xsnprintf(commandBuffer, WIFLY_COMMAND_BUFFER_SIZE, "set w r %u\r", value);
    return setCommand(commandBuffer);
}

int8 WiFly_setWlanSsid(char *string)
{
    if (strlen(string) != 0u)
    {
        xsnprintf(commandBuffer, WIFLY_COMMAND_BUFFER_SIZE, "set w s %s\r", string);
    }
    else
    {
        strcpy(commandBuffer, "set w s 0\r");
    }
    return setCommand(commandBuffer);
}

int8 WiFly_setWlanTx(uint8 value)
{
    xsnprintf(commandBuffer, WIFLY_COMMAND_BUFFER_SIZE, "set w t %u\r", value);
    return setCommand(commandBuffer);
}

int8 WiFly_setWlanWindow(uint32 value)
{
    xsnprintf(commandBuffer, WIFLY_COMMAND_BUFFER_SIZE, "set w w %u\r", value);
    return setCommand(commandBuffer);
}

int8 WiFly_setQSensor(uint32 mask)
{
    xsnprintf(commandBuffer, WIFLY_COMMAND_BUFFER_SIZE, "set q s 0x%x\r", mask);
    return setCommand(commandBuffer);
}

int8 WiFly_setQPower(uint8 value)
{
    xsnprintf(commandBuffer, WIFLY_COMMAND_BUFFER_SIZE, "set q p %u\r", value);
    return setCommand(commandBuffer);
}

int8 WiFly_getAdhoc()
{
    return getCommand("get a\r");
}

int8 WiFly_getBroadcast()
{
    return getCommand("get b\r");
}

int8 WiFly_getCom()
{
    return getCommand("get c\r");
}

int8 WiFly_getDns()
{
    return getCommand("get d\r");
}

int8 WiFly_getEverything()
{
    return getCommand("get e\r");
}

int8 WiFly_getFtp()
{
    return getCommand("get f\r");
}

int8 WiFly_getIp(bool a)
{
    if (a)
    {
        return getCommand("get i a\r");
    }
    else
    {
        return getCommand("get i\r");
    }
}

int8 WiFly_getMac()
{
    return getCommand("get m\r");
}

int8 WiFly_getOption()
{
    return getCommand("get o\r");
}

int8 WiFly_getSys()
{
    return getCommand("get s\r");
}

int8 WiFly_getTime()
{
    return getCommand("get t\r");
}

int8 WiFly_getWlan()
{
    return getCommand("get w\r");
}

int8 WiFly_getUart()
{
    return getCommand("get u\r");
}

int8 WiFly_getVer()
{
    return getCommand("ver\r");
}

int8 WiFly_showBattery()
{
    return getCommand("show b\r");
}

int8 WiFly_showConnection()
{
    return getCommand("show c\r");
}

int8 WiFly_showIo()
{
    return getCommand("show i\r");
}

int8 WiFly_showNet(uint8 n)
{
    return getCommand("show n\r");
}

int8 WiFly_showRssi()
{
    return getCommand("show r\r");
}

int8 WiFly_showStats()
{
    return getCommand("show s\r");
}

int8 WiFly_showTime()
{
    return getCommand("show t\r");
}

int8 WiFly_showQ(uint8 num)
{
    xsnprintf(commandBuffer, WIFLY_COMMAND_BUFFER_SIZE, "show q %u\r", num);
    return getCommand(commandBuffer);
}

int8 WiFly_showQ0x1(uint8 mask)
{
    xsnprintf(commandBuffer, WIFLY_COMMAND_BUFFER_SIZE, "show q 0x1%u\r", mask);
    return getCommand(commandBuffer);
}

int8 WiFly_actionEnterCommandMode(uint8 isAfterReboot)
{
    uint8 retryCount;
    
    for (retryCount = 0u; 
         retryCount < WIFLY_COMMAND_MODE_ENTER_RETRY_ATTEMPTS; 
         retryCount++)
    {
        if (retryCount > 0u)    // if the first try fails introduce a guard times
        {
            if (isAfterReboot == 1u)
            {
                Timer_delayMs(WIFLY_REBOOT_GUARD_TIME);  // This delay is so characters aren't missed after a reboot.
            }
            else 
            {
                Timer_delayMs(WIFLY_COMMAND_MODE_GUARD_TIME);
            }
        }
    
        Uart_flush(wiflyUart);
        Uart_printf(wiflyUart, "%c%c%c",commChar,commChar,commChar);  // Print the command chars
        
        Timer_delayMs(WIFLY_COMMAND_MODE_GUARD_TIME);
        
        Uart_printf(wiflyUart, "\r\r");                               // Print 2 carriage return to make shure it has entered command mode
        Timer_delayMs(WIFLY_COMMAND_SETTLE_TIME);

        // This is used to determine whether command mode has been entered
        // successfully.
        Uart_printf(wiflyUart, "ver\r");
        Timer_delayMs(WIFLY_COMMAND_SETTLE_TIME);
        
        if (findInResponse("\r\r\n<", WIFLY_RESPONSE_TIMEOUT) == (int8)(0))
        {
            return findWiFlyVersion(WIFLY_RESPONSE_TIMEOUT);
        }
    }
    
    return (int8)(-1);
}

int8 WiFly_actionExitCommandMode()
{
    Uart_flush(wiflyUart);
    Uart_printf(wiflyUart, "exit\r");
    Timer_delayMs(WIFLY_COMMAND_SETTLE_TIME);
    
    if (findInResponse("EXIT", WIFLY_RESPONSE_TIMEOUT) == (int8)(0))
    { 
        return (int8)(0);
    }

    return (int8)(-1);
}

void WiFly_actionCloseTcpConnection()
{
    Uart_printf(wiflyUart, "close\r");
}

void WiFly_actionFactoryReset()
{
    Uart_printf(wiflyUart, "factory RESET\r");
}

void WiFly_actionJoin()
{
    Uart_printf(wiflyUart, "join\r");
}

void WiFly_actionJoinSsid(char *ssid)
{
    Uart_printf(wiflyUart, "join %s\r", ssid);
}

void WiFly_actionJoinNum(uint8 num)
{
    Uart_printf(wiflyUart, "join # %u\r", num);
}

void WiFly_actionLeaveAccesPoint()
{
    Uart_printf(wiflyUart, "leave\r");
}

void WiFly_actionLites()
{
    Uart_printf(wiflyUart, "lites\r");
}

void WiFly_actionLookup(char *hostname)
{
    Uart_printf(wiflyUart, "lookup %s\r", hostname);
}

void WiFly_actionOpenTcpConnection()
{
    Uart_printf(wiflyUart, "open\r");
}

void WiFly_actionOpenTcpConnectionAddr(char *addr, uint16 port)
{
    Uart_printf(wiflyUart, "open %s %u\r", addr, port);
}

void WiFly_actionPing(char *parameters, uint16 num)
{
    Uart_printf(wiflyUart, "ping  %s %u\r", parameters, num);
}

int8 WiFly_actionReboot()
{
    uint8 retryCount;
    
    for (retryCount = 0u; retryCount < WIFLY_SOFTWARE_REBOOT_RETRY_ATTEMPTS; retryCount++)
    {
        if (WiFly_actionEnterCommandMode(TRUE) == (int8)(-1))
        {
            return (int8)(-1);  // If the included retries have failed we give up
        }
        
        Uart_flush(wiflyUart);          // Empty buffers
        Uart_printf(wiflyUart, "reboot\r");  // Send command
        Timer_delayMs(WIFLY_COMMAND_SETTLE_TIME);
        //Uart_printf(wiflyUart, "reboot\r");
            
        // For some reason the full "*Reboot*" message doesn't always
        // seem to be received so we look for the later "*READY*" message instead.
    
        if (findInResponse("*Reboot*", WIFLY_RESPONSE_TIMEOUT) == (int8)(0))
        {
            wiFlyState = WiFly_ConnectionState_Disconnected;    // Now it also should be disconnected
            return (int8)(0);
        }
    }
    
    return (int8)(-1);
}

void WiFly_actionScan(uint32 time, bool passive)
{
    if (passive)
        Uart_printf(wiflyUart, "scan %u P\r", time);
    else
        Uart_printf(wiflyUart, "scan %u\r", time);
}

void WiFly_actionSleep()
{
    Uart_printf(wiflyUart, "sleep\r");
}

void WiFly_actionTime()
{
    Uart_printf(wiflyUart, "time\r");
}

int8 WiFly_actionAPmode(char* ssid, uint8 channel)
{
    return (int8)(-1);
}

void WiFly_fileIoDel(char *name)
{
    Uart_printf(wiflyUart, "del %s\r", name);
}

void WiFly_fileIoLoad(char *name)
{
    Uart_printf(wiflyUart, "load %s\r", name);
}

void WiFly_fileIoLs()
{
    Uart_printf(wiflyUart, "ls\r");
}

int8 WiFly_fileIoSaveDefault()
{
    return otherCommand("save config\r", "Storing in config");
}

int8 WiFly_fileIoSave(char *name)
{
    char buffer[100u];
    xsnprintf(commandBuffer, WIFLY_COMMAND_BUFFER_SIZE, "save %s\r", name);
    xsnprintf(buffer, 100u, "Storing in %s", name);
    return otherCommand(commandBuffer, buffer);
}

void WiFly_fileIoBootImage(uint16 num)
{
    Uart_printf(wiflyUart, "boot image %u\r", num);
}

void WiFly_fileIoFtpUpdate(char *name)
{
    Uart_printf(wiflyUart, "ftp update %s\r", name);
}

inline int8 WiFly_putchar(char c)
{
    return Uart_putchar(wiflyUart, c);
}

inline int8 WiFly_writeData(void *data, uint32 length)
{
    return Uart_writeData(wiflyUart, data, length);
}

inline int8 WiFly_getchar(char* c)
{
    return Uart_getchar(wiflyUart, c);
}

void WiFly_putat( void * ap, const char s)
{
    Uart_putchar(wiflyUart, s);
}

void WiFly_printf(char *fmt, ...)
{
    va_list arg_ptr;
    
    va_start(arg_ptr,fmt);
    xformat(Uart_putat, (void*)&wiflyUart, fmt, arg_ptr);
    va_end(arg_ptr);
}

void WiFly_setProcessFunction(void (* func)(char *))
{
    taskFunctionPointer = func;
}

void WiFly_setUdpProcessFunction(void (* func)(char *))
{
    udpTaskFunctionPointer = func;
}

void WiFly_setErrorFunction(void (* func)())
{
    Uart_setErrorFunction(wiflyUart, func);
}

inline void WiFly_processTask()
{
    //Uart_processTask(wiflyUart);
    
    static char receivedData;
    while (Uart_getchar(wiflyUart, &receivedData) == (int8)(0))
    {
        if (receivedData != WIFLY_COMMAND_CHAR)
        {
            if (receivedData == '*')
            {
                taskBuffer[taskBufferPos] = '\0';
                internalProcessTask(taskBuffer);
                taskBufferPos = 0u;
            }
        
            taskBuffer[taskBufferPos] = receivedData;
            if (taskBufferPos < (WIFLY_TASK_BUFFER_SIZE-1u))
            {
                taskBufferPos++;
            }
            else
            {
                //(*errorFunctionPointer1)();
                taskBufferPos = 0u;
            }
        } 
        else
        {
            taskBuffer[taskBufferPos] = '\0';
            if (taskBuffer[0u] != '*')
            {
                internalProcessTask(taskBuffer);
            }
            else    // this command was sent from a client -> ignore *
            {
                internalProcessTask(&(taskBuffer[1u]));
            }
            taskBufferPos = 0u;
        }
    }
}

void internalProcessTask(char *command)
{
    uint16 commandLength;
    
    if (command[0u] == '*')
    {
        commandLength = strlen(command);
        if (strncmp(command, commOpenString, 5u) == (int)0)// command[1u] == commOpenString[2u])
        {
            wiFlyState = WiFly_ConnectionState_Connected;
        }
        else if (strncmp(command, commCloseString, 5u) == (int)0)
        {
            wiFlyState = WiFly_ConnectionState_Disconnected;
        }
        else if (commandLength >= 5u) // status command, udp message can be attached
        {
            processStatusMessage(&(command[1u]));
            
            if (commandLength > 5u)
            {
                (*udpTaskFunctionPointer)(&(command[5u]));
            }
        }
    }
    else if (wiFlyState == WiFly_ConnectionState_Connected)
    {
        (*taskFunctionPointer)(command);
    }
    else // probably UDP stuff
    {
        (*udpTaskFunctionPointer)(command);
    }
}

void processStatusMessage(char *command)
{
    uint16 statusMessage;
    uint8  networkMode;
    uint8  wlanChannel;
    uint8  wlanAuthentication;
    uint8  wlanAssociation;
    uint8  tcpStatus;
    
    statusMessage = (uint16)Generic_hex2int(command, 4u);
    networkMode = (uint8)((statusMessage >> 14u) & 0b11u);
    wlanChannel = (uint8)((statusMessage >> 8u) & 0b1111u);
    wlanAuthentication = (uint8)((statusMessage >> 5u) & 0b1u);
    wlanAssociation = (uint8)((statusMessage >> 4u) & 0b1u);
    tcpStatus = (uint8)((statusMessage >> 0u) & 0b1111u);
    
    switch (networkMode)
    {
        case 2u: wiflyStatus.networkMode = WiFly_NetworkMode_AccessPoint; break;
        case 3u: wiflyStatus.networkMode = WiFly_NetworkMode_AdHoc; break;
        default: wiflyStatus.networkMode = WiFly_NetworkMode_Infrastructure;
    }
    wiflyStatus.wlanChannel = wlanChannel;
    wiflyStatus.wlanAuthenticationState = (WiFly_WlanAuthenticationState)wlanAuthentication;
    wiflyStatus.wlanAssociationState = (WiFly_WlanAssociationState)wlanAssociation;
    wiflyStatus.tcpStatus = (WiFly_TcpStatus)tcpStatus;
}

int8 findInResponse(const char* toMatch, uint32 timeout)
{
    uint32 timeoutTarget;
    int16 offset;
    char   byteRead;
    
    responseBufferPos = 0u;                                  // clear the responseBuffer
    timeoutTarget = Timeout_msecs() + timeout;               // set the timeout target
    
    for (offset = 0u; offset < strlen(toMatch); offset++)
    {
        while (WiFly_getchar(&byteRead) != (int8)(0))
        {
            // Wait with timeout
            if (timeout > 0u)
            {
                if (Timeout_msecs() > timeoutTarget)
                {
                    return (int8)(-1);
                }
            }
            Timer_delayMs(WIFLY_RESPONSE_WAIT_TIMEOUT); // Improves reliability, 1ms just makes this thing freak out
        }
        
        if (byteRead != toMatch[(uint16)offset]) 
        {
            offset = (int16)(0);
            
            responseBuffer[responseBufferPos] = byteRead;   // Store unmatched bytes
            if (responseBufferPos < WIFLY_RESPONSE_BUFFER_SIZE-1u)
                responseBufferPos++;
            else
                responseBufferPos = 0u;
            
            // Ignore character read if it's not a match for the start of the string
            if (byteRead != toMatch[(uint16)offset])
            {
                offset = (int16)(-1);
            }
        }
    }
    
    responseBuffer[responseBufferPos] = '\0';   // Add the string terminator
    return (int8)(0);
}

int8 findWiFlyVersion(uint32 timeout)
{
    uint32 timeoutTarget;
    char   byteRead;    
    uint8 i;
    
    for (i = 0u; i < WIFLY_VERSION_LENGTH; i++)
    {
        timeoutTarget = Timeout_msecs() + timeout;
        
        while (WiFly_getchar(&byteRead) != (int8)(0))
        {
            // Wait with timeout
            if (timeout > 0u)
            {
                if (Timeout_msecs() > timeoutTarget)
                {
                    return (int8)(-1);
                }
            }
            Timer_delayMs(WIFLY_RESPONSE_WAIT_TIMEOUT); // Improves reliability
        }
        
        wiFlyVersion[1u + i] = byteRead;
    }
    wiFlyVersion[0u] = '<';
    wiFlyVersion[WIFLY_VERSION_LENGTH + 1u] = '>';
    wiFlyVersion[WIFLY_VERSION_LENGTH + 2u] = '\0';
    
    return (int8)(0);
}

/*int8 responseMatched(const char* toMatch)
{
    int8 matchFound = (int8)(0);
    uint32 timeout;
    uint16 offset;
    char byteRead;
    
    for (offset = 0u; offset < strlen(toMatch); offset++)
    {
        timeout = Timeout_msecs();
        while (WiFly_getchar(&byteRead) != (int8)(0))
        {
            // Wait, with optional timeout
            if ((Timeout_msecs() - timeout) > WIFLY_RESPONSE_MATCH_TIMEOUT)
            {
                return (int8)(-1);
            }
            Timer_delayMs(WIFLY_RESPONSE_WAIT_TIMEOUT);
        }
        
        if (byteRead != toMatch[offset])
        {
            matchFound = (int8)(-1);
            break;
        }
    }
    
    return matchFound;
}*/

int8 setCommand(char* command)
{
    Uart_flush(wiflyUart);          // Empty buffers
    Uart_printf(wiflyUart, command);  // Send command
    Timer_delayMs(WIFLY_COMMAND_SETTLE_TIME);
    return findInResponse(WIFLY_SET_OK, WIFLY_RESPONSE_TIMEOUT);
}

int8 getCommand(char* command)
{
    Uart_flush(wiflyUart);          // Empty buffers
    Uart_printf(wiflyUart, command);  // Send command
    return findInResponse(wiFlyVersion, WIFLY_RESPONSE_TIMEOUT);
}

int8 otherCommand(char* command, char* awaitedResponse)
{
    Uart_flush(wiflyUart);          // Empty buffers
    Uart_printf(wiflyUart, command);  // Send command
    return findInResponse(awaitedResponse, WIFLY_RESPONSE_TIMEOUT);
}

char* WiFly_getResponse()
{
    return responseBuffer;
}

uint8 WiFly_isConnected()
{
    return (wiFlyState == WiFly_ConnectionState_Connected);
}

WiFly_Status WiFly_getStatus()
{
    return wiflyStatus;
}

int8 WiFly_setAdhocParams()
{
    if (WiFly_setWlanJoin(0u) == (int8)(-1))
        return (int8)(-1);
    if (WiFly_setWlanAuth(0u) == (int8)(-1))
        return (int8)(-1);
    if (WiFly_setIpDhcp(2u) == (int8)(-1))
        return (int8)(-1);
    
    return (int8)(0);
}

int8 WiFly_setInfrastructureParams()
{
    if (WiFly_setWlanChannel(0u) == (int8)(-1))
        return (int8)(-1);
    
    return (int8)(0);
}

int8 WiFly_createAdhocNetwork(char *ssid)
{
   // if (WiFly_actionReboot() == (int8)(-1))
//        return (int8)(-1);
    
    if (WiFly_actionEnterCommandMode(TRUE) == (int8)(-1))
        return (int8)(-1);
    
    if (WiFly_setWlanAuth(0u) == (int8)(-1))
        return (int8)(-1);
    
    if (WiFly_setWlanJoin(4u) == (int8)(-1))  // Turn on Adhoc mode
        return (int8)(-1);
    
    if (WiFly_setWlanSsid(ssid) == (int8)(-1))   // Set SSID of Adhoc Network
        return (int8)(-1);
    
    if (WiFly_setWlanChannel(1u) == (int8)(-1))   // Set Channel for Adhoc Network
        return (int8)(-1);
    
    if (WiFly_setIpAddress("169.254.1.1") == (int8)(-1)) // Set Ip for Adhoc Network
        return (int8)(-1);
    
    if (WiFly_setIpNetmask("255.255.0.0") == (int8)(-1))
        return (int8)(-1);
    
    if (WiFly_setIpDhcp(0u) == (int8)(-1))
        return (int8)(-1);
    
    if (WiFly_setCommRemote("0") == (int8)(-1))    // Turn off remote message
        return (int8)(-1);
    
    if (WiFly_fileIoSaveDefault() == (int8)(-1))
        return (int8)(-1);
    
    if (WiFly_actionReboot() == (int8)(-1))
        return (int8)(-1);
    
    return (int8)(0);
}

int8 WiFly_createAccessPoint(char *ssid)
{
 //   if (WiFly_actionReboot() == (int8)(-1))
  //      return (int8)(-1);
    
    if (WiFly_actionEnterCommandMode(TRUE) == (int8)(-1))
        return (int8)(-1);
    
    if (WiFly_setWlanAuth(0u) == (int8)(-1))
        return (int8)(-1);
    
    if (WiFly_setWlanJoin(7u) == (int8)(-1))  // Turn on Access point mode
        return (int8)(-1);
    
    if (WiFly_setWlanChannel(5u) == (int8)(-1))   // Set Channel for Access Point
        return (int8)(-1);
    
    if (WiFly_setWlanSsid(ssid) == (int8)(-1))   // Set SSID of Access Point
        return (int8)(-1);
    
    if (WiFly_setIpDhcp(4u) == (int8)(-1))  // Turn on DHCP server
        return (int8)(-1);
    
    if (WiFly_setIpAddress("169.254.1.1") == (int8)(-1)) // Set Ip for Access Point
        return (int8)(-1);
    
    if (WiFly_setIpNetmask("255.255.255.0") == (int8)(-1))
        return (int8)(-1);
    
    if (WiFly_setIpGateway("169.254.1.1") == (int8)(-1))
        return (int8)(-1);
    
    if (WiFly_fileIoSaveDefault() == (int8)(-1))
        return (int8)(-1);
    
    if (WiFly_actionReboot() == (int8)(-1))
        return (int8)(-1);
    
    return (int8)(0);
}
