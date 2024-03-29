CC = gcc
CFLAGS = -std=c11 -MMD -Wall -Wextra -pedantic -g

SRCS := $(wildcard *.c)

OBJS := $(patsubst %.c, %.o, $(SRCS))

PROGS := $(patsubst %.c, %.d, $(SRCS))

TARGET = main

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	gcc $(CFLAGS) -o $@ $^
 
clean:
	rm -f ${PROGS} ${OBJS} $(TARGET)

run:
	./main < input.txt > output.txt
	./ic22int output.txt

debug:
	./main < input.txt > output.txt
	./ic22int output.txt -v
