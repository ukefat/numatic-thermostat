#pragma once

#include <stdarg.h>
#include <stdint.h>
void log_message2(uint8_t level, const char *format, ...);

#define LOG_LEVEL_DEBUG   4
#define LOG_LEVEL_INFO    3
#define LOG_LEVEL_WARN    2
#define LOG_LEVEL_ERROR   1
#define LOG_LEVEL_NONE    0

#define LOG_LEVEL LOG_LEVEL_INFO  // Set desired log level

#define LOG_DEBUG(fmt, ...) log_message2(LOG_LEVEL_DEBUG, "[DEBUG] " fmt "\r\n", ##__VA_ARGS__)
#define LOG_INFO(fmt, ...)  log_message2(LOG_LEVEL_INFO, "[INFO] " fmt "\r\n", ##__VA_ARGS__)
#define LOG_WARN(fmt, ...)  log_message2(LOG_LEVEL_WARN, "[WARN] " fmt "\r\n", ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) log_message2(LOG_LEVEL_ERROR, "[ERROR] " fmt "\r\n", ##__VA_ARGS__)



#if LOG_LEVEL_DEBUG
#define Debug(fmt, ...) LOG_DEBUG(fmt, ##__VA_ARGS__)
#else
	#define Debug(__info,...)
#endif

