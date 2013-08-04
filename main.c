
#include <stdio.h>

#include "MemoryAllocatorApi.h"

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
			retval=MA_InitializeMemoryPool(&context,memoryPool,memoryPoolSize,1024);
			if (retval==MA_NO_ERROR) {
				int i;
				void *memory[10];
				memset(memory,0,sizeof(void*)*10);
				CheckFreeMemory(context);
				CheckMaximumAvailableMemory(context);
				for (i=0; i<10; i++) {
					printf("\r\nMemory allocate %d\r\n",i);
					retval=MA_AllocateMemory(context,600+i*100,&memory[i]);
					if (retval!=MA_NO_ERROR) {
						printf("\r\nAllocation error : %d\r\n",retval);
						break;
					}
					CheckRequestedMemory(context);
					CheckAllocatedMemory(context);
					CheckFreeMemory(context);
					CheckMaximumAvailableMemory(context);
				}
				for (i=0; i<10; i++) {
					if (memory[i]) {
						printf("\r\nMemory free %d\r\n",i);
						retval=MA_FreeMemory(context,memory[i]);
						if (retval!=MA_NO_ERROR) {
							printf("memory free failed ...[%d]\r\n",retval);
						}
						CheckRequestedMemory(context);
						CheckAllocatedMemory(context);
						CheckFreeMemory(context);
						CheckMaximumAvailableMemory(context);
					}
				}
				MA_DeinitializeMemoryPool(context);
			}
		} else {
			retval=MA_SYSTEM_MEMORY_FULL;
		}
	}
	return retval;
}

