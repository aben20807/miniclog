# miniclog

Simple logger with file stream handler, output filter, and user-defined 
format. Inspired by https://github.com/sysprog21/threaded-logger and logging 
package of Python

## Features

+ file stream handler
+ output filter
+ user-defined format

## Examples

+ file_stream_handler:

```c
#include <stdio.h>
#include "../miniclog.h"

int main(int argc, char *argv[]) {

    logger_st* l = logger_init();
    handler_st* h1 = handler_init(stdout, MINICLOG_NOTSET);
    handler_st* h2 = handler_init(stderr, MINICLOG_NOTSET);
    add_handler(l, h1);
    add_handler(l, h2);
    LOG_INFO(l, "OuO number: %d\n", 42);
    logger_deinit(l);
    return 0;
}
```

```bash
$ cd examples
$ make file_stream_handler
$ ./file_stream_handler
OuO number: 42
OuO number: 42
```

+ output_filter:

```c
#include <stdio.h>
#include "../miniclog.h"

bool foo(logger_level_et level, const char *msg) {
    if (level < MINICLOG_DEBUG) {
        return false;
    }
    return true;
}

bool bar(logger_level_et level, const char *msg) {
    if (starts_with(msg, "OuO")) {
        return true;
    }
    return false;
}

int main(int argc, char *argv[]) {

    logger_st* l = logger_init();
    handler_st* h1 = handler_init(stdout, MINICLOG_NOTSET);
    add_filter(h1, foo);
    add_filter(h1, bar);
    add_handler(l, h1);
    LOG(l, "OuO number 1: %d\n", 42); // skipped because of foo
    LOG_INFO(l, "OuO number 2: %d\n", 42);
    logger_deinit(l);
    return 0;
}
```

```bash
$ cd examples
$ make output_filter
$ ./output_filter
OuO number 2: 42
```

+ user_defined_format:

```c
#include <stdio.h>
#include "../miniclog.h"

int main(int argc, char *argv[]) {

    logger_st* l = logger_init();
    handler_st* h1 = handler_init(stdout, MINICLOG_NOTSET);
    set_format(h1, "[%(time)] %(file):%(func):%(line):%(message)");
    add_handler(l, h1);
    LOG_INFO(l, "OuO number: %d\n", 42);
    logger_deinit(l);
    return 0;
}
```

```bash
$ cd examples
$ make user_defined_format
$ ./user_defined_format
[2022-06-02 16:22:44] user_defined_format.c:main:10:OuO number: 42
```
