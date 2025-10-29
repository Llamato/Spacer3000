# Compiler and flags
CC = cc
CFLAGS = -g
LDFLAGS = -lglfw -lm

# Targets
TARGET = build/spacer3000
SOURCES = glad/glad.c main.c
OBJS = $(SOURCES:.c=.o)

# Default target
all: $(TARGET)

# Link the final executable
$(TARGET): $(OBJS)
	@mkdir -p build
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Compile C source files to object files
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

# Clean build artifacts
clean:
	rm -f $(OBJS) $(TARGET)
	rm -rf build

# Phony targets
.PHONY: all clean