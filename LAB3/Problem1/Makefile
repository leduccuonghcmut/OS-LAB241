CC = gcc
CFLAGS = -pthread
TARGET = seqlock

all: $(TARGET)

$(TARGET): seqlock.c
	$(CC) $(CFLAGS) -o $(TARGET) seqlock.c

clean:
	rm -f $(TARGET)
