#include <stdio.h>

uint start = 0x11;

int search(uint addr){
    //找到三个偏移量：pde pte physical 的偏移
    uint pde_off = (addr>>10) & 31;
    uint pte_off = (addr>>5) & 31;
    uint phy_off = addr & 31;
    //开始找第一个page directory entry的内容
    uint pde_con = mem[start][pdeoff];
    //看看是不是valid
    bool pde_valid = pde_con >> 7;
    if (pte_valid){
        //把valid位去掉
        pde_con &= 127;
        //直接通过该值，得到对应的数组地址
        uint pte_addr = pde_con;
        //通过pte offset 来找内容
        uint pte_con = mem[pte_addr][pte_off];
        //找到valid位
        bool pte_valid = pte_con >> 7;
        if (pte_valid){
            //重复以上过程
            pte_con &= 127;
            uint phy_addr = pte_con;
            //找到了！
            uint phy_con = mem[phy_addr][phy_off];
            printf("mem found, value : %u\n", phy_con);
            return phy_con;
        } else {
            printf("Fault (page table entry not valid)\n", );
            return -1;
        }

    } else {
        printf("Fault (page directory entry not valid)\n");
        return -1;
    }
}

int main(){
    search();
    return 0;
}