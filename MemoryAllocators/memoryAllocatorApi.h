
#ifndef _MEMORY_ALLOCATOR_API_H_
#define _MEMORY_ALLOCATOR_API_H_

typedef enum {
	MA_NO_ERROR,
	MA_INVALID_PARAM,
	MA_NOT_INITIALIZED,
	MA_INVALID_MEMORY_ALLOCATOR,
	MA_SYSTEM_MEMORY_FULL,
	MA_MEMORY_FULL,
	MA_NOT_SUPPORTED,
	
	MA_UNKNOWN_ERROR,
	MA_ERROR_MAX
} MA_ERROR;


extern MA_ERROR MA_InitializeMemoryAllocation(void);

extern MA_ERROR MA_InitializeMemoryPool(void **context,
		void *memoryPool, unsigned long memoryPoolSize,
		unsigned long minimumAllocationSize);
extern MA_ERROR MA_DeinitializeMemoryPool(void *context);
extern MA_ERROR MA_AllocateMemory(void *context, unsigned long size, void **memory);
extern MA_ERROR MA_FreeMemory(void *context, void *memory);
extern MA_ERROR MA_GetFreeMemorySize(void *context, unsigned long *memorySize);
extern MA_ERROR MA_GetAllocatedMemorySize(void *context, unsigned long *memorySize);
extern MA_ERROR MA_GetTotalRequestedMemorySize(void *context, unsigned long *memorySize);
extern MA_ERROR MA_GetMaximumAvailableMemorySize(void *context, unsigned long *memorySize);
extern MA_ERROR MA_Ioctl(unsigned long command, void *context,
		void *param1, void *param2, void *param3, void *param4);

extern void *MA_GeneralMalloc(unsigned long size);
extern void MA_GeneralFree(void *memory);

#endif	/* _MEMORY_ALLOCATOR_API_H_ */

