# Executable name
TARGET = main

# Directories
SRCDIR = src
INCDIR = include
OBJDIR = obj
BINDIR = bin

# Files to use for build process
SRCS = $(wildcard $(SRCDIR)/*.c)
INCS = $(wildcard $(INCDIR)/*.h)
OBJS = $(SRCS:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

# C compiler to use
CC = gcc

# Compiler flags
CFLAGS = -Wall -I$(INCDIR)

# Linker to use
LINKER = gcc

# Linker flags
LFLAGS = -Wall

# Libraries to link into executable
LIBS = -lcurl -lpthread

# Generic Makefile part, can be used to build any
# executable just by changing definitions above

$(BINDIR)/$(TARGET): $(OBJS)
	@mkdir -p $(BINDIR)
	@$(LINKER) $(OBJS) $(LFLAGS) -o $@ $(LIBS)
	@echo "Linking done!"

$(OBJS): $(OBJDIR)/%.o : $(SRCDIR)/%.c
	@mkdir -p $(OBJDIR)
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo "Compiled "$<" successfully!"

.PHONY: clean
clean:
	rm -rf $(OBJS) $(BINDIR)/$(TARGET)
	@echo "Cleanup complete!"
