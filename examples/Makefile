CC ?= gcc8
CFLAGS ?= -Wall -O2 -std=gnu99 -D_GNU_SOURCE

OBJS := miniclog.o
deps := $(OBJS:%.o=%.o.d)

EXAMPLES := \
			file_stream_handler \
			output_filter \
			user_defined_format


all: $(EXAMPLES)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ -MMD -MF $@.d $<

miniclog.o: ../miniclog.c
	$(CC) $(CFLAGS) -c -o $@ -MMD -MF $@.d $<

%: $(OBJS) %.c
	$(CC) -o $@ $^

clean:
	rm -f $(EXAMPLES) *.o *.d
