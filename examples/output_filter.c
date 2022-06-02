#include <stdio.h>
#include "../miniclog.h"

bool foo(logger_level_et level, const char *msg)
{
    if (level < MINICLOG_DEBUG) {
        return false;
    }
    return true;
}

bool bar(logger_level_et level, const char *msg)
{
    if (starts_with(msg, "OuO")) {
        return true;
    }
    return false;
}

int main(int argc, char *argv[])
{
    logger_st *l = logger_init();
    handler_st *h1 = handler_init(stdout, MINICLOG_NOTSET);
    add_filter(h1, foo);
    add_filter(h1, bar);
    add_handler(l, h1);
    LOG(l, "OuO number 1: %d\n", 42);
    LOG_INFO(l, "OuO number 2: %d\n", 42);
    logger_deinit(l);
    return 0;
}
