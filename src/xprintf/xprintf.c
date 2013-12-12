/*------------------------------------------------------------------------/
/  Universal string handler for user console interface
/-------------------------------------------------------------------------/
/
/  Copyright (C) 2011, ChaN, all right reserved.
/  Modified by Alexander Rössler
/
/ * This software is a free software and there is NO WARRANTY.
/ * No restriction on use. You can use, modify and redistribute it for
/   personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.
/ * Redistributions of source code must retain the above copyright notice.
/
/-------------------------------------------------------------------------*/

#include "xprintf.h"


#if _USE_XFUNC_OUT
void (*xfunc_out)(void*, char);	/* Pointer to the output stream */
static char *outptr;

/*----------------------------------------------*/
/* Put a character                              */
/*----------------------------------------------*/

void xputc (void* ptr, char c)
{
    if (_CR_CRLF && c == '\n') xputc(ptr, '\r');		/* CR -> CRLF */

    if (outptr) {
        *outptr++ = c;
        return;
    }
    
    if (xfunc_out) xfunc_out(ptr, c);
}



/*----------------------------------------------*/
/* Put a null-terminated string                 */
/*----------------------------------------------*/

void xputs (
    void* ptr,
    const char* str
)
{
    while (*str)
        xputc(ptr, *str++);
}

void xfputs (
    void(*func)(void*, char),
    void* ptr,
    const char* str
)
{
    void (*pf)(void*, char);


    pf = xfunc_out;     /* Save current output device */
    xfunc_out = func;   /* Switch output to specified device */
    while (*str)        /* Put the string */
        xputc(ptr, *str++);
    xfunc_out = pf;     /* Restore output device */
}



/*----------------------------------------------*/
/* Formatted string output                      */
/*----------------------------------------------*/
/*  xprintf("%d", 1234);			"1234"
    xprintf("%6d,%3d%%", -200, 5);	"  -200,  5%"
    xprintf("%-6u", 100);			"100   "
    xprintf("%ld", 12345678L);		"12345678"
    xprintf("%04x", 0xA3);			"00a3"
    xprintf("%08LX", 0x123ABC);		"00123ABC"
    xprintf("%016b", 0x550F);		"0101010100001111"
    xprintf("%s", "String");		"String"
    xprintf("%-4s", "abc");			"abc "
    xprintf("%4s", "abc");			" abc"
    xprintf("%c", 'a');				"a"
    xprintf("%f", 10.0);            <xprintf lacks floating point support>
*/

/** Formats a string
 *  @param ptr  Pointer to custom data
 *  @param fmt  Pointer to the format string
 *  @param arp  Pointer to arguments
 */
static 
void xvprintf (
    void* ptr,
    const char* fmt,
    va_list arp
)
{
    uint32 r, i, j, w, f;
    uint64 v;
    char s[16u], c, d, *p;


    for (;;) {
        c = *fmt++;                 /* Get a char */
        if (!c) break;              /* End of format? */
        if (c != '%') {             /* Pass through it if not a % sequense */
            xputc(ptr, c); continue;
        }
        f = 0u;
        c = *fmt++;                 /* Get first char of the sequense */
        if (c == '0') {             /* Flag: '0' padded */
            f = 1u; c = *fmt++;
        } else {
            if (c == '-') {         /* Flag: left justified */
                f = 2u; c = *fmt++;
            }
        }
        for (w = 0u; c >= '0' && c <= '9'; c = *fmt++)  /* Minimum width */
            w = w * 10u + c - '0';
        if (c == 'l' || c == 'L') { /* Prefix: Size is long int */
            f |= 4u; c = *fmt++;
        }
        if (!c) break;              /* End of format? */
        d = c;
        if (d >= 'a') d -= 0x20;
        switch (d) {                /* Type is... */
        case 'S' :                  /* String */
            p = va_arg(arp, char*);
            for (j = 0u; p[j]; j++) ;
            while (!(f & 2u) && j++ < w) xputc(ptr, ' ');
            xputs(ptr, p);
            while (j++ < w) xputc(ptr, ' ');
            continue;
        case 'C' :                  /* Character */
            xputc(ptr, (char)va_arg(arp, int)); continue;
        case 'B' :                  /* Binary */
            r = 2u; break;
        case 'O' :                  /* Octal */
            r = 8u; break;
        case 'D' :                  /* Signed decimal */
        case 'U' :                  /* Unsigned decimal */
            r = 10u; break;
        case 'X' :                  /* Hexdecimal */
            r = 16u; break;
        default:                    /* Unknown type (passthrough) */
            xputc(ptr, c); continue;
        }

        /* Get an argument and put it in numeral */
        v = (f & 4u) ? va_arg(arp, long) : ((d == 'D') ? (long)va_arg(arp, int) : (long)va_arg(arp, unsigned int));
        if (d == 'D' && (v & 0x80000000u)) {
            v = 0u - v;
            f |= 8u;
        }
        i = 0u;
        do {
            d = (char)(v % r); v /= r;
            if (d > 9) d += (c == 'x') ? 0x27 : 0x07;
            s[i++] = d + '0';
        } while (v && i < sizeof(s));
        if (f & 8u) s[i++] = '-';
        j = i; d = (f & 1u) ? '0' : ' ';
        while (!(f & 2u) && j++ < w) xputc(ptr, d);
        do xputc(ptr, s[--i]); while(i);
        while (j++ < w) xputc(ptr, ' ');
    }
}

void xprintf (
    void* ptr,
    const char* fmt,
    ...
)
{
    va_list arp;

    va_start(arp, fmt);
    xvprintf(ptr, fmt, arp);
    va_end(arp);
}

void xsprintf (
    char* buff,
    const char* fmt,
    ...
)
{
    va_list arp;


    outptr = buff;		/* Switch destination for memory */

    va_start(arp, fmt);
    xvprintf(0, fmt, arp);
    va_end(arp);

    *outptr = 0;		/* Terminate output string with a \0 */
    outptr = 0;			/* Switch destination for device */
}

void xsnprintf (
    char* buff,
    unsigned int size,
    const char* fmt,
    ...
)
{
    va_list arp;

    outptr = buff;      /* Switch destination for memory */

    va_start(arp, fmt);
    xvprintf(0, fmt, arp);
    va_end(arp);

    *outptr = 0;        /* Terminate output string with a \0 */
    outptr = 0;         /* Switch destination for device */
}

void xfprintf (
    void(*func)(void*, char),
    void* ptr,
    const char* fmt,
    ...
)
{
    va_list arp;
    void (*pf)(void*, char);

    pf = xfunc_out;		/* Save current output device */
    xfunc_out = func;	/* Switch output to specified device */

    va_start(arp, fmt);
    xvprintf(ptr, fmt, arp);
    va_end(arp);

    xfunc_out = pf;
}

void xformat (
    void (*func)(void*, char),
    void* ptr,
    const char* fmt,
    va_list arg
)
{
    void (*pf)(void*, char);

    pf = xfunc_out;     /* Save current output device */
    xfunc_out = func;   /* Switch output to specified device */
    
    xvprintf(ptr, fmt, arg);
    
    xfunc_out = pf;     /* Restore output device */
}

/*----------------------------------------------*/
/* Dump a line of binary dump                   */
/*----------------------------------------------*/

void put_dump (
    void* ptr,
    const void* buff,
    uint64 addr,
    uint32 len,
    uint32 width
)
{
    uint32 i;
    const uint8 *bp;
    const uint16 *sp;
    const uint64 *lp;


    xprintf(ptr, "%08lX ", addr);       /* address */

    switch (width) {
    case DW_CHAR:
        bp = buff;
        for (i = 0u; i < len; i++)      /* Hexdecimal dump */
            xprintf(ptr, " %02X", bp[i]);
        xputc(ptr, ' ');
        for (i = 0u; i < len; i++)      /* ASCII dump */
            xputc(ptr, (bp[i] >= ' ' && bp[i] <= '~') ? bp[i] : '.');
        break;
    case DW_SHORT:
        sp = buff;
        do                              /* Hexdecimal dump */
            xprintf(ptr, " %04X", *sp++);
        while (--len);
        break;
    case DW_LONG:
        lp = buff;
        do                              /* Hexdecimal dump */
            xprintf(ptr, " %08LX", *lp++);
        while (--len);
        break;
    }

    xputc(ptr, '\n');
}

#endif /* _USE_XFUNC_OUT */



#if _USE_XFUNC_IN
uint8 (*xfunc_in)(void);    /* Pointer to the input stream */

/*----------------------------------------------*/
/* Get a line from the input                    */
/*----------------------------------------------*/

uint8 xgets (     /* 0:End of stream, 1:A line arrived */
    char* buff, /* Pointer to the buffer */
    uint32 len     /* Buffer length */
)
{
    char c;
    uint8 i;


    if (!xfunc_in) return 0u;       /* No input function specified */

    i = 0;
    for (;;) {
        c = xfunc_in();             /* Get a char from the incoming stream */
        if (!c) return 0u;          /* End of stream? */
        if (c == '\r') break;       /* End of line? */
        if (c == '\b' && i) {       /* Back space? */
            i--;
            if (_LINE_ECHO) xputc(NULL, c);
            continue;
        }
        if (c >= ' ' && i < len - 1u) {  /* Visible chars */
            buff[i++] = c;
            if (_LINE_ECHO) xputc(NULL, c);
        }
    }
    buff[i] = 0;    /* Terminate with a \0 */
    if (_LINE_ECHO) xputc(NULL, '\n');
    return 1u;
}


uint8 xfgets (    /* 0:End of stream, 1:A line arrived */
    uint8 (*func)(void),    /* Pointer to the input stream function */
    char* buff,     /* Pointer to the buffer */
    uint32 len      /* Buffer length */
)
{
    uint8 (*pf)(void);
    uint8 n;


    pf = xfunc_in;          /* Save current input device */
    xfunc_in = func;        /* Switch input to specified device */
    n = xgets(buff, len);   /* Get a line */
    xfunc_in = pf;          /* Restore input device */

    return n;
}


/*----------------------------------------------*/
/* Get a value of the string                    */
/*----------------------------------------------*/
/*	"123 -5   0x3ff 0b1111 0377  w "
	    ^                           1st call returns 123 and next ptr
	       ^                        2nd call returns -5 and next ptr
                   ^                3rd call returns 1023 and next ptr
                          ^         4th call returns 15 and next ptr
                               ^    5th call returns 255 and next ptr
                                  ^ 6th call fails and returns 0
*/

uint8 xatoi (       /* 0:Failed, 1:Successful */
    char **str,     /* Pointer to pointer to the string */
    int64 *res       /* Pointer to the valiable to store the value */
)
{
    uint64 val;
    uint8 c, r, s = 0u;


    *res = 0;

    while ((c = **str) == ' ') (*str)++;    /* Skip leading spaces */

    if (c == '-') {     /* negative? */
        s = 1u;
        c = *(++(*str));
    }

    if (c == '0') {
        c = *(++(*str));
        switch (c) {
        case 'x':       /* hexdecimal */
            r = 16u; c = *(++(*str));
            break;
        case 'b':       /* binary */
            r = 2u; c = *(++(*str));
            break;
        default:
            if (c <= ' ') return 1u;    /* single zero */
            if (c < '0' || c > '9') return 0u;  /* invalid char */
            r = 8u;     /* octal */
        }
    } else {
        if (c < '0' || c > '9') return 0u;  /* EOL or invalid char */
        r = 10u;        /* decimal */
    }

    val = 0u;
    while (c > ' ') {
        if (c >= 'a') c -= 0x20u;
        c -= '0';
        if (c >= 17u) {
            c -= 7u;
            if (c <= 9u) return 0u;  /* invalid char */
        }
        if (c >= r) return 0u;      /* invalid char for current radix */
        val = val * r + c;
        c = *(++(*str));
    }
    if (s) val = 0u - val;           /* apply sign if needed */

    *res = (int64)val;
    return 1u;
}

#endif /* _USE_XFUNC_IN */
