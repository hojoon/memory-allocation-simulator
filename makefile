
CC = gcc
AR = ar
RANLIB = ranlib
RM = rm
MKDIR = mkdir

ifeq ($(RELEASE), 1)
OBJS_DIR = Release
DBG_FLAG = -O2 -DNDEBUG
else
OBJS_DIR = Debug
DBG_FLAG = -g -O0 -DDEBUG
endif

DEPEND_FILE = $(OBJS_DIR)/depend_file

TARGET_SRCS = main.c
TARGET_OBJS = $(TARGET_SRCS:%.c=$(OBJS_DIR)/%.o)
TARGET_NAMES = $(TARGET_SRCS:%.c=$(OBJS_DIR)/%)

LIBS += 
LIB_DIRS = -L.

TARGET = memtest

.SUFFIXES : .c .o

all: $(LIB_FULL_NAME) $(TARGET_NAMES)

$(LIB_FULL_NAME): $(LIB_OBJS)
	$(AR) rcv $@ $(LIB_OBJS)
	$(RANLIB) $@

$(OBJS_DIR)/%.o: %.c
	@`[ -d $(OBJS_DIR) ] || $(MKDIR) $(OBJS_DIR)`
	$(CC) $(CFLAGS) $(DBG_FLAGS) -c $< -o $@

.SECONDEXPANSION:
$(TARGET_NAMES): $$@.o
	$(CC) -o $@ $< $(LIB_DIRS) $(LIBS)

clean:
	$(RM) -fr $(OBJS_DIR)

dep:
	@`[ -d $(OBJS_DIR) ] || $(MKDIR) $(OBJS_DIR)`
	@$(RM) -f $(DEPEND_FILE)
	@for FILE in $(LIB_SRCS:%.c=%) $(TARGET_SRCS:%.c=%); do \
		$(CC) -MM -MT $(OBJS_DIR)/$$FILE.o $$FILE.c >> $(DEPEND_FILE); \
	done

ifneq ($(MAKECMDGOALS), clean)
ifneq ($(MAKECMDGOALS), depend)
ifneq ($(strip $(LIB_SRCS) $(TARGET_SRCS)),)
-include $(DEPEND_FILE)
endif
endif
endif

