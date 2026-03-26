# Compiler and flags
CC = cc
CFLAGS = -g
LDFLAGS = -lglfw -lm

# Targets
TARGET = build/spacer3000
CBMCHARGEN = build/cbmchargen

# Source files for each target
SPACER_SOURCES = glad/glad.c cbmchargen/cbmchargen.c main.c 
CBMCHARGEN_SOURCES = cbmchargen/cbmchargen.c cbmchargen/main.c

# Object files for each target
SPACER_OBJS = $(SPACER_SOURCES:.c=.o)
CBMCHARGEN_OBJS = $(CBMCHARGEN_SOURCES:.c=.o)

# Default target
all: $(TARGET)

# Link the spacer3000 executable
$(TARGET): $(SPACER_OBJS)
	@mkdir -p build
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Link the cbmchargen utility (if it's a separate program)
$(CBMCHARGEN): $(CBMCHARGEN_OBJS)
	@mkdir -p build
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Compile C source files to object files
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

# Clean build artifacts
clean:
	rm -f $(SPACER_OBJS) $(CBMCHARGEN_OBJS) $(TARGET) $(CBMCHARGEN)
	rm -rf build

# Test target
test: $(CBMCHARGEN)

# Phony targets
.PHONY: all clean test