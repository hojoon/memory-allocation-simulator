
#include "../memoryAllocatorApi.h"
#include "../memoryAllocatorInterface.h"

struct SimpleMemoryAllocatorContext {
	unsigned long count;	// context creation count
	void *memoryPool;
	unsigned long memoryPoolSize;
	unsigned long *blockTable;
	unsigned long blockNumber;		// number of segmented memory blocks
	unsigned char *memoryBase;
	unsigned long minimumAllocationSize;
	unsigned long requestedSize;
	unsigned long allocatedSize;
	unsigned long lossByAllocator;
};

static void *SimpleMalloc(struct SimpleMemoryAllocatorContext *context,
		unsigned long size) {
	void *retval=0;
	
	if (context && size) {
		unsigned long i;
		unsigned long j;
		unsigned long neededBlocks=(size+context->minimumAllocationSize-1)
				/context->minimumAllocationSize;

		for (i=0; i<context->blockNumber; i++) {
			if (context->blockTable[i*2]==0) {	// not allocated
				for (j=0; j<neededBlocks; j++) {
					if (context->blockTable[(i+j)*2]) {	// allocated
						break;
					}
				}
				if ((j==neededBlocks) && ((i+j)<context->blockNumber)) {
					// allocate
					for (j=0; j<neededBlocks; j++) {
						context->blockTable[(i+j)*2]=i | 0x80000000;
						context->blockTable[(i+j)*2+1]=0;
					}
					context->blockTable[i*2+1]=size;
					retval=(unsigned char *)(context->memoryBase)+
							i*context->minimumAllocationSize;
					context->requestedSize+=size;
					context->allocatedSize+=
							neededBlocks*context->minimumAllocationSize;
					break;
				} else {
					i+=j;
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
		int i;
		int blockIndex;
		int count;
		
		if (((unsigned long)memory)%4==0) {
			blockIndex=((unsigned long)memory-
					(unsigned long)(context->blockTable+context->blockNumber*2))/
					context->minimumAllocationSize;
			if ((context->blockTable[blockIndex*2] & 0x7FFFFFFF)==blockIndex) {	// start of blocks
				context->requestedSize-=context->blockTable[blockIndex*2+1];
				count=0;
				for (i=blockIndex; i<context->blockNumber; i++) {
					if ((context->blockTable[i*2] & 0x7FFFFFFF)==blockIndex) {
						count++;
						context->blockTable[i*2]=0;
						context->blockTable[i*2+1]=0;
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
		int i;
		int j;
		int count;
		int maxCount;
		
		maxCount=0;
		for (i=0; i<context->blockNumber; i++) {
			if (context->blockTable[i*2]==0) {	// not allocated
				count=1;
				for (j=i+1; j<context->blockNumber; j++) {
					if (context->blockTable[j*2]) {	// allocated
						break;
					}
					count++;
					if (count>maxCount) {
						maxCount=count;
					}
				}
				i=j;
			}
		}
		retval=maxCount*context->minimumAllocationSize;
	}
	return retval;
}

static unsigned long contextCount=0;
#define SIMPLE_MALLOCATOR_MINIMUM_ALLOCATION_SIZE   16
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
            unsigned long minAllocSize;
            unsigned long maxBlockNum=memoryPoolSize/2/8;
			memset(temp,0,sizeof(struct SimpleMemoryAllocatorContext));
			memset(memoryPool,0,memoryPoolSize);
			temp->count=++contextCount;
			temp->memoryPool=memoryPool;
			temp->memoryPoolSize=memoryPoolSize;
			remainder=((unsigned long)memoryPool)%4;
			temp->blockTable=(unsigned long *)(((unsigned char *)memoryPool)+remainder);
			temp->lossByAllocator=remainder;
            minAllocSize=memoryPoolSize/2/maxBlockNum;
            if (minimumAllocationSize<minAllocSize) {
                minimumAllocationSize=minAllocSize;
            }
			if (minimumAllocationSize<SIMPLE_MALLOCATOR_MINIMUM_ALLOCATION_SIZE) {
				minimumAllocationSize=SIMPLE_MALLOCATOR_MINIMUM_ALLOCATION_SIZE;
			}
			temp->minimumAllocationSize=minimumAllocationSize;
			// +4 : requested size cannot be maintained
			// +8 : requested size can be calculated
			temp->blockNumber=
					(memoryPoolSize-remainder)/(minimumAllocationSize+8);
			temp->memoryBase=(unsigned char *)(temp->blockTable+temp->blockNumber*2);
			temp->lossByAllocator+=(
					memoryPoolSize-remainder-
					temp->blockNumber*minimumAllocationSize);
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
	retval=MA_NO_ERROR;
	
	return retval;
}

static MA_ERROR Simple_GetAllocatedMemorySize(void *context, unsigned long *memorySize) {
	MA_ERROR retval=MA_INVALID_PARAM;
	
	*memorySize=SimpleGetAllocatedMemorySize(context);
	retval=MA_NO_ERROR;
	
	return retval;
}

static MA_ERROR Simple_GetTotalRequestedMemorySize(void *context, unsigned long *memorySize) {
	MA_ERROR retval=MA_INVALID_PARAM;
	
	*memorySize=SimpleGetTotalRequestedMemorySize(context);
	retval=MA_NO_ERROR;
	
	return retval;
}

static MA_ERROR Simple_GetMaximumAvailableMemorySize(void *context, unsigned long *memorySize) {
	MA_ERROR retval=MA_INVALID_PARAM;
	
    if (memorySize) {
        *memorySize=SimpleGetMaximumAvailableMemorySize(context);
        retval=MA_NO_ERROR;
    }
	return retval;
}


struct MemoryAllocator simpleMemoryAllocator={
	"Simple",
	Simple_InitializeMemoryPool,
	Simple_DeinitializeMemoryPool,
	Simple_AllocateMemory,
	Simple_FreeMemory,
	Simple_GetFreeMemorySize,
	Simple_GetAllocatedMemorySize,
	Simple_GetTotalRequestedMemorySize,
	Simple_GetMaximumAvailableMemorySize,
	0	// Simple_Ioctl
};
