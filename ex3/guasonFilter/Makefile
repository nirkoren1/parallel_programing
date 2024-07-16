# Makefile for Gaussian Filter Project

# Compiler and compiler flags
CC=gcc
CFLAGS=-Wall -fopenmp

# Linker flags
LDFLAGS=-lm

# Source files
SRC=main.c guassonFilter.c guassonFilterParallel.c

# Header files
HEADERS=guassonFilter.h guassonFilterParallel.h stb_image_write.h stb_image.h

# Object files
OBJS=$(SRC:.c=.o)

# Output executable
TARGET=guasson_filter

# Default target
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# To obtain object files
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up
clean:
	rm -f $(OBJS) $(TARGET)

# Phony targets
.PHONY: all clean
