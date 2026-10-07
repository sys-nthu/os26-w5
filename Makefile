# Worksheet 5 hands-on: build every program used in the hands-on part.
#
#   ./setup.sh     install gcc, make, strace, pstree, ps, python3, git, timeout (run once)
#   make           build all programs (the number is the step in the hands-on part)
#   make clean     remove the binaries
#
# -O0 -g keeps the compiled code close to the source, which helps when you
# read the strace output next to the program.
CC      = gcc
CFLAGS  ?= -O0 -g -Wall -Wextra

PROGS := 01-pipeline 02-numbers 02-sum 04-capture-noclose \
         04-capacity 04-worker 04-capture-broken 04-capture

all: $(PROGS)

%: %.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(PROGS) *.o

.PHONY: all clean
