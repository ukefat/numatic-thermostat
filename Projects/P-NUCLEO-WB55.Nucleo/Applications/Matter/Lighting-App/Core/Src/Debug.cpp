/*
 * Debug.cpp
 *
 *  Created on: Mar 3, 2025
 *      Author: kobru
 */
#include "Debug.h"
#include  <cstdint>
#include <cstdio>

void log_message2(uint8_t level, const char *format, ...) {
    if (level > LOG_LEVEL) {
        return;
    }

    char buffer[128];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    printf("%s", buffer);
}
