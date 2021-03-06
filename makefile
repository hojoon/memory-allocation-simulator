# makefile

# to debug makefile, remark below line or use "make Q="
Q=@

ifeq ($PROJ_ROOT,)
#error PROJ_ROOT is not set. Please set PROJ_ROOT to build this project and libraries before building.
endif

include $(PROJ_ROOT)/includePre.mk


# library name. library will be built with the name, lib$(LIB_NAME).a
LIB_NAME = 

# list of library source files
LIB_SRCS = 

# source files for application
TARGET_SRCS = main.c

# library files with paths to be added to dependency rule
DEPEND_LIBS = MemoryAllocators/libMemoryAllocators.a MemoryAllocators/Simple/libSimpleMemoryAllocator.a TestSequences/libTestSequences.a

# library files
# LIBS += -lSystemLibrary -lMyLibrary
LIBS += -lMemoryAllocators -lSystemDefaultMemoryAllocator -lSimpleMemoryAllocator -lTestSequences

# relative paths of library directories if library files are not in current directory
# LIB_DIRS += -LLibDirectory1 -LLibDirectory2 -LSubDirectory1
LIB_DIRS += -LMemoryAllocators -LMemoryAllocators/SystemDefault -LMemoryAllocators/Simple -LTestSequences

# sub directories to be built before
# SUB_DIRS = SubDirectory1 SubDirectory2
SUB_DIRS = MemoryAllocators TestSequences


include $(PROJ_ROOT)/includePost.mk
