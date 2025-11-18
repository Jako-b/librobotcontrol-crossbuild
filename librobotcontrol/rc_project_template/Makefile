# This is a general use makefile for librobotcontrol projects written in C.
# Just change the target name to match your main source code filename.
TARGET = rc_project_template

# compiler and linker binaries (portabel)
CC          ?= gcc
LINKER      ?= $(CC)
AR          ?= ar
LD          ?= ld

# compiler and linker flags
WFLAGS      := -Wall -Wextra -Werror=float-equal -Wuninitialized -Wunused-variable -Wdouble-promotion
CFLAGS      := -g -c -Wall
LDFLAGS    += -lm -lrt -pthread $(LIBROBOTCONTROL_STATIC)

SOURCES     := $(wildcard *.c)
INCLUDES    := $(wildcard *.h)
OBJECTS     := $(SOURCES:$%.c=$%.o)

prefix      := /usr/local
RM          := rm -f
INSTALL     := install -m 4755
INSTALLDIR  := install -d -m 755

SYMLINK     := ln -s -f
SYMLINKDIR  := /etc/robotcontrol
SYMLINKNAME := link_to_startup_program

# linking Objects
$(TARGET): $(OBJECTS)
	@$(LINKER) -o $@ $(OBJECTS) $(LDFLAGS)
	@echo "Made: $@"

# compiling command
$(OBJECTS): %.o : %.c $(INCLUDES)
	@$(CC) $(CFLAGS) $(WFLAGS) $(DEBUGFLAG) $< -o $@
	@echo "Compiled: $@"

all:    $(TARGET)

debug:
	$(MAKE) $(MAKEFILE) DEBUGFLAG="-g -D DEBUG"
	@echo " "
	@echo "$(TARGET) Make Debug Complete"
	@echo " "

install:
	@$(MAKE) --no-print-directory
	@$(INSTALLDIR) $(DESTDIR)$(prefix)/bin
	@$(INSTALL) $(TARGET) $(DESTDIR)$(prefix)/bin
	@echo "$(TARGET) Install Complete"

clean:
	@echo "Cleaning rc_project_template..."
	@if [ -f "$(TARGET)" ]; then rm -f "$(TARGET)"; fi
	@if [ -n "$(OBJECTS)" ]; then rm -f $(OBJECTS); fi
	@echo "$(TARGET) Clean Complete"

uninstall:
	@$(RM) $(DESTDIR)$(prefix)/bin/$(TARGET)
	@echo "$(TARGET) Uninstall Complete"

runonboot:
	@$(MAKE) install --no-print-directory
	@$(SYMLINK) $(DESTDIR)$(prefix)/bin/$(TARGET) $(SYMLINKDIR)/$(SYMLINKNAME)
	@echo "$(TARGET) Set to Run on Boot"