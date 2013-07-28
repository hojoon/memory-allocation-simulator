# includePre.mk

CC = gcc
AR = ar
RANLIB = ranlib
RM = rm
MKDIR = mkdir
MAKE = make

ifeq ($(RELEASE), 1)
OBJS_DIR = Release
DBG_FLAG = -O2 -DNDEBUG
else
OBJS_DIR = Debug
DBG_FLAG = -g -O0 -DDEBUG
endif

DEPEND_FILE = $(OBJS_DIR)/depend_file

LIB_DIRS = -L.
