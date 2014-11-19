PROG=ftm
SRCS=$(sort $(wildcard *.c))
OBJS=$(SRCS:.c=.o)

CFLAGS=-Wall -g3 -O0

EZSDL_PATH=.
INC=-I$(EZSDL_PATH)

-include config.mak

all: $(PROG)

clean:
	rm -f $(OBJS) $(PROG)

$(PROG): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -lSDL -o $@ $^

%.o: %.c $(EZSDL_PATH)/ezsdl.h
	$(CC) $(CPPFLAGS) $(INC) $(CFLAGS) -c -o $@ $<
