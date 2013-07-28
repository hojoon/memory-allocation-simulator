# includePost.mk

ifneq ($(LIB_NAME),)
LIB_FULL_NAME = lib$(LIB_NAME).a
ALL_LIBS = -l$(LIB_NAME)
endif
ALL_LIBS += $(LIBS)
LIB_OBJS = $(LIB_SRCS:%.c=$(OBJS_DIR)/%.o)

TARGET_OBJS = $(TARGET_SRCS:%.c=$(OBJS_DIR)/%.o)
TARGET_NAMES = $(TARGET_SRCS:%.c=$(OBJS_DIR)/%)

ifneq ($(LIB_DIRS),)
INC_DIRS += $(LIB_DIRS:-L%=-I%)
endif

.SUFFIXES : .c .o

all : lib subdirs targets

subdirs :
	$(Q)for dir in $(SUB_DIRS); do \
		$(MAKE) -C $$dir all; \
		if [ $$? != 0 ]; then exit 1; fi; \
	done

lib : $(LIB_FULL_NAME)

liball : $(LIB_FULL_NAME)
	$(Q)for dir in $(SUB_DIRS); do \
		$(MAKE) -C $$dir liball; \
		if [ $$? != 0 ]; then exit 1; fi; \
	done

targets : $(TARGET_NAMES)

targetsall : targets
	$(Q)for dir in $(SUB_DIRS); do \
		$(MAKE) -C $$dir targetsall; \
		if [ $$? != 0 ]; then exit 1; fi; \
	done

$(LIB_FULL_NAME) : $(LIB_OBJS)
ifneq ($(LIB_NAME),)
	$(Q)`[ -d $(OBJS_DIR) ] || $(MKDIR) -p $(OBJS_DIR)`
	$(AR) rcv $@ $(LIB_OBJS)
	$(RANLIB) $@
endif

$(OBJS_DIR)/%.o: %.c
	$(Q)`[ -d $(OBJS_DIR) ] || $(MKDIR) -p $(OBJS_DIR)`
	$(CC) $(CFLAGS) $(DBG_FLAGS) $(INC_DIRS) -c $< -o $@

.SECONDEXPANSION:
$(TARGET_NAMES): $$@.o
#ifeq ($(LIBS_CYCLING_DEPEND),1)
#	$(CC) -o $@ $< $(LIB_DIRS) -Wl,-\( $(ALL_LIBS) -Wl,-\)
#else
	$(CC) -o $@ $< $(LIB_DIRS) $(ALL_LIBS)
#endif

clean :
	$(Q)$(RM) -fr $(OBJS_DIR) $(LIB_FULL_NAME)

cleanall : clean
	$(Q)for dir in $(SUB_DIRS); do \
		$(MAKE) -C $$dir cleanall; \
		if [ $$? != 0 ]; then exit 1; fi; \
	done

dep:
	$(Q)`[ -d $(OBJS_DIR) ] || $(MKDIR) -p $(OBJS_DIR)`
	$(Q)$(RM) -f $(DEPEND_FILE)
	$(Q)for FILE in $(LIB_SRCS:%.c=%) $(TARGET_SRCS:%.c=%); do \
		$(CC) -MM -MT $(OBJS_DIR)/$$FILE.o $$FILE.c $(CFLAGS) $(DBGFLAGS) $(INC_DIRS) >> $(DEPEND_FILE); \
	done

depall : dep
	$(Q)for dir in $(SUB_DIRS); do \
		$(MAKE) -C $$dir depall; \
		if [ $$? != 0 ]; then exit 1; fi; \
	done

$(TARGET_NAMES) : $(LIB_FULL_NAME) $(DEPEND_LIBS)


ifneq ($(MAKECMDGOALS), clean)
ifneq ($(MAKECMDGOALS), dep)
ifneq ($(MAKECMDGOALS), clealall)
ifneq ($(MAKECMDGOALS), depall)
ifneq ($(strip $(LIB_SRCS) $(TARGET_SRCS)),)
-include $(DEPEND_FILE)
endif
endif
endif
endif
endif
