#include <stdio.h>
#include <iostream>
#include <fstream>

using namespace std;

int mem[128][32];
int disk[128][32];

int trans(char a) {
    int b;
    if ((int)a < 58) {
        b = (int)(a-'0');
    }
    else {
        b = (int)(a-'a')+10;
    }
    return b;
}

int start =  0x6c;

int search(int addr){
    //找到三个偏移量：pde pte physical 的偏移
    int pde_off = (addr>>10) & 31;
    int pte_off = (addr>>5) & 31;
    int phy_off = addr & 31;
    //开始找第一个page directory entry的内容
    int pde_con = mem[start][pde_off];
    //看看是不是valid
    bool pde_valid = pde_con >> 7;
    printf("pde index:0x%x  pde contents:(0x%x, bin: , valid %d, pfn 0x%x(page 0x%x))\n", pde_off, pde_con, pde_valid, pde_con&127, pde_con&127);
    if (pde_valid){
        //把valid位去掉
        pde_con &= 127;
        //直接通过该值，得到对应的数组地址
        int pte_addr = pde_con;
        //通过pte offset 来找内容
        int pte_con = mem[pte_addr][pte_off];
        //找到valid位
        bool pte_valid = pte_con >> 7;
        printf("pte index:0x%x  pte contents:(0x%x, bin: , valid %d, pfn 0x%x)\n", pte_off, pte_con, pte_valid, pte_con&127);
        if (pte_valid){
            //重复以上过程
            pte_con &= 127;
            int phy_addr = pte_con;
            //找到了！
            int phy_con = mem[phy_addr][phy_off];
            printf(" To Physical Address 0x%x --> value : 0x%x\n", (32*pte_con+phy_off), phy_con);
            return phy_con;
        } else {
            pte_con &= 127;
            int disk_addr = pte_con;
            //disk
            int disk_con = disk[disk_addr][phy_off];
            printf("To Disk Sector Address 0x%x --> value : 0x%x\n", (32*pte_con+phy_off), disk_con);
            return disk_con;
        }
    } else {
        printf("Fault (page directory entry not valid)\n");
        return -1;
    }
}

void readfile(char* filename, int flag){
    ifstream in(filename);  
    char buffer[200];
    if (! in.is_open()) {
        cout << "Error opening file"; 
    }  
    else {      
        int l = 128;
        int j = 0;
        while (l--) { 
            in.getline (buffer,200); 
            // cout << buffer << endl;
            
            for (int i = 0; i < 32; i++) {
                if (flag)
                    disk[j][i] = trans(buffer[i*3+9])*16+trans(buffer[i*3+10]);
                else 
                    mem[j][i] = trans(buffer[i*3+9])*16+trans(buffer[i*3+10]);
            }
            j++;
           
        } 
    } 

}

int main(){
    readfile("in1.txt", 0);
    readfile("in2.txt", 1);
    search(0x6653);
    search(0x1c13);
    search(0x6890);
    search(0xaf6);
    search(0x1e6f);
    return 0;
}