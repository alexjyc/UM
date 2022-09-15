#
# Makefile for the UM Test lab
# 
CC = gcc

IFLAGS   = -I/comp/40/build/include -I/usr/sup/cii40/include/cii
CFLAGS   = -g -std=gnu99 -Wall -Wextra -Werror -pedantic $(IFLAGS)
LDFLAGS  = -g -L/comp/40/build/lib -L/usr/sup/cii40/lib64
LDLIBS   = -lbitpack -l40locality -lcii40 -lm
INCLUDES = $(shell echo *.h)

EXECS    = main

all: $(EXECS)

main: main.o um.o memory_seg.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

# To get *any* .o file, compile its .c file with the following rule.
%.o: %.c $(INCLUDES)
	$(CC) $(CFLAGS) -c $< -o $@


clear:
	rm -f $(EXECS)  *.o

