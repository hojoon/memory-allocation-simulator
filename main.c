
#include <stdio.h>

#include "MemoryAllocatorApi.h"
#include "TestSequences/testSequence001.h"
#include "TestSequences/fixedSizeAllocationTest01.h"

void CheckRequestedMemory(void *context) {
	if (context) {
		unsigned long size=0;
		MA_ERROR retval=MA_GetTotalRequestedMemorySize(context,&size);
		if (retval==MA_NO_ERROR) {
			printf("Total requested memory : %ld\r\n",size);
		} else {
			printf("requested memory error : %d - %p %p\r\n",retval,context,&size);
		}
	}
}
void CheckAllocatedMemory(void *context) {
	if (context) {
		unsigned long size=0;
		MA_ERROR retval=MA_GetAllocatedMemorySize(context,&size);
		if (retval==MA_NO_ERROR) {
			printf("Total allocated memory : %ld\r\n",size);
		} else {
			printf("allocated memory error : %d\r\n",retval);
		}
	}
}
void CheckFreeMemory(void *context) {
	if (context) {
		unsigned long size;
		MA_ERROR retval=MA_GetFreeMemorySize(context,&size);
		if (retval==MA_NO_ERROR) {
			printf("Free memory size : %ld\r\n",size);
		} else {
			printf("Free memory size error : %d\r\n",retval);
		}
	}
}
void CheckMaximumAvailableMemory(void *context) {
	if (context) {
		unsigned long size;
		MA_ERROR retval=MA_GetMaximumAvailableMemorySize(context,&size);
		if (retval==MA_NO_ERROR) {
			printf("Maximum available memory : %ld\r\n",size);
		} else {
			printf("available memory error : %d\r\n",retval);
		}
	}
}

int main(int argc, void *argv[]) {
	int retval=0;

	printf("test\r\n");

	MA_InitializeMemoryAllocation();

	retval=MA_SetMemoryAllocatorByName("Simple");
	if (retval==MA_NO_ERROR) {
		void *context;
		unsigned char *memoryPool;
		unsigned long memoryPoolSize;
		
		memoryPoolSize=10*1024*1024;
		memoryPool=(unsigned char *)MA_GeneralMalloc(memoryPoolSize);
		if (memoryPool) {
            retval=TestSequence001(memoryPool, memoryPoolSize, 16);
            if (retval==MA_NO_ERROR) {
                retval=FixedSizeAllocationTest01(memoryPool, memoryPoolSize, 16);
            }
		} else {
			retval=MA_SYSTEM_MEMORY_FULL;
		}
	}
	return retval;
}

