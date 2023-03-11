// This line **must** come **before** including <time.h> in order to
// bring in the POSIX functions such as `clock_gettime() from <time.h>`!
#define _POSIX_C_SOURCE 199309L
        
#include <time.h>
#include "_times.h"

/// Convert seconds to milliseconds
#define SEC_TO_MS(sec) ((sec)*1000)
/// Convert seconds to microseconds
#define SEC_TO_US(sec) ((sec)*1000000)
/// Convert seconds to nanoseconds
#define SEC_TO_NS(sec) ((sec)*1000000000)

/// Convert nanoseconds to seconds
#define NS_TO_SEC(ns)   ((ns)/1000000000)
/// Convert nanoseconds to milliseconds
#define NS_TO_MS(ns)    ((ns)/1000000)
/// Convert nanoseconds to microseconds
#define NS_TO_US(ns)    ((ns)/1000)

/// Get a time stamp in milliseconds.
long long int millis() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    long long int ms = SEC_TO_MS((long long int)ts.tv_sec) + NS_TO_MS((long long int)ts.tv_nsec);
    return ms;
}

/// Get a time stamp in microseconds.
long long int micros() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    long long int us = SEC_TO_US((long long int)ts.tv_sec) + NS_TO_US((long long int)ts.tv_nsec);
    return us;
}

/// Get a time stamp in nanoseconds.
long long int nanos() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    long long int ns = SEC_TO_NS((long long int)ts.tv_sec) + (long long int)ts.tv_nsec;
    return ns;
}

/// Get time required for time difference calculation itself. 
long long int delta() {
    return millis() - millis();
}
