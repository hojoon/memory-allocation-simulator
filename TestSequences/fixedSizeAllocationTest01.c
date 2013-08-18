
#include "../MemoryAllocators/memoryAllocatorApi.h"
#include "fixedSizeAllocationTest01.h"

#define LOOP_COUNT_MAX  100000
static MA_ERROR RepeatingAllocAndFree(void *context, unsigned long size, unsigned long count) {
    MA_ERROR retval=MA_INVALID_PARAM;
    int i;
    void **memoryArray;

    //printf("Starting %s:%lu-%lu\r\n",__FUNCTION__,size,count);
    
    //MA_CheckMemory(context,"Allocating count",count);
    if (count==0) {
        count=LOOP_COUNT_MAX;
    }
    memoryArray=MA_GeneralMalloc(sizeof(void*)*count);
    if (memoryArray) {
        memset(memoryArray,0,count*sizeof(void*));
        for (i=0; i<count; i++) {
            retval=MA_AllocateMemory(context, size, &memoryArray[i]);
            if (retval!=MA_NO_ERROR) {
                break;
            }
        }
        MA_CheckMemory(context,"Allocated",0);
        if (count==LOOP_COUNT_MAX && retval==MA_MEMORY_FULL) {
            retval=MA_NO_ERROR;
        }
        if (retval==MA_NO_ERROR) {
            unsigned long allocatedMemorySize;
            for (i=0; i<count; i++) {
                if (memoryArray[i]) {
                    retval=MA_FreeMemory(context, memoryArray[i]);
                    if (retval!=MA_NO_ERROR) {
                        break;
                    }
                }
            }
            if (MA_GetAllocatedMemorySize(context, &allocatedMemorySize)==MA_NO_ERROR) {
                if (allocatedMemorySize!=0) {
                    MA_CheckMemory(context,"Freed",0);
                }
            }
        } else {
            // free memory to quit
            for (i=0; i<count; i++) {
                if (memoryArray[i]) {
                    (void)MA_FreeMemory(context, memoryArray[i]);
                }
            }
        }
        MA_GeneralFree(memoryArray);
    } else {
        retval=MA_SYSTEM_MEMORY_FULL;
    }

    if (retval!=MA_NO_ERROR) {
        printf("Result of %s = %d\r\n",__FUNCTION__,retval);
    }
    return retval;
}

static MA_ERROR Loop1(void *context, unsigned long size) {
    MA_ERROR retval=MA_INVALID_PARAM;
    static unsigned long countArray[10]={10, 100, 1000, 10000, 0};
    int availableMemorySize;
    
    //printf("Starting %s:%lu\r\n",__FUNCTION__,size);
    
    if (context) {
        retval=MA_GetFreeMemorySize(context, &availableMemorySize);
        if (retval==MA_NO_ERROR) {
            int i;
            for (i=0;i<5;i++) {
                if ((countArray[i]*size)>availableMemorySize) {
                    printf("MemoryPool is not enough : %lu/%lu\r\n",countArray[i]*size,availableMemorySize);
                    break;
                }
                retval=RepeatingAllocAndFree(context, size, countArray[i]);
                if (retval!=MA_NO_ERROR) {
                    break;
                }
            }
        }
    }
    if (retval!=MA_NO_ERROR) {
        printf("Result of %s = %d\r\n",__FUNCTION__,retval);
    }
    return retval;
}

static MA_ERROR Loop2(void *context) {
    MA_ERROR retval=MA_INVALID_PARAM;
    unsigned long sizeArray[]={512, 1024, 1500, 30, 0};
    int availableMemorySize;
    
    //printf("Starting %s\r\n",__FUNCTION__);

    if (context) {
        int i=0;
        while (sizeArray[i]!=0) {
            retval=Loop1(context, sizeArray[i]);
            if (retval!=MA_NO_ERROR) {
                break;
            }
            i++;
        };
    }
    if (retval!=MA_NO_ERROR) {
        printf("Result of %s = %d\r\n",__FUNCTION__,retval);
    }
    return retval;
}

MA_ERROR FixedSizeAllocationTest01(void *memoryPool, unsigned long memoryPoolSize, unsigned long minimumAllocationSize) {
    MA_ERROR retval=MA_INVALID_PARAM;
    
    printf("\nStarting %s\r\n",__FUNCTION__);
    
    if (memoryPool && memoryPoolSize) {
        void *context;
        
        MA_SetLogTitle("FixedSizeTest");
        retval=MA_InitializeMemoryPool(&context,memoryPool,memoryPoolSize,minimumAllocationSize);
        if (retval==MA_NO_ERROR) {
            int i;
            
            //MA_CheckMemory(context,"Start Value",0);

            retval=Loop2(context);
            
            //MA_CheckMemory(context,"End Value",0);
            
            MA_DeinitializeMemoryPool(context);
        }
    }
    if (retval!=MA_NO_ERROR) {
        printf("Result of %s = %d\r\n",__FUNCTION__,retval);
    }
    return retval;
}