#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <stdio.h>

/* Output stream for debug (stdout or stderr) */
#define DEBUG_STREAM stderr

/*
 * This function works like printf but: 
 * - uses DEBUG_STREAM for the output
 * - the level of verbosity can be controled through the Makefile
 */


#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"


#ifdef VERBOSE
#define PRINT_DEBUG(level, fmt, ...) do {                               \
        if(level<=VERBOSE){                                             \
            fprintf(DEBUG_STREAM, ANSI_COLOR_BLUE "%s:%d:", __FILE__, __LINE__); \
            fprintf(DEBUG_STREAM, ANSI_COLOR_MAGENTA "%s():", __func__); \
            fprintf(DEBUG_STREAM, ANSI_COLOR_RESET " " fmt, ##__VA_ARGS__); \
        }                                                               \
    } while (0)
#else
#define PRINT_DEBUG(level, fmt, ...)
#endif

#define PRINT_TEST_SUCCESS(fmt, ...) do {                               \
        fprintf(DEBUG_STREAM, ANSI_COLOR_GREEN "**SUCCESS** ");         \
        fprintf(DEBUG_STREAM, ANSI_COLOR_RESET "-- " fmt, ##__VA_ARGS__);            \
    } while (0)

#define PRINT_TEST_FAILED(fmt, ...) do {                                 \
        fprintf(DEBUG_STREAM, ANSI_COLOR_RED "**FAILED** ");          \
        fprintf(DEBUG_STREAM, ANSI_COLOR_RESET "-- " fmt, ##__VA_ARGS__); \
    } while (0)


    

#endif /* __DEBUG_H__ */
