/* Include the sbrk function */
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int has_initialized = 0;
void *managed_memory_start;
void *last_valid_address;

struct mem_control_block {
    int is_available;
    int size;
};

void malloc_init()
{
    /*  从OS 中获取当前虚拟地址 */
    last_valid_address = sbrk(0);
    /*
     * 初始化当前的地址起点
     */
    managed_memory_start = last_valid_address;
    /* 设置初始化完成的标记 */
    has_initialized = 1;
}

void free(void *firstbyte) {
    struct mem_control_block *mcb;
    /* 先把指针指向记录结构的起始 */
    mcb = (struct mem_control_block *)((char*)firstbyte - sizeof(struct mem_control_block));
    /* 把当前块标记为可用 */
    mcb->is_available = 1;
    //结束
    return;
}

void *malloc(long numbytes) {
    /* 用来存储当前正查找的地址*/
    void *current_location;
    
    struct mem_control_block *current_location_mcb;
    /* 用来存储目标指针（也就是满足条件的地址）*/
    void *memory_location;
    /* 保险 */
    if(! has_initialized)   {
        malloc_init();
    }
    /* 我们需要把记录结构的大小也计算进去 */
    numbytes = numbytes + sizeof(struct mem_control_block);
    /* 初始化 */
    memory_location = 0;
    current_location = managed_memory_start;
    while(current_location != last_valid_address)
    {
        /* current_location 和 current_location_mcb 指向同一地址
         * 但是current_location是用来计算地址的，
         * 而current_location_mcb是用来表示struct的，可以直接使用
         */
        current_location_mcb =
            (struct mem_control_block *)current_location;
        if(current_location_mcb->is_available)
        {
            if(current_location_mcb->size >= numbytes)
            {
                /* 找到了合适的 地址了，立刻把它占用 */
                current_location_mcb->is_available = 0;
                memory_location = current_location;
                break;
            }
        }
        current_location = (char*)current_location + current_location_mcb->size;
    }
    /* 如果未找到合适的，证明需要另开空间
     */
    if(! memory_location)
    {
        sbrk(numbytes);
        //把目标地址设置成最后可用的地址
        memory_location = last_valid_address;
        last_valid_address = (char*)last_valid_address + numbytes;
        //新分配 管理单元的 内存 并初始化
        current_location_mcb = (struct mem_control_block *)memory_location;
        current_location_mcb->is_available = 0;
        current_location_mcb->size = numbytes;
    }

    memory_location = (char*)memory_location + sizeof(struct mem_control_block);
    return memory_location;
 }

int main(){
    malloc_init();
    int * num = (int*)malloc(sizeof(int));
    int * arrs = (int*)malloc(sizeof(int)*10);
    free(num);
    free(arrs);
    printf("finished.\n");
    return 0;
}