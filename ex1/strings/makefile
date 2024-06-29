# 316443902 Nir Koren

# Makefile

# Variables
CC=gcc
CFLAGS=-no-pie -lm -g -msse4.2
SOURCES=main.c b64.c hamming.s
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=program

# Default target
all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(CFLAGS)

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

%.o: %.s
	$(CC) -c $< -o $@ $(CFLAGS)


run:
	./$(EXECUTABLE)

# PHONY
.PHONY: all clean