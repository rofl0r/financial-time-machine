PROG=ftm
SRCS=$(sort $(wildcard *.c))
OBJS=$(SRCS:.c=.o)

CFLAGS=-Wall -g3 -O0

-include config.mak

all: $(PROG)

clean:
	rm -f $(OBJS) $(PROG)

$(PROG): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<
