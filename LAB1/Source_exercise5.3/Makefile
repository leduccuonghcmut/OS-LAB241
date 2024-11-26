
CC = gcc
CFLAGS = -g -Wall
TARGET = calc


SOURCES = calc.c

OBJECTS = $(SOURCES:.c=.o)


all: $(TARGET)


$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^


%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@


clean:
	rm -f $(OBJECTS) $(TARGET)


.PHONY: all clean
