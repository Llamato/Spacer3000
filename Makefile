# Compiler and flags
CC = cc
CFLAGS = -g
CPPFLAGS = -I. -Iglibs
LDFLAGS = -lglfw -lm

# Targets
TARGET = build/spacer3000
CBMCHARGEN = build/cbmchargen

# Source files for each target
COMMON_GLIB_SOURCES = glibs/common.c
CBMCHARGEN_SOURCES = glibs/cbmchargen/cbmchargen.c
GVMATH_SOURCES = glibs/gvmath/gvmath.c
SPACER_SOURCES = glad/glad.c $(COMMON_GLIB_SOURCES) $(CBMCHARGEN_SOURCES) $(GVMATH_SOURCES) main.c 

# Object files for each target
SPACER_OBJS = $(SPACER_SOURCES:.c=.o)
COMMON_GLIB_OBJS = $(COMMON_GLIB_SOURCES:.c=.o)
CBMCHARGEN_OBJS = $(CBMCHARGEN_SOURCES:.c=.o)
GVMATH_OBJS = $(GVMATH_SOURCES:.c=.o)


# Default target
all: $(TARGET)

# Link the spacer3000 executable
$(TARGET): $(SPACER_OBJS)
	@mkdir -p build
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Compile C source files to object files
%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

# Clean build artifacts
clean:
	rm -f $(SPACER_OBJS) $(CBMCHARGEN_OBJS) $(TARGET) $(CBMCHARGEN)
	rm -rf build

# Test target
test: $(CBMCHARGEN)

# Phony targets
.PHONY: all clean test