
#include "../MemoryAllocators/memoryAllocatorApi.h"
#include "TestSequence001.h"


MA_ERROR TestSequence001(void *memoryPool, unsigned long memoryPoolSize, unsigned long minimumAllocationSize) {
    MA_ERROR retval=MA_INVALID_PARAM;
    if (memoryPool && memoryPoolSize) {
        void *context;
        retval=MA_InitializeMemoryPool(&context,memoryPool,memoryPoolSize,minimumAllocationSize);
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
    }
}