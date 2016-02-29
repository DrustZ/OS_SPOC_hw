##linux系统调用分析
###通过分析lab1_ex0了解Linux应用的系统调用编写和含义
-	objdump命令：
	反汇编可执行文件,把二进制或者可执行文件反编译为汇编文件
	
	本题输出的汇编：
	
			.include "defines.h"
		.data
		hello:
			.string "hello world\n"
		
		.globl	main
		main:
			movl	$SYS_write,%eax
			movl	$STDOUT,%ebx
			movl	$hello,%ecx
			movl	$12,%edx
			int	$0x80
		
			ret
	
	它通过调用SYS_write，把"hello world\n"输出（stout）屏幕。
	
-	nm:
	打印可执行文件的程序段信息（符号信息，BSS，CS等）
	
	本题输出：
	
		00000002 a AF_INET
		0804a040 B __bss_start
		0804a040 b completed.6590
		0804a014 D __data_start
		0804a014 W data_start
		08048330 t deregister_tm_clones
		080483a0 t __do_global_dtors_aux
		08049f0c t __do_global_dtors_aux_fini_array_entry
		0804a018 D __dso_handle
		08049f14 d _DYNAMIC
		0804a040 D _edata
		0804a044 B _end
		08048464 T _fini
		08048478 R _fp_hw
		080483c0 t frame_dummy
		08049f08 t __frame_dummy_init_array_entry
		08048530 r __FRAME_END__
		0804a000 d _GLOBAL_OFFSET_TABLE_
		         w __gmon_start__
		0804a01c d hello
		08048294 T _init
		08049f0c t __init_array_end
		08049f08 t __init_array_start
		0804847c R _IO_stdin_used
		00000006 a IPPROTO_TCP
		         w _ITM_deregisterTMCloneTable
		         w _ITM_registerTMCloneTable
		08049f10 d __JCR_END__
		08049f10 d __JCR_LIST__
		         w _Jv_RegisterClasses
		08048460 T __libc_csu_fini
		080483f0 T __libc_csu_init
		         U __libc_start_main@@GLIBC_2.0
		0804a029 D main
		00000001 a MAP_SHARED
		00000001 a PROT_READ
		08048360 t register_tm_clones
		00000002 a SEEK_END
		00000001 a SOCK_STREAM
		080482f0 T _start
		00000001 a STDOUT
		00000006 a SYS_close
		0000003f a SYS_dup2
		0000000b a SYS_execve
		00000001 a SYS_exit
		00000002 a SYS_fork
		00000013 a SYS_lseek
		0000005a a SYS_mmap
		0000005b a SYS_munmap
		00000005 a SYS_open
		00000066 a SYS_socketcall
		00000005 a SYS_socketcall_accept
		00000002 a SYS_socketcall_bind
		00000004 a SYS_socketcall_listen
		00000001 a SYS_socketcall_socket
		00000004 a SYS_write
		0804a040 D __TMC_END__
		08048320 T __x86.get_pc_thunk.bx
	
-	file:

	file [-beLvz][-f <名称文件>][-m <魔法数字文件>...][文件或目录...] 
	
	通过file指令，我们得以辨识该文件的类型
	
	本题输出

		lab1-ex0.exe: ELF 32-bit LSB executable, Intel 80386, version 1 (SYSV), dynamically linked, interpreter /lib/ld-linux.so.2, for GNU/Linux 2.6.32, BuildID[sha1]=ee322e8193a7d18b15891738abe7eaf4398217a3, not stripped

系统调用的含义：

	系统调用是由操作系统实现提供的所有系统调用所构成的集合即程序接口或应用编程接口(Application Programming Interface，API)。是应用程序同系统之间的接口。
	用户态的应用程序与操作系统交互的唯一手段是通过系统调用，当系统调用后，会进入内核态，又操作系统进行相关处理函数的调用。
	为什么用户不可以直接进入内核态？这是为了安全考虑，因为cpu信任操作系统，如果没有系统调用，用户态应用程序随意操作，则会有危险。
	
###通过调试lab1_ex1了解Linux应用的系统调用执行过程
-	strace: 用来打印一个程序在执行中的系统调用信息,跟踪进程执行时的系统调用和所接收的信号，它可以跟踪到一个进程产生的系统调用,包括参数，返回值，执行消耗的时间

系统调用执行过程：

	首先用户态发出系统调用的信号，当它发出系统调用后，将通过Trap Gate完成了从用户态（ring 3）的用户进程进了核心态（ring 0）的OS kernel
	
	在执行系统调用的时候，不是通过“CALL”指令而是通过“INT”指令发起调用；
	不是通过“RET”指令，而是通过“IRET”指令完成调用返回；
	当到达内核态后，操作系统需要严格检查系统调用传递的参数，确保不破坏整个系统的安全性；
	执行系统调用可导致进程等待某事件发生，从而可引起进程切换；
	
	1，用户态的应用程序需要做：
	在执行trap函数前，软件还需进一步保存执行系统调用前的执行现场，即把与用户进程继续执行所需的相关寄存器等当前内容保存到当前进程的中断帧trapframe中
	
	用于保存用户态的用户进程执行现场的trapframe的内容填写完毕，操作系统可开始完成具体的系统调用服务
	
	2，CPU需要做：
	对于用户态的系统调用，这时CPU会从当前程序的TSS信息（该信息在内存中的起始地址存在TR寄存器中）里取得该程序的内核栈地址，即包括内核态的ss和esp的值，并立即将系统当前使用的栈切换成新的内核栈。这个栈就是即将运行的中断服务程序要使用的栈。紧接着就将当前程序使用的用户态的ss和esp压到新的内核栈中保存起来；
	
	CPU需要开始保存当前被打断的程序的现场（即一些寄存器的值），以便于将来恢复被打断的程序继续执行。这需要利用内核栈来保存相关现场信息，即依次压入当前被打断	程序使用的eflags，cs，eip，errorCode（如果是有错误码的异常）信息；
	
	CPU利用中断服务例程的段描述符将其第一条指令的地址加载到cs和eip寄存器中，开始执行中断服务例程。这意味着先前的程序被暂停执行，中断服务程序正式开始工作。
	
中断结束后，CPU会执行：
	
	每个中断服务例程在有中断处理工作完成后需要通过iret（或iretd）指令恢复被打断的程序的执行。CPU执行IRET指令的具体过程如下：
	程序执行这条iret指令时，首先会从内核栈里弹出先前保存的被打断的程序的现场信息，即eflags，cs，eip重新开始执行；
	因为需要特权及转换，需要从内核栈中弹出用户态栈的ss和esp，这样也意味着栈也被切换回原先使用的用户态的栈了；
	
	
针对ex1：strace输出为
	
	
	hello world
	% time     seconds  usecs/call     calls    errors syscall
	------ ----------- ----------- --------- --------- ----------------
	 23.17    0.000019           5         4           mprotect
	 21.95    0.000018           2         8           mmap
	 12.20    0.000010          10         1           write
	 10.98    0.000009           9         1           munmap
	 10.98    0.000009           3         3         3 access
	  9.76    0.000008           4         2           open
	  3.66    0.000003           1         3           fstat
	  2.44    0.000002           2         1           read
	  2.44    0.000002           1         2           close
	  1.22    0.000001           1         1           execve
	  1.22    0.000001           1         1           arch_prctl
	  0.00    0.000000           0         1           brk
	------ ----------- ----------- --------- --------- ----------------
	100.00    0.000082                    28         3 total

首先执行execve,这个执行文件被传进去并且被执行

brk是对heap进行空间分配，准备足够的空间

open是对一些库文件的打开操作

access 查看当前进程是否可以打开对应地址的文件

mprotect 用来修改一段指定内存区域的保护属性

munmap, mmap则是将分配的空间map到RAM（shared libraries）

最后，我们看到write被调用并且输出。



---
---
===

##3.5 ucore系统调用分析
###1.ucore的系统调用中参数传递代码分析。
	struct trapframe *tf = current->tf;
    uint32_t arg[5];
    int num = tf->tf_regs.reg_eax;
    if (num >= 0 && num < NUM_SYSCALLS) {
        if (syscalls[num] != NULL) {
            arg[0] = tf->tf_regs.reg_edx;
            arg[1] = tf->tf_regs.reg_ecx;
            arg[2] = tf->tf_regs.reg_ebx;
            arg[3] = tf->tf_regs.reg_edi;
            arg[4] = tf->tf_regs.reg_esi;
            tf->tf_regs.reg_eax = syscalls[num](arg);
            return ;
        }
    }
    print_trapframe(tf);
    panic("undefined syscall %d, pid = %d, name = %s.\n",
            num, current->pid, current->name);
 由上述代码可以看出，在每次系统调用时，通过从当前进程的中断帧trapframe的tf_regs里面存储的edi，esi，ebx，ecx，edx等寄存器值来获取参数。

具体的把系统调用号放到EAX，其他5个参数a[0]~a[4]分别保存到EDX/ECX/EBX/EDI/ESI五个寄存器中，最多用6个寄存器来传递系统调用的参数，且系统调用的返回结果是EAX。
###2.以getpid为例，分析ucore的系统调用中返回结果的传递代码。
对于getpid他的系统调用号（SYS_getpid=18）保存在EAX中，返回值（调用此库函数的的当前进程号pid）也在EAX中。

当用户进程调用getpid函数，最终执行到“INT T_SYSCALL”指令后，CPU根据操作系统建立的系统调用中断描述符，转入内核态，并跳转到vector128处（kern/trap/vectors.S），开始了操作系统的系统调用执行过程，函数调用和返回操作的关系如下所示：

	vector128(vectors.S)--\>
	\_\_alltraps(trapentry.S)--\>trap(trap.c)--\>trap\_dispatch(trap.c)--
	--\>syscall(syscall.c)--\>sys\_getpid(syscall.c)--\>……--\>\_\_trapret(trapentry.S)
在执行trap函数前，软件还需进一步保存执行系统调用前的执行现场，即把与用户进程继续执行所需的相关寄存器等当前内容保存到当前进程的中断帧trapframe中（注意，在创建进程是，把进程的trapframe放在给进程的内核栈分配的空间的顶部）。软件做的工作在vector128和__alltraps的起始部分：

	vectors.S::vector128起始处:
	  pushl $0
	  pushl $128
	......
	trapentry.S::__alltraps起始处:
	pushl %ds
	  pushl %es
	  pushal
	……

自此，用于保存用户态的用户进程执行现场的trapframe的内容填写完毕，操作系统可开始完成具体的系统调用服务。

在sys_getpid函数中，简单地把当前进程的pid成员变量做为函数返回值就是一个具体的系统调用服务。完成服务后，操作系统按调用关系的路径原路返回到__alltraps中。然后操作系统开始根据当前进程的中断帧内容做恢复执行现场操作。其实就是把trapframe的一部分内容保存到寄存器内容。恢复寄存器内容结束后，调整内核堆栈指针到中断帧的tf_eip处，这是内核栈的结构如下：


	/* below here defined by x86 hardware */
    uintptr_t tf_eip;
    uint16_t tf_cs;
    uint16_t tf_padding3;
    uint32_t tf_eflags;
	/* below here only when crossing rings */
    uintptr_t tf_esp;
    uint16_t tf_ss;
    uint16_t tf_padding4;

这时执行“IRET”指令后，CPU根据内核栈的情况回复到用户态，并把EIP指向tf_eip的值，即“INT T_SYSCALL”后的那条指令。这样整个系统调用就执行完毕了。
###3.以ucore lab8的answer为例，分析ucore 应用的系统调用编写和含义。
首先初始化系统调用对应的中段描述符，在ucore初始化函数kern_init中调用了idt_init函数来初始化中断描述符表，并设置一个特定中断号的中断门，专门用于用户进程访问系统调用。此事由ide_init函数完成

建立好这个中断描述符后，一旦用户进程执行“INT T_SYSCALL”后，由于此中断允许用户态进程产生（注意它的特权级设置为DPL_USER），所以CPU就会从用户态切换到内核态，保存相关寄存器，并跳转到__vectors[T_SYSCALL]处开始执行

然后建立系统调用的用户库准备，为了简化应用程序访问系统调用的复杂性。为此在用户态建立了一个中间层来完成对访问系统调用的封装。用户态最终的访问系统调用函数是syscall，所以应用程序调用的exit/fork/wait/getpid等库函数最终都会调用syscall函数，只是调用的参数不同而已

###4.以ucore lab8的answer为例，尝试修改并运行ucore OS kernel代码，使其具有类似Linux应用工具strace的功能，即能够显示出应用程序发出的系统调用，从而可以分析ucore应用的系统调用执行过程。
将lab8_result/kern/syscal/syscall.c修改如下：

添加函数printtrack用来打印系统函数的名称信息：
	

	void 
	printtrack(int num){
	    switch(num){
	        case SYS_exit :
	            cprintf("syscall: exit\n");
	            break;
	        case SYS_fork :
	            cprintf("syscall: fork\n");
	            break;
	        case SYS_wait :
	            cprintf("syscall: wait\n");
	            break;
	        case SYS_exec :
	            cprintf("syscall: exec\n");
	            break;
	        case SYS_clone :
	            cprintf("syscall: clone\n");
	            break;
	        case SYS_yield :
	            cprintf("syscall: yield\n");
	            break;
	        case SYS_sleep :
	            cprintf("syscall: sleep\n");
	            break;
	        case SYS_kill :
	            cprintf("syscall: kill\n");
	            break;
	        case SYS_gettime :
	            cprintf("syscall: gettime\n");
	            break;
	        case SYS_getpid :
	            cprintf("syscall: getpid\n");
	            break;
	        case SYS_mmap :
	            cprintf("syscall: mmap\n");
	            break;
	        case SYS_munmap :
	            cprintf("syscall: munmap\n");
	            break;
	        case SYS_shmem :
	            cprintf("syscall: shmem\n");
	            break;
	        case SYS_putc :
	            cprintf("syscall: putc\n");
	            break;
	        case SYS_pgdir :
	            cprintf("syscall: pgdir\n");
	            break;
	        case SYS_open :
	            cprintf("syscall: open\n");
	            break;
	        case SYS_close :
	            cprintf("syscall: close\n");
	            break;
	        case SYS_read :
	            cprintf("syscall: read\n");
	            break;
	        case SYS_write :
	            cprintf("syscall: write\n");
	            break;
	        case SYS_seek :
	            cprintf("syscall: seek\n");
	            break;
	        case SYS_fstat :
	            cprintf("syscall: fstat\n");
	            break;
	        case SYS_fsync :
	            cprintf("syscall: fsync\n");
	            break;
	        case SYS_getcwd :
	            cprintf("syscall: getcwd\n");
	            break;
	        case SYS_getdirentry :
	            cprintf("syscall: getdirentry\n");
	            break;
	        case SYS_dup :
	            cprintf("syscall: dup\n");
	            break;
	        case SYS_lab6_set_priority :
	            cprintf("syscall: lab6_set_priority\n");
	            break;
	        default:
	            cprintf("unknow syscall\n");
	    }
	}
在syscall中，在判断if (syscalls[num] != NULL) 之后，添加语句

	            printtrack(num);
调用函数。执行 make qemu，可以得到系统调用的输出结果：

	syscall: exec
	syscall: open
	syscall: open
	syscall: write
	usyscall: write
	ssyscall: write
	esyscall: write
	rsyscall: write
	 syscall: write
	ssyscall: write
	hsyscall: write
	 syscall: write
	isyscall: write
	ssyscall: write
	 syscall: write
	rsyscall: write
	usyscall: write
	nsyscall: write
	nsyscall: write
	isyscall: write
	nsyscall: write
	gIter 1, No.0 philosopher_sema is eating
	Iter 1, No.2 philosopher_sema is eating
	phi_test_condvar: state_condvar[0] will eating
	phi_test_condvar: signal self_cv[0] 
	cond_signal begin: cvp c03569e0, cvp->count 0, cvp->owner->next_count 0
	cond_signal end: cvp c03569e0, cvp->count 0, cvp->owner->next_count 0
	Iter 1, No.0 philosopher_condvar is eating
	phi_take_forks_condvar: 1 didn't get fork and will wait
	cond_wait begin:  cvp c03569f4, cvp->count 0, cvp->owner->next_count 0
	phi_test_condvar: state_condvar[2] will eating
	phi_test_condvar: signal self_cv[2] 
	cond_signal begin: cvp c0356a08, cvp->count 0, cvp->owner->next_count 0
	cond_signal end: cvp c0356a08, cvp->count 0, cvp->owner->next_count 0
	Iter 1, No.2 philosopher_condvar is eating
	phi_take_forks_condvar: 3 didn't get fork and will wait
	cond_wait begin:  cvp c0356a1c, cvp->count 0, cvp->owner->next_count 0
	phi_take_forks_condvar: 4 didn't get fork and will wait
	cond_wait begin:  cvp c0356a30, cvp->count 0, cvp->owner->next_count 0
	syscall: write
	!syscall: write
	!syscall: write
	!syscall: write
	$syscall: write
	 syscall: read
	phi_test_condvar: state_condvar[4] will eating

