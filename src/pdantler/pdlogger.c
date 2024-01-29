/** This file is managed by pdantler.

    Any changes you make to it will be overwritten next time pdantler is run.

    Logger for logging on Playdate that can also handle logging in a
   non-playdate setup (eg offline tests)

    Inspired by https://github.com/empyreanx/pico_headers but simplified and
   adapted for Playdate
 */

#ifndef PDLOGGER_INCLUDE
#define PDLOGGER_INCLUDE
#include "pdlogger.h"
#include "printf.h"

static struct Logger {
  void (*log_appender_fn)(const char *entry, void *userdata);
  void *userdata;
} logger;

void pdlogger_register_appender(log_appender_fn appender_fn, void *userdata) {
  logger.log_appender_fn = appender_fn;
  logger.userdata = userdata;
}

void pdlogger_info(const char *message, ...) {
  if (logger.log_appender_fn) {
    char log_buffer[LOG_BUFFER_SIZE] = "[INFO ] ";
    va_list va;
    va_start(va, message);
    vsnprintf(log_buffer + LOG_BUFFER_PREFIX_SIZE,
              LOG_BUFFER_SIZE - LOG_BUFFER_PREFIX_SIZE, message, va);
    logger.log_appender_fn(log_buffer, logger.userdata);
    va_end(va);
  }
}

void pdlogger_error(const char *message, ...) {
  if (logger.log_appender_fn) {
    char log_buffer[LOG_BUFFER_SIZE] = "[ERROR] ";
    va_list va;
    va_start(va, message);
    vsnprintf(log_buffer + LOG_BUFFER_PREFIX_SIZE,
              LOG_BUFFER_SIZE - LOG_BUFFER_PREFIX_SIZE, message, va);
    logger.log_appender_fn(log_buffer, logger.userdata);
    va_end(va);
    logger.log_appender_fn(message, logger.userdata);
  }
}

// For use with printf.h
// The printf functions are unused so this can be a stub
void _putchar(char character) {}

#endif
