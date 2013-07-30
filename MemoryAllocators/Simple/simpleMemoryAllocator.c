
#include "../memoryAllocatorApi.h"
#include "../memoryAllocatorInterface.h"

struct SimpleMemoryAllocatorContext {
	unsigned long count;
};

static unsigned long contextCount=0;

static MA_ERROR Simple_InitializeMemoryPool(void **context,
		void *memoryPool, unsigned long memoryPoolSize,
		unsigned long minimumAllocationSize) {
	MA_ERROR retval=MA_INVALID_PARAM;
	
	if (context) {
		struct SimpleMemoryAllocatorContext *temp;
		
		temp=(struct SimpleMemoryAllocatorContext *)MA_GeneralMalloc(
				sizeof(struct SimpleMemoryAllocatorContext));
		if (temp) {
			temp->count=++contextCount;
			*context=temp;
			retval=MA_NO_ERROR;
		} else {
			retval=MA_SYSTEM_MEMORY_FULL;
		}
	}
	return retval;
}

static MA_ERROR Simple_DeinitializeMemoryPool(void *context) {
	MA_ERROR retval=MA_INVALID_PARAM;
	
	if (context) {
		MA_GeneralFree(context);
		retval=MA_NO_ERROR;
	}
	return retval;
}

static MA_ERROR Simple_AllocateMemory(void *context, unsigned long size, void **memory) {
	MA_ERROR retval=MA_INVALID_PARAM;
	
	if (context && size && memory) {
		void *temp;
		
		temp=malloc(size);
		if (temp) {
			*memory=temp;
			retval=MA_NO_ERROR;
		} else {
			retval=MA_MEMORY_FULL;
		}
	}
	return retval;
}

static MA_ERROR Simple_FreeMemory(void *context, void *memory) {
	MA_ERROR retval=MA_INVALID_PARAM;
	
	if (context && memory) {
		free(memory);
		retval=MA_NO_ERROR;
	}
	return retval;
}

// static MA_ERROR Simple_GetFreeMemorySize(void *context, unsigned long *memorySize) {
// 	MA_ERROR retval=MA_INVALID_PARAM;
// 	
// 	
// 	return retval;
// }
// 
// static MA_ERROR Simple_GetAllocatedMemorySize(void *context, unsigned long *memorySize) {
// 
// }
// 
// static MA_ERROR Simple_GetTotalRequestedMemorySize(void *context, unsigned long *memorySize) {
// 
// 
// }
// 
// static MA_ERROR Simple_GetMaximumAvailableMemorySize(void *context, unsigned long *memorySize) {
// 
// 
// }


struct MemoryAllocator simpleMemoryAllocator={
	"Simple",
	Simple_InitializeMemoryPool,
	Simple_DeinitializeMemoryPool,
	Simple_AllocateMemory,
	Simple_FreeMemory,
	0,	// GetFreeMemorySize,
	0,	// GetAllocatedMemorySize
	0,	// GetAllocatedMemorySize
	0,	// GetTotalRequestedMemorySize
	0	// Ioctl
};
