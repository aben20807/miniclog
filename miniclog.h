/*
   filename: miniclog.h

   Simple logger with file stream handler, output filter, and user-defined
   format. Inspired by https://github.com/sysprog21/threaded-logger and logging
   package of Python
*/
#pragma once
#include <stdbool.h>
#include <stdio.h>

#define MINICLOG_BUFFER_SIZE 32

#define MINICLOG_LEVEL_LABELS              \
    MINICLOG_LL(MINICLOG_NOTSET, "NOTSET") \
    MINICLOG_LL(MINICLOG_DEBUG, "DEBUG")   \
    MINICLOG_LL(MINICLOG_INFO, "INFO")     \
    MINICLOG_LL(MINICLOG_WARN, "WARN")     \
    MINICLOG_LL(MINICLOG_ERROR, "ERROR")   \
    MINICLOG_LL(MINICLOG_FATAL, "FATAL")

/* Note: FATAL level will exit with non-zero status (1) */

#define MINICLOG_LL(a, b) a,
typedef enum { MINICLOG_LEVEL_LABELS } logger_level_et;
#undef MINICLOG_LL

typedef bool (*filter_ft)(logger_level_et level, const char *msg);

typedef struct logger_s logger_st;
typedef struct handler_s handler_st;

struct logger_s {
    handler_st **handlers;
    int handlers_num;
};

struct handler_s {
    FILE *stream;          /* log output stream. default: stdout */
    logger_level_et level; /* skip msg whose level is less than threshold.
                              default: NOTSET */
    filter_ft *filters;    /* the filters list to filter the log */
    int filters_num;       /* the number of the filters */
    const char *format;    /* the format string. defualt: "%(message)" */
};

// Format:
//   %(message): the log message
//   %(file): src file
//   %(func): function name
//   %(line): line of src file
//   %(time): log time
//   %(level): level label
// For example:
//   [%(time)] [%(level)] %(file):%(func):%(line):%(message)

#define MINICLOG_FORMAT_LABELS            \
    MINICLOG_FL(FL_MESSAGE, "%(message)") \
    MINICLOG_FL(FL_FILE, "%(file)")       \
    MINICLOG_FL(FL_FUNC, "%(func)")       \
    MINICLOG_FL(FL_LINE, "%(line)")       \
    MINICLOG_FL(FL_TIME, "%(time)")       \
    MINICLOG_FL(FL_LEVEL, "%(level)")

logger_st *logger_init();
void logger_deinit(logger_st *logger);
void add_handler(logger_st *logger, handler_st *handler);

handler_st *handler_init(FILE *outstream, const logger_level_et level);
void set_level(handler_st *handler, const logger_level_et level);
void set_stream(handler_st *handler, FILE *stream);
void set_format(handler_st *handler, const char *format);
void add_filter(handler_st *handler, filter_ft filter);

void logger_printf(logger_st *logger,
                   logger_level_et level,
                   const char *file,
                   const char *func,
                   unsigned int line,
                   const char *format,
                   ...);

// Utilities
bool starts_with(const char *str, const char *pre);

// Macros
#define LOG(logger, fmt, ...)                                                \
    logger_printf(logger, MINICLOG_NOTSET, __FILE__, __FUNCTION__, __LINE__, \
                  fmt, ##__VA_ARGS__)
#define LOG_DEBUG(logger, fmt, ...)                                         \
    logger_printf(logger, MINICLOG_DEBUG, __FILE__, __FUNCTION__, __LINE__, \
                  fmt, ##__VA_ARGS__)
#define LOG_INFO(logger, fmt, ...)                                         \
    logger_printf(logger, MINICLOG_INFO, __FILE__, __FUNCTION__, __LINE__, \
                  fmt, ##__VA_ARGS__)
#define LOG_WARN(logger, fmt, ...)                                         \
    logger_printf(logger, MINICLOG_WARN, __FILE__, __FUNCTION__, __LINE__, \
                  fmt, ##__VA_ARGS__)
#define LOG_ERROR(logger, fmt, ...)                                         \
    logger_printf(logger, MINICLOG_ERROR, __FILE__, __FUNCTION__, __LINE__, \
                  fmt, ##__VA_ARGS__)
#define LOG_FATAL(logger, fmt, ...)                                         \
    logger_printf(logger, MINICLOG_FATAL, __FILE__, __FUNCTION__, __LINE__, \
                  fmt, ##__VA_ARGS__)
