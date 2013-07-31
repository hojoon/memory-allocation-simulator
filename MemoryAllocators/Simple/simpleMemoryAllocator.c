
#include "../memoryAllocatorApi.h"
#include "../memoryAllocatorInterface.h"

struct SimpleMemoryAllocatorContext {
	unsigned long count;
	void *memoryPool;
	unsigned long memoryPoolSize;
	unsigned long minimumAllocationSize;
	unsigned long requestedSize;
	unsigned long allocatedSize;
	unsigned long lossByAllocator;
	unsigned long blockNumber;		// number of segmented memory blocks
};

static void *SimpleMalloc(struct SimpleMemoryAllocatorContext *context,
		unsigned long size) {
	void *retval=0;
	
	if (context && size) {
		unsigned long *blockTable=context->memoryPool;
		int i;
		int j;
		unsigned long neededBlocks=(size+context->minimumAllocationSize-1)
				/context->minimumAllocationSize;
		for (i=0; i<context->blockNumber; i++) {
			if (blockTable[i*2]==0) {	// not allocated
				for (j=0; j<neededBlocks; j++) {
					if (blockTable[(i+j)*2]) {	// allocated
						break;
					}
				}
				if (j==neededBlocks) {
					// allocate
					for (j=0; j<neededBlocks; j++) {
						blockTable[(i+j)*2]=i;
						blockTable[(i+j)*2+1]=0;
					}
					blockTable[i*2+1]=size;
					retval=(unsigned char *)(blockTable+(context->blockNumber*2))+
							i*context->minimumAllocationSize;
					context->requestedSize+=size;
					context->allocatedSize+=
							neededBlocks*context->minimumAllocationSize;
				} else {
					i=j;
				}
			}
		}
	}
	return retval;
}

static MA_ERROR SimpleFree(struct SimpleMemoryAllocatorContext *context,
		void *memory) {
	MA_ERROR retval=MA_INVALID_PARAM;

	if (context && memory) {
		unsigned long *blockTable=context->memoryPool;
		int i;
		int blockIndex;
		int count;
		
		if (((unsigned long)memory)%4==0) {
			blockIndex=((unsigned long)memory-
					(unsigned long)(blockTable+context->blockNumber))/4;
			if (blockTable[blockIndex]==blockIndex) {	// start of blocks
				context->requestedSize-=blockTable[blockIndex*2+1];
				count=0;
				for (i=blockIndex; i<context->blockNumber; i++) {
					if (blockTable[i*2]==blockIndex) {
						count++;
						blockTable[i*2]=0;
						blockTable[i*2+1]=0;
					} else {
						break;
					}
				}
				context->allocatedSize-=count*context->minimumAllocationSize;
			}
		}
	}
	return retval;
}

static unsigned long SimpleGetFreeMemorySize(
		struct SimpleMemoryAllocatorContext *context) {
	unsigned long retval=0;
	if (context) {
		retval=context->memoryPoolSize-context->lossByAllocator-context->allocatedSize;
	}
	return retval;
}

static unsigned long SimpleGetAllocatedMemorySize(
		struct SimpleMemoryAllocatorContext *context) {
	unsigned long retval=0;
	if (context) {
		retval=context->allocatedSize;
	}
	return retval;
}

static unsigned long SimpleGetTotalRequestedMemorySize(
		struct SimpleMemoryAllocatorContext *context) {
	unsigned long retval=0;
	if (context) {
		retval=context->requestedSize;
	}
	return retval;
}

static unsigned long SimpleGetMaximumAvailableMemorySize(
		struct SimpleMemoryAllocatorContext *context) {
	unsigned long retval=0;
	if (context) {
		retval=0;
	}
	return retval;
}

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
			int remainder;
			memset(temp,0,sizeof(struct SimpleMemoryAllocatorContext));
			memset(memoryPool,0,memoryPoolSize);
			temp->count=++contextCount;
			remainder=(unsigned long)memoryPool%4;
			temp->memoryPool=((unsigned char *)memoryPool)+remainder;
			temp->lossByAllocator=remainder;
			temp->memoryPoolSize=memoryPoolSize;
			if (minimumAllocationSize<128) {
				minimumAllocationSize=128;
			}
			temp->minimumAllocationSize=minimumAllocationSize;
			// +4 : requested size cannot be maintained
			// +8 : requested size can be calculated
			temp->blockNumber=
					(memoryPoolSize-remainder)/(minimumAllocationSize+8);
			temp->lossByAllocator+=(
					(memoryPoolSize-remainder)-
					temp->blockNumber*(minimumAllocationSize+8));
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
		
		temp=SimpleMalloc(context,size);
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
		SimpleFree(context,memory);
		retval=MA_NO_ERROR;
	}
	return retval;
}

static MA_ERROR Simple_GetFreeMemorySize(void *context, unsigned long *memorySize) {
	MA_ERROR retval=MA_INVALID_PARAM;
	
	*memorySize=SimpleGetFreeMemorySize(context);
	
	return retval;
}

static MA_ERROR Simple_GetAllocatedMemorySize(void *context, unsigned long *memorySize) {
	MA_ERROR retval=MA_INVALID_PARAM;
	
	*memorySize=SimpleGetAllocatedMemorySize(context);
	
	return retval;
}

static MA_ERROR Simple_GetTotalRequestedMemorySize(void *context, unsigned long *memorySize) {
	MA_ERROR retval=MA_INVALID_PARAM;
	
	*memorySize=SimpleGetRequestedMemorySize(context);
	
	return retval;
}

static MA_ERROR Simple_GetMaximumAvailableMemorySize(void *context, unsigned long *memorySize) {
	MA_ERROR retval=MA_INVALID_PARAM;
	
	*memorySize=SimpleGetMaximumAvailableMemorySize(context);
	
	return retval;
}


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
