#include <stdio.h>
#include "../miniclog.h"

int main(int argc, char *argv[])
{
    logger_st *l = logger_init();
    handler_st *h1 = handler_init(stdout, MINICLOG_NOTSET);
    set_format(h1, "[%(time)] [%(level)] %(file):%(func):%(line):%(message)");
    add_handler(l, h1);
    LOG_INFO(l, "OuO number: %d\n", 42);
    logger_deinit(l);
    return 0;
}
