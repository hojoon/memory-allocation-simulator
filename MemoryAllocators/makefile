# makefile

# to debug makefile, remark below line or use "make Q="
Q=@

ifeq ($PROJ_ROOT,)
#error PROJ_ROOT is not set. Please set PROJ_ROOT to build this project and libraries before building.
endif

include $(PROJ_ROOT)/includePre.mk


# library name. library will be built with the name, lib$(LIB_NAME).a
LIB_NAME = MemoryAllocators

# list of library source files
LIB_SRCS = memoryAllocatorApi.c

# source files for application
TARGET_SRCS = 

# library files with paths to be added to dependency rule
DEPEND_LIBS = SystemDefault/libSystemDefaultMemoryAllocator.a Simple/libSympleMemoryAllocator.a

# library files
# LIBS += -lSystemLibrary -lMyLibrary
LIBS += -lSystemDefaultMemoryAllocator -lSimpleMemoryAllocator

# relative paths of library directories if library files are not in current directory
# LIB_DIRS += -LLibDirectory1 -LLibDirectory2 -LSubDirectory1
LIB_DIRS += -LSystemDefaultMemoryAllocator -LSimpleMemoryAllocator

# sub directories to be built before
# SUB_DIRS = SubDirectory1 SubDirectory2
SUB_DIRS = SystemDefault Simple


include $(PROJ_ROOT)/includePost.mk
