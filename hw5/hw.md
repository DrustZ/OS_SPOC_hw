##请回答下列虚地址是否有合法对应的物理内存，请给出对应的pde index, pde contents, pte index, pte contents

-	第一个：Virtual Address 6c74
	
		Virtual Address 6c74:
		  --> pde index:0x1b  pde contents:(valid 1, pfn 0xa0, 物理地址0x400)
		    --> pte index:0xb  pte contents:(valid 0, pfn 0xe1)
		      --> Translates to Physical Address 0x3 --> Value: 06
	
-	Virtual Address 6b22
	
		Virtual Address 6b22:
		  --> pde index:0x1a  pde contents:(valid 1, pfn 0xd2, 物理地址0xa40)
		    --> pte index:0x19  pte contents:(valid 1, pfn 0xc7)
		       --> Translates to Physical Address 0x8e0 --> Value: 0x1a = 26


##请基于你对原理课二级页表的理解，并参考Lab2建页表的过程，设计一个应用程序（可基于python, ruby, C, C++，LISP等）可模拟实现(2)题中描述的抽象OS，可正确完成二级页表转换

