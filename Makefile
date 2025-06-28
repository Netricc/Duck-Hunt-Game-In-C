# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
LDFLAGS = -lcsfml-graphics -lcsfml-window -lcsfml-system -lm

# Source files
SRC = src/main.c src/assets.c src/dog.c src/duck.c

# Object files, replacing .c with .o
OBJ = $(SRC:.c=.o)

# Executable name
EXEC = duckhunt.exe

# Default target
all: $(EXEC)

# Link the executable
$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

# Compile source files to object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean object files and executable
clean:
	rm -f $(OBJ) $(EXEC)

.PHONY: all clean
