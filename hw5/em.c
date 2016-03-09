#include <stdio.h>
#include <iostream>
#include <fstream>

using namespace std;

int mem[128][32];

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

int start = 0x11;

int search(int addr){
    //找到三个偏移量：pde pte physical 的偏移
    int pde_off = (addr>>10) & 31;
    int pte_off = (addr>>5) & 31;
    int phy_off = addr & 31;
    //开始找第一个page directory entry的内容
    int pde_con = mem[start][pde_off];
    //看看是不是valid
    bool pde_valid = pde_con >> 7;
    if (pde_valid){
        //把valid位去掉
        pde_con &= 127;
        //直接通过该值，得到对应的数组地址
        int pte_addr = pde_con;
        //通过pte offset 来找内容
        int pte_con = mem[pte_addr][pte_off];
        //找到valid位
        bool pte_valid = pte_con >> 7;
        if (pte_valid){
            //重复以上过程
            pte_con &= 127;
            int phy_addr = pte_con;
            //找到了！
            int phy_con = mem[phy_addr][phy_off];
            printf("mem found, value : %u\n", phy_con);
            return phy_con;
        } else {
            printf("Fault (page table entry not valid)\n");
            return -1;
        }

    } else {
        printf("Fault (page directory entry not valid)\n");
        return -1;
    }
}

int main(){
    ifstream in("in.txt");  
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
                mem[j][i] = trans(buffer[i*3+9])*16+trans(buffer[i*3+10]);
            }
            j++;
           
        } 
    }   

    search(0x6c74);
    search(0x6b22);
    return 0;
}