
#include "../MemoryAllocators/memoryAllocatorApi.h"
#include "TestSequence001.h"


MA_ERROR TestSequence001(void *memoryPool, unsigned long memoryPoolSize, unsigned long minimumAllocationSize) {
    MA_ERROR retval=MA_INVALID_PARAM;

    printf("Starting %s\r\n",__FUNCTION__);

    if (memoryPool && memoryPoolSize) {
        void *context;
        retval=MA_InitializeMemoryPool(&context,memoryPool,memoryPoolSize,minimumAllocationSize);
        if (retval==MA_NO_ERROR) {
            int i;
            void *memory[10];
            unsigned long allocatedMemorySize;
            
            memset(memory,0,sizeof(void*)*10);
            CheckFreeMemory(context);
            CheckMaximumAvailableMemory(context);
            for (i=0; i<10; i++) {
                //printf("\r\nMemory allocate %d\r\n",i);
                retval=MA_AllocateMemory(context,600+i*100,&memory[i]);
                if (retval!=MA_NO_ERROR) {
                    printf("\r\nAllocation error : %d\r\n",retval);
                    break;
                }
                MA_CheckMemory(context,"alloc",i);
            }
            for (i=0; i<10; i++) {
                if (memory[i]) {
                    //printf("\r\nMemory free %d\r\n",i);
                    MA_ERROR ret=MA_FreeMemory(context,memory[i]);
                    if (ret!=MA_NO_ERROR) {
                        printf("memory free failed ...[%d]\r\n",retval);
                    }
                }
            }
            if (MA_GetAllocatedMemorySize(context,&allocatedMemorySize)==MA_NO_ERROR) {
                if (allocatedMemorySize!=0) {
                    MA_CheckMemory(context,"Finished",0);
                } else {
                    printf("Finished %s\r\n",__FUNCTION__);
                }
            }
            MA_DeinitializeMemoryPool(context);
        }
    }

    printf("Result of %s = %d\r\n",__FUNCTION__,retval);
    
    return retval;
}