# ---- Paths ----
INCDIR   := inc
SRCDIR   := src
LIBDIR   := lib
BINDIR   := bin
DATADIR  := data
BUILDDIR := build

# ---- Output ----
TARGET := $(BINDIR)/sim

# ---- Toolchain (override on the command line if needed) ----
CC      := gcc
CFLAGS  ?= -O2 -Wall -Wextra -std=c11 -I$(INCDIR) -I$(LIBDIR) -MMD -MP -DFIXED_POINT=16 -DFRACBITS=8
LDFLAGS ?=
LDLIBS  ?=

# ---- Source/Object discovery ----
SRCS := $(wildcard $(SRCDIR)/*.c) $(LIBDIR)/kiss_fft.c
OBJS := $(patsubst $(SRCDIR)/%.c,$(BUILDDIR)/%.o,$(filter $(SRCDIR)/%.c,$(SRCS))) \
        $(patsubst $(LIBDIR)/%.c,$(BUILDDIR)/%.o,$(filter $(LIBDIR)/%.c,$(SRCS)))
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

$(BUILDDIR)/%.o: $(LIBDIR)/%.c | $(BUILDDIR)
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
