##v9-cpu系统调用实现
###v9-cpu中os4.c的系统调用中参数传递代码分析。

程序首先运行在用户态，之后时钟中断，进入trap（），由

	task0_sp -= 2; *task0_sp = &task0;
	（push进当前sp的地址）

	task0_sp -= 2; *task0_sp = USER; // fault code
	(把当前的TRAP标志push进栈）

	task0_sp -= 2; *task0_sp = 0; // a
	task0_sp -= 2; *task0_sp = 0; // b
	task0_sp -= 2; *task0_sp = 0; // c
	（这三句把几个关键的寄存器push进栈）

	task0_sp -= 2; *task0_sp = &task0_stack[1000]; //user stack
	（把用户态的栈顶push进栈）
	task0_sp -= 2; *task0_sp = &trapret;（最后压处理的返回地址）
	
可知，kernel stack首先做好了构造syscall栈的操作，而参数传递过程在alltrap中： 
	
	  asm(PSHA);
	  asm(PSHB);
	  asm(PSHC);//先把寄存器压栈
	  asm(LUSP);
	  asm(PSHA);//再把用户态中的下一步sp指令地址压栈

因此，在处理中断的时候：

	trap(int *sp, int c, int b, int a, int fc, unsigned *pc)

分析参数，即sp为当前栈顶（可能是用户态也可能是内核态，取决于中断前的状态，本题为用户态的栈顶）

c，b，a为寄存器值，fc为trap标志，pc即为中断前下一步指令地址（本题为task0入口）

###v9-cpu中os4.c的系统调用中返回结果的传递代码分析

返回结果传递代码，也就是alltraps中的（在trapret中也是同样代码）

	 asm(POPA);
	 asm(SUSP);
	 asm(POPC);
	 asm(POPB);
	 asm(POPA);
	 asm(RTI) ;
	 
代码段，前两句首先将用户栈的栈顶pop出，赋值给usp，然后把cba寄存器依次pop，RTI返回：

	RTI： return from interrupt, POP fault code, pc, sp,  if fault code== USER, then switch to user mode; if has pending interrupt, process the interrupt
	
###理解v9-cpu中os4.c的系统调用编写和含义

系统调用：此文件只处理了时钟中断以及syscall的中断，因此当用户系统调用的时候，可以捕捉到，并且做处理：

	case FSYS + USER: // syscall
    switch (pc[-1] >> 8) {
    case S_write: a = sys_write(a, b, c); break;
    default: sys_write(1, "panic! unknown syscall\n", 23); asm(HALT);
    }
    break;
    
用户系统调用首先调用write函数，在write中，把trap的标志设为S_write
	
	 asm(TRAP,S_write);
	 
之后中断，然后由刚刚的case语句进行处理