# =====================================================
# librobotcontrol - static build version
# =====================================================

# directories
SRCDIR      := src
LIBDIR      := lib
BUILDDIR    := build
INCLUDEDIR  := include
TARGET      := $(LIBDIR)/librobotcontrol.a
RC_VAR_DIR  := var/lib/robotcontrol

# file definitions for rules
SOURCES     := $(shell find $(SRCDIR) -type f -name *.c)
OBJECTS     := $(SOURCES:$(SRCDIR)/%.c=$(BUILDDIR)/%.o)
INCLUDES    := $(shell find $(INCLUDEDIR) -name '*.h')

# compiler and linker binaries
CC      ?= gcc
AR      ?= ar
LD      ?= ld

# general compiler flags
WFLAGS      := -Wall -Wextra -Werror=float-equal -Wuninitialized \
    -Wunused-variable -Wdouble-promotion -pedantic -Wmissing-prototypes \
    -Wmissing-declarations -Werror=undef
CFLAGS      := $(EXT_CFLAGS) -g -fPIC -I $(INCLUDEDIR)
OPT_FLAGS   := -O2
LDFLAGS     := -static -lm -lrt -pthread

# different compile flags for math libs
MATH_OPT_FLAGS  := -O3 -ffast-math -ftree-vectorize

# commands
RM          := rm -rf
INSTALL     := install -m 755
INSTALLDIR  := install -d -m 755
INSTALLDIRWRITE := install -d -m 777

# prefix variable for making debian package
prefix      ?= /usr

# =====================================================
# Build rules
# =====================================================

# create static library archive
$(TARGET): $(OBJECTS)
	@mkdir -p $(LIBDIR)
	@$(AR) rcs $(TARGET) $(OBJECTS)
	@echo "Done making static $(TARGET)"

# math objects
$(BUILDDIR)/math/%.o : $(SRCDIR)/math/%.c $(INCLUDES)
	@mkdir -p $(dir $(@))
	@$(CC) -c $(CFLAGS) $(WFLAGS) $(MATH_OPT_FLAGS) $(DEBUGFLAG) $< -o $(@)
	@echo "made: $(@)"

# all other objects
$(BUILDDIR)/%.o : $(SRCDIR)/%.c $(INCLUDES)
	@mkdir -p $(dir $(@))
	@$(CC) -c $(CFLAGS) $(WFLAGS) $(OPT_FLAGS) $(DEBUGFLAG) $< -o $(@)
	@echo "made: $(@)"

# main target
all: $(TARGET)

# debug build
debug:
	$(MAKE) $(MAKEFILE) DEBUGFLAG="-g -D DEBUG"
	@echo "$(TARGET) Make Debug Complete"

clean:
	@echo "Cleaning library..."
	@if [ -d "$(LIBDIR)" ]; then rm -rf "$(LIBDIR)"; fi
	@if [ -d "$(BUILDDIR)" ]; then rm -rf "$(BUILDDIR)"; fi
	@echo "Library Clean Complete"

# install/uninstall left unchanged (optional)
install uninstall:
	@echo "Static build - no install step"