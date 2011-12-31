# Object files needed
OBJS=main.o readnet.o net.o findtree.o bitmatrix.o job.o thread.o

# Compiler
CC=g++

# Flags: common for all
CFLAGS=-Wall -ansi

# Link libraries
#LIBS=-lpcrecpp
LIBS=-lpthread

all: main 

.cc.o:
	$(CC) $(CFLAGS) -c -o $@ $<

.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

main: $(OBJS)
	$(CC) $(CFLAGS) $(LIBS) -o $@ $^

clean:
	@for file in main $(OBJS) \
		; do \
		if [ -e $$file ]; then rm $$file ; fi; \
	done;

.PHONY: clean all
