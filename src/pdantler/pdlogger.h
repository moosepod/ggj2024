/** This file is managed by pdantler.

    Any changes you make to it will be overwritten next time pdantler is run.
 */

#define LOG_BUFFER_SIZE 400
#define LOG_BUFFER_PREFIX_SIZE 8

typedef void (*log_appender_fn)(const char *entry, void *udata);

extern void pdlogger_info(const char *message, ...);
extern void pdlogger_error(const char *message, ...);
extern void pdlogger_register_appender(log_appender_fn appender_fn,
                                       void *userdata);
