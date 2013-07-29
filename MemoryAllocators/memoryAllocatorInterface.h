
#ifndef _MEMORY_ALLOCATOR_INTERFACE_H_
#define _MEMORY_ALLOCATOR_INTERFACE_H_

// required header files
// memoryAllocatorApi.h	: error constant

struct MemoryAllocator {
	unsigned char name[30];
	
	MA_ERROR (*InitializeMemoryPool)(void **context,
			void *memoryPool, unsigned long memoryPoolSize,
			unsigned long minimumAllocationSize);
	MA_ERROR (*DeinitializeMemoryPool)(void *context);
	MA_ERROR (*AllocateMemory)(void *context, unsigned long size, void **memory);
	MA_ERROR (*FreeMemory)(void *context, void *memory);
	MA_ERROR (*GetFreeMemorySize)(void *context, unsigned long *memorySize);
	MA_ERROR (*GetAllocatedMemorySize)(void *context, unsigned long *memorySize);
	MA_ERROR (*GetTotalRequestedMemorySize)(void *context, unsigned long *memorySize);
	MA_ERROR (*GetMaximumAvailableMemorySize)(void *context, unsigned long *memorySize);

	MA_ERROR (*Ioctl)(unsigned long command, void *context,
			void *param1, void *param2, void *param3, void *param4);
};

#endif	/* _MEMORY_ALLOCATOR_INTERFACE_H_ */

