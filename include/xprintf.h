/*------------------------------------------------------------------------*/
/* Universal string handler for user console interface  (C)ChaN, 2011     */
/* Modifed by Alexander Rössler 2013                                      */
/*------------------------------------------------------------------------*/
/**
 * @file xprintf.h
 * @author ChaN, modified by Alexander Rössler
 * @brief Small printf Library
 * @date 16-11-2013
 */
/** @defgroup xprintf Small printf Library for embedded systems
 * @ingroup Functional
 * @{
 */

#ifndef _STRFUNC
#define _STRFUNC

#define _USE_XFUNC_OUT	1	/* 1: Use output functions */
#define	_CR_CRLF		1	/* 1: Convert \n ==> \r\n in the output char */

#define _USE_XFUNC_IN	0	/* 1: Use input function */
#define	_LINE_ECHO		1	/* 1: Echo back input chars in xgets function */

#include <stdarg.h>

#if _USE_XFUNC_OUT
#define xdev_out(func) xfunc_out = (void(*)(unsigned char))(func)
extern void (*xfunc_out)(void*, char);

void xputc (void* ptr, char c);
/** Put a string to the default device
 *  @param ptr Pointer to custom data
 *  @param str Pointer to the string
 */
void xputs (void* ptr, const char* str);
/** Put a string to the specified device
 *  @param func Pointer to the output function
 *  @param ptr  Pointer to custom data
 *  @param str  Pointer to the string
 */
void xfputs (void(*func)(void*, char), void* ptr, const char* str);
/** Put a formatted string to the default device
 *  @param ptr  Pointer to custom data
 *  @param fmt  Pointer to the format string
 *  @param ...  Optional arguments
 */
void xprintf (void* ptr, const char* fmt, ...);
/** Put a formatted string to the memory
 *  @param buff Pointer to the output buffer 
 *  @param fmt  Pointer to the format string 
 *  @param ...  Optional arguments
 */
void xsprintf (char* buff, const char* fmt, ...);
/** Put a formatted string to the memory
 *  @param buff Pointer to the output buffer 
 *  @param size Size of the buffer
 *  @param fmt  Pointer to the format string 
 *  @param ...  Optional arguments
 */
void xsnprintf (char* buff, unsigned int size, const char* fmt,...);
/** Put a formatted string to the specified device 
 *  @param func Pointer to the output function 
 *  @param ptr  Pointer to custom data
 *  @param fmt  Pointer to the format string
 *  @param ...  Optional arguments
 */
void xfprintf (void(*func)(void*, char), void* ptr, const char* fmt, ...);
/** put a formatted string to the specified device
 *  @param func Pointer to the output function
 *  @param ptr  Pointer to custom data
 *  @param fmt  Pointer to the format string
 *  @param arg  Argument list
 */
void xformat (void (*func)(void*, char), void* ptr, const char* fmt, va_list arg);
/** Dump a line of binary dump
 *  @param ptr  Pointer to custom data
 *  @param buff Pointer to the array to be dumped
 *  @param addr Heading address value
 *  @param len  Number of items to be dumped
 *  @param width Size of the items (DF_CHAR, DF_SHORT, DF_LONG)
 */
void put_dump (void* ptr, const void* buff, long unsigned int addr, int len, int width);
#define DW_CHAR		sizeof(char)
#define DW_SHORT	sizeof(short)
#define DW_LONG		sizeof(long)
#endif

#if _USE_XFUNC_IN
#define xdev_in(func) xfunc_in = (unsigned char(*)(void))(func)
extern unsigned char (*xfunc_in)(void);
int xgets (char* buff, int len);
int xfgets (unsigned char (*func)(void), char* buff, int len);
int xatoi (char** str, long* res);
#endif

#endif

/**
 * @}
 */
