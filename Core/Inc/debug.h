#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <stdarg.h>
/**
 * putc function pointer definition
 */
typedef int (*putc_t)(int c);

/**
 * Light printf implementation
 * @param[in] putcf Putchar function to be used by Printf
 * @param[in] fmt Format string
 * @param[in] ... Parameters to print
 * @return the number of character printed
 */
int eprintf(putc_t putcf, const char * fmt, ...)
    __attribute__ (( format(printf, 2, 3) ));

/**
 * Light printf implementation
 * @param[in] putcf Putchar function to be used by Printf
 * @param[in] fmt Format string
 * @param[in] ap Parameters to print
 * @return the number of character printed
 */
int evprintf(putc_t putcf, const char * fmt, va_list ap);


#ifdef DEBUG_MODULE
#define DEBUG_FMT(fmt) DEBUG_MODULE ": " fmt
#endif

#ifndef DEBUG_FMT
#define DEBUG_FMT(FMT) FMT
#endif

#include "usart.h"
int debugUartPutchar(int c);
#define DEBUG_PRINT_UART(FMT, ...) eprintf(debugUartPutchar, FMT, ## __VA_ARGS__)

#endif