/*
   filename: miniclog.c
*/
#include "miniclog.h"

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

logger_st *logger_init()
{
    logger_st *logger = (logger_st *) calloc(1, sizeof(logger_st));
    if (!logger) {
        exit(1);
    }
    logger->handlers = NULL;
    logger->handlers_num = 0;
    return logger;
}

void logger_deinit(logger_st *logger)
{
    for (int i = 0; i < logger->handlers_num; ++i) {
        free(logger->handlers[i]->filters);
        free(logger->handlers[i]);
    }
    free(logger->handlers);
    free(logger);
    logger = NULL;
}

void add_handler(logger_st *logger, handler_st *handler)
{
    int handlers_num = logger->handlers_num;
    handler_st **new_handlers = (handler_st **) realloc(
        logger->handlers, (handlers_num + 1) * sizeof(logger_st *));
    if (!new_handlers) {  // if realloc failed, the original pointer needs to be
                          // freed
        free(logger->handlers);
        exit(1);
    }
    new_handlers[handlers_num] = handler;
    logger->handlers_num++;
    logger->handlers = new_handlers;
}

handler_st *handler_init(FILE *outstream, const logger_level_et level)
{
    handler_st *handler = (handler_st *) calloc(1, sizeof(handler_st));
    if (!handler) {
        exit(1);
    }
    handler->level = level;
    handler->stream = outstream;
    handler->filters = NULL;
    handler->filters_num = 0;
    handler->format = "%(message)";
    return handler;
}

void set_level(handler_st *handler, const logger_level_et level)
{
    handler->level = level;
}

void set_stream(handler_st *handler, FILE *stream)
{
    handler->stream = stream;
}

void set_format(handler_st *handler, const char *format)
{
    handler->format = format;
}

#define MINICLOG_FL(a, b) a,
enum { MINICLOG_FORMAT_LABELS };
#undef MINICLOG_FL

#define MINICLOG_FL(a, b) b,
static const char *const format_labels[] = {MINICLOG_FORMAT_LABELS};
#undef MINICLOG_FL

#define MINICLOG_LL(a, b) b,
static const char *const level_labels[] = {MINICLOG_LEVEL_LABELS};
#undef MINICLOG_LL

void add_filter(handler_st *handler, filter_ft filter)
{
    int filters_num = handler->filters_num;
    filter_ft *new_filters = (filter_ft *) realloc(
        handler->filters, (filters_num + 1) * sizeof(filter));
    if (!new_filters) {  // if realloc failed, the original pointer needs to be
                         // freed
        free(handler->filters);
        exit(1);
    }
    new_filters[filters_num] = filter;
    handler->filters_num++;
    handler->filters = new_filters;
}

static char *get_time(char *buffer, size_t size)
{
    time_t timer = time(NULL);
    struct tm *tm_info = localtime(&timer);
    size_t success = strftime(buffer, size, "%Y-%m-%d %H:%M:%S", tm_info);

    if (!success) {
        fprintf(stderr,
                "The buffer size (%ld bytes) of get_time is not enough.\n",
                size);
        return "(no time)";
    }
    return buffer;
}

void logger_printf(logger_st *logger,
                   logger_level_et level,
                   const char *file,
                   const char *func,
                   unsigned int line,
                   const char *format,
                   ...)
{
    char *message = (char *) calloc(MINICLOG_BUFFER_SIZE, sizeof(char));
    if (!message) {
        exit(1);
    }
    va_list args;
    va_start(args, format);
    int len = vsnprintf(message, MINICLOG_BUFFER_SIZE, format, args);
    if (MINICLOG_BUFFER_SIZE <= len) {
        char *new_message = (char *) realloc(message, (len + 1) * sizeof(char));
        if (!new_message) {  // if realloc failed, the original pointer needs to
                             // be freed
            free(message);
            exit(1);
        }
        va_start(args, format);
        vsnprintf(new_message, len + 1, format, args);
        message = new_message;
    }
    handler_st *handler;
    for (int i = 0; i < logger->handlers_num; ++i) {
        handler = logger->handlers[i];

        /* skip the threshold level of handler */
        if (level < handler->level) {
            goto free_msg;
        }

        /* filter the level or message */
        for (int j = 0; j < handler->filters_num; ++j) {
            if (!handler->filters[j](level, message)) {
                goto free_msg;
            }
        }

        /* format the output */
        const char *f = handler->format;
        for (int j = 0; f[j] != '\0'; ++j) {
            if (starts_with(f + j, format_labels[FL_MESSAGE])) {
                fprintf(handler->stream, "%s", message);
                j += strlen(format_labels[FL_MESSAGE]) - 1;
            } else if (starts_with(f + j, format_labels[FL_FILE])) {
                fprintf(handler->stream, "%s", file);
                j += strlen(format_labels[FL_FILE]) - 1;
            } else if (starts_with(f + j, format_labels[FL_FUNC])) {
                fprintf(handler->stream, "%s", func);
                j += strlen(format_labels[FL_FUNC]) - 1;
            } else if (starts_with(f + j, format_labels[FL_LINE])) {
                fprintf(handler->stream, "%d", line);
                j += strlen(format_labels[FL_LINE]) - 1;
            } else if (starts_with(f + j, format_labels[FL_TIME])) {
                char buffer[20] = {};
                fprintf(handler->stream, "%s",
                        get_time(buffer, sizeof(buffer)));
                j += strlen(format_labels[FL_TIME]) - 1;
            } else if (starts_with(f + j, format_labels[FL_LEVEL])) {
                fprintf(handler->stream, "%s", level_labels[level]);
                j += strlen(format_labels[FL_LEVEL]) - 1;
            } else {
                fprintf(handler->stream, "%c", f[j]);
            }
        }
    }
free_msg:
    free(message);
    if (level == MINICLOG_FATAL) {
        exit(1);
    }
}

bool starts_with(const char *str, const char *pre)
{
    size_t lenpre = strlen(pre), lenstr = strlen(str);
    return lenstr < lenpre ? false : memcmp(pre, str, lenpre) == 0;
}
