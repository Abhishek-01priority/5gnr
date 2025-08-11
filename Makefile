# ---- Paths ----
INCDIR   := inc
SRCDIR   := src
BINDIR   := bin
DATADIR  := data
BUILDDIR := build

# ---- Output ----
TARGET := $(BINDIR)\sim.exe

# ---- Toolchain (override on the command line if needed) ----
# e.g., make CC=clang
CC      := gcc
CFLAGS  ?= -O2 -Wall -Wextra -std=c11 -I$(INCDIR) -MMD -MP
LDFLAGS ?=
LDLIBS  ?=

# ---- Source/Object discovery ----
SRCS := $(wildcard $(SRCDIR)/*.c)
OBJS := $(patsubst $(SRCDIR)/%.c,$(BUILDDIR)/%.o,$(SRCS))
DEPS := $(OBJS:.o=.d)

# ---- OS-specific shell helpers ----
ifeq ($(OS),Windows_NT)
  SHELL := cmd
  .SHELLFLAGS := /C
  MKDIR    = mkdir
  RMDIR    = rmdir /S /Q
  RM       = del /Q
  COPYBINS = @if exist "$(DATADIR)\*.bin" copy /Y "$(DATADIR)\*.bin" "$(BINDIR)" >nul
else
  MKDIR    = mkdir -p
  RMDIR    = rm -rf
  RM       = rm -f
  COPYBINS = @cp -f $(DATADIR)/*.bin "$(BINDIR)" 2>/dev/null || true
endif

# ---- Default target ----
all: $(TARGET) copy-data

# ---- Link ----
$(TARGET): $(OBJS) | $(BINDIR)
	$(CC) $(LDFLAGS) $(OBJS) -o $@ $(LDLIBS)

# ---- Compile ----
$(BUILDDIR)/%.o: $(SRCDIR)/%.c | $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# ---- Ensure output dirs exist ----
$(BUILDDIR):
	$(MKDIR) "$(BUILDDIR)"

$(BINDIR):
	$(MKDIR) "$(BINDIR)"

# ---- Copy .bin test data next to the EXE (if any) ----
# copy-data: | $(BINDIR)
# 	$(COPYBINS)

# ---- Utilities ----
run: all
	$(TARGET)

clean:
	-$(RMDIR) "$(BINDIR)"
	-$(RMDIR) "$(BUILDDIR)"

-include $(DEPS)

.PHONY: all clean run copy-data
