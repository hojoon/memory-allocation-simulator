
#include <stdio.h>
#include <stdlib.h>

#include "memoryAllocatorApi.h"
#include "memoryAllocatorInterface.h"

#include "SystemDefault/systemDefaultMemoryAllocator.h"
#include "Simple/simpleMemoryAllocator.h"

struct MemoryContext {
	struct MemoryAllocator *memoryAllocator;
	void *memoryAllocatorContext;
	void *memoryPool;
	unsigned long memoryPoolSize;
	unsigned long minimumAllocationSize;
	
	unsigned long requestedAllocationSize;
	
};

static struct MemoryAllocator *memoryAllocatorSelected;

static struct MemoryAllocator *memoryAllocators[]={
	&systemDefaultMemoryAllocator,
	&simpleMemoryAllocator,
	
	0
};

void *MA_GeneralMalloc(unsigned long size) {
	void *retval;
	
	retval=(void*)malloc(size);
	
	return retval;
}

void MA_GeneralFree(void *memory) {
	free(memory);
}

MA_ERROR MA_InitializeMemoryAllocation(void) {
	MA_ERROR retval;
	int i;
	int memoryAllocatorsCount=sizeof(memoryAllocators)/sizeof(struct MemoryAllocator);

	memoryAllocatorSelected=0;
	
	for (i=0; i<memoryAllocatorsCount; i++) {
		if (memoryAllocators[i]) {
			if (memoryAllocators[i]->name) {
				printf("Memory allocator, %s is added.\r\n",memoryAllocators[i]->name);
			} else {
				printf("Memory allocator with no name is added.\r\n");
			}
		} else {
			printf("Check the list, memoryAllocators!!!\r\n");
		}
	}
	
	retval=MA_NO_ERROR;
	return retval;
}

MA_ERROR MA_SetMemoryAllocator(struct MemoryAllocator *memoryAllocator) {
	MA_ERROR retval=MA_INVALID_PARAM;
	
	if (memoryAllocator) {
		if (memoryAllocator->InitializeMemoryPool &&
				memoryAllocator->DeinitializeMemoryPool && 
				memoryAllocator->AllocateMemory &&
				memoryAllocator->FreeMemory) {
			memoryAllocatorSelected=memoryAllocator;
			retval=MA_NO_ERROR;
		} else {
			retval=MA_INVALID_MEMORY_ALLOCATOR;
		}
	}
	return retval;
}

MA_ERROR MA_SetMemoryAllocatorByName(unsigned char *name) {
	MA_ERROR retval=MA_INVALID_PARAM;

	if (name) {
		int i;
		for (i=0; i<10; i++) {
			if (memoryAllocators[i]==0) {
				break;
			}
			printf("%d %s\r\n",i,memoryAllocators[i]->name);
			if (strcmp(name, memoryAllocators[i]->name)==0) {
				retval=MA_SetMemoryAllocator(memoryAllocators[i]);
				break;
			}
		};
	}
	return retval;
}

MA_ERROR MA_InitializeMemoryPool(void **context,
		void *memoryPool, unsigned long memoryPoolSize,
		unsigned long minimumAllocationSize) {
	MA_ERROR retval=MA_INVALID_PARAM;	

	if (context && memoryPool && memoryPoolSize && minimumAllocationSize) {
		if (memoryAllocatorSelected) {
			struct MemoryContext *memoryContext;

			memoryContext=(struct MemoryContext *)MA_GeneralMalloc(sizeof(struct MemoryContext));
			if (memoryContext) {
				void *memoryAllocatorContext;
				if (memoryAllocatorSelected->InitializeMemoryPool) {
					retval=(memoryAllocatorSelected->InitializeMemoryPool)(&memoryAllocatorContext,
							memoryPool, memoryPoolSize, minimumAllocationSize);
					if (retval==MA_NO_ERROR) {
						memoryContext->memoryAllocatorContext=memoryAllocatorContext;
						memoryContext->memoryAllocator=memoryAllocatorSelected;
						memoryContext->memoryPool=memoryPool;
						memoryContext->memoryPoolSize=memoryPoolSize;
						memoryContext->minimumAllocationSize=minimumAllocationSize;
						memoryContext->requestedAllocationSize=0;
						*context=memoryContext;
					}
				}
				if (retval!=MA_NO_ERROR) {
					MA_GeneralFree(memoryContext);
				}
			} else {
				retval=MA_SYSTEM_MEMORY_FULL;
			}
		} else {
			retval=MA_NOT_INITIALIZED;
		}
	}
	return retval;
}

MA_ERROR MA_DeinitializeMemoryPool(void *context) {
	MA_ERROR retval=MA_INVALID_PARAM;	

	if (context) {
		struct MemoryContext *memoryContext=context;
		
		if (memoryContext->memoryAllocator &&
				memoryContext->memoryAllocator->DeinitializeMemoryPool) {
			retval=(memoryContext->memoryAllocator->DeinitializeMemoryPool)(
					memoryContext->memoryAllocatorContext);
		} else {
			retval=MA_INVALID_MEMORY_ALLOCATOR;
		}
		MA_GeneralFree(memoryContext);
	}
	return retval;
}

MA_ERROR MA_AllocateMemory(void *context, unsigned long size, void **memory) {
	MA_ERROR retval=MA_INVALID_PARAM;	

	if (context && size && memory) {
		struct MemoryContext *memoryContext=context;
		
		if (memoryContext->memoryAllocator &&
				memoryContext->memoryAllocator->AllocateMemory) {
			retval=(memoryContext->memoryAllocator->AllocateMemory)(
					memoryContext->memoryAllocatorContext, size, memory);
		} else {
			retval=MA_INVALID_MEMORY_ALLOCATOR;
		}
	}
	return retval;
}

MA_ERROR MA_FreeMemory(void *context, void *memory) {
	MA_ERROR retval=MA_INVALID_PARAM;	

	if (context && memory) {
		struct MemoryContext *memoryContext=context;
		
		if (memoryContext->memoryAllocator &&
				memoryContext->memoryAllocator->FreeMemory) {
			retval=(memoryContext->memoryAllocator->FreeMemory)(
					memoryContext->memoryAllocatorContext, memory);
		} else {
			retval=MA_INVALID_MEMORY_ALLOCATOR;
		}
	}
	return retval;
}

MA_ERROR MA_GetFreeMemorySize(void *context, unsigned long *memorySize) {
	MA_ERROR retval=MA_INVALID_PARAM;	

	if (context && memorySize) {
		struct MemoryContext *memoryContext=context;
		
		if (memoryContext->memoryAllocator &&
				memoryContext->memoryAllocator->GetFreeMemorySize) {
			retval=(memoryContext->memoryAllocator->GetFreeMemorySize)(
					memoryContext->memoryAllocatorContext, memorySize);
		} else {
			retval=MA_NOT_SUPPORTED;
		}
	}
	return retval;
}

MA_ERROR MA_GetAllocatedMemorySize(void *context, unsigned long *memorySize) {
	MA_ERROR retval=MA_INVALID_PARAM;	

	if (context && memorySize) {
		struct MemoryContext *memoryContext=context;
		
		if (memoryContext->memoryAllocator &&
				memoryContext->memoryAllocator->GetAllocatedMemorySize) {
			retval=(memoryContext->memoryAllocator->GetAllocatedMemorySize)(
					memoryContext->memoryAllocatorContext, memorySize);
		} else {
			retval=MA_NOT_SUPPORTED;
		}
	}
	return retval;
}

MA_ERROR MA_GetTotalRequestedMemorySize(void *context, unsigned long *memorySize) {
	MA_ERROR retval=MA_INVALID_PARAM;	

	if (context && memorySize) {
		struct MemoryContext *memoryContext=context;
		
		if (memoryContext->memoryAllocator &&
				memoryContext->memoryAllocator->GetTotalRequestedMemorySize) {
			retval=(memoryContext->memoryAllocator->GetTotalRequestedMemorySize)(
					memoryContext->memoryAllocatorContext, memorySize);
		} else {
			retval=MA_NOT_SUPPORTED;
		}
	}
	return retval;
}

MA_ERROR MA_GetMaximumAvailableMemorySize(void *context, unsigned long *memorySize) {
	MA_ERROR retval=MA_INVALID_PARAM;	

	if (context && memorySize) {
		struct MemoryContext *memoryContext=context;
		
		if (memoryContext->memoryAllocator &&
				memoryContext->memoryAllocator->GetMaximumAvailableMemorySize) {
			retval=(memoryContext->memoryAllocator->GetMaximumAvailableMemorySize)(
					memoryContext->memoryAllocatorContext, memorySize);
		} else {
			retval=MA_NOT_SUPPORTED;
		}	
	}
	return retval;
}

MA_ERROR MA_Ioctl(unsigned long command, void *context,
		void *param1, void *param2, void *param3, void *param4) {
	MA_ERROR retval=MA_INVALID_PARAM;

	if (command) {
	
	
	}
	return retval;
}


static unsigned char currentTitle[100]="";
static unsigned char logEnabled=1;

void MA_SetLogTitle(unsigned char *title) {
    strcpy(currentTitle,title);
}

void MA_EnableLog(void) {
    logEnabled=1;
}

void MA_DisableLog(void) {
    logEnabled=0;
}

void MA_CheckMemory(void *context,unsigned char *string, unsigned char value) {
	if (logEnabled && context) {
		unsigned long totalRequestedSize=0;
        unsigned long allocatedMemorySize=0;
        unsigned long freeMemory=0;
        unsigned long maximumAvailableSize=0;
        
		(void)MA_GetTotalRequestedMemorySize(context,&totalRequestedSize);
		(void)MA_GetAllocatedMemorySize(context,&allocatedMemorySize);
		(void)MA_GetFreeMemorySize(context,&freeMemory);
		(void)MA_GetMaximumAvailableMemorySize(context,&maximumAvailableSize);
        
        printf("%s",currentTitle);
        if (string) {
            printf("%s\t: ",string);
        }
        if (value>0) {
            printf("%03d\t: ",value);
        }

        printf("%8lu %8lu %8lu %8lu",totalRequestedSize, allocatedMemorySize,
               freeMemory, maximumAvailableSize);
        printf(" (%5.1f%%)\r\n",
               (allocatedMemorySize)?(double)totalRequestedSize*100/allocatedMemorySize:0);
	}
}

