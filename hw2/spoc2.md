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



		