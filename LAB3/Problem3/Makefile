# Compiler and linker settings
CC = gcc
CFLAGS = -g -pthread  # Enable pthreads and debugging symbols
LDFLAGS = -pthread    # Link pthread library

# Program settings
EXEC = logbuf    # Name of the output executable
SRC = logbuf.c        # Source file (thay đổi từ main.c thành logbuf.c)
OBJ = logbuf.o        # Object file (thay đổi từ main.o thành logbuf.o)

# Default target: build the program
all: $(EXEC)

# Rule to build the executable
$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $(EXEC) $(LDFLAGS)

# Rule to build object file from source file
$(OBJ): $(SRC)
	$(CC) $(CFLAGS) -c $(SRC)

# Clean up object files and executable
clean:
	rm -f $(OBJ) $(EXEC)

# Phony targets (non-file targets)
.PHONY: all clean
