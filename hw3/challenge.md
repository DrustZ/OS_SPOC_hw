###写出一些简单的小程序（c or asm）来体现这些特权级的区别和联系

我们的代码意思为，在用户态执行指令的时候，如果调用系统调用（在代码中为fork），则可以顺利执行，并且会有输出（I'm child）

而在父进程中之后会执行一个特权指令（在用户态下由于MAX(CPL,RPL)<DPL不能执行）,在代码中为LGDT（load GDT），会被拒绝，报错之后退出。如是体现了特权级的作用。

###写出一些简单的小程序（c or asm）来显示出不同特权级的的中断切换的堆栈变化情况
在ucore中的kern/init.c中的
lab1_switch_test()中修改代码，在每次切换状态后调用print_stackframe()，就可以打印出堆栈信息以及寄存器值，输出结果如下：

```
	0: @ring 0
0:  cs = 8
0:  ds = 10
0:  es = 10
0:  ss = 10
+++ switch to  user  mode +++
ebp:0x00007b98 eip:0x001009c0 args:0x001035e8 0x00000010 0x00000020 0x00000000 
    kern/debug/kdebug.c:305: print_stackframe+21
ebp:0x00007bb8 eip:0x00100201 args:0x0010359c 0x00103580 0x0000136a 0x00000000 
    kern/init/init.c:112: lab1_switch_test+32
ebp:0x00007be8 eip:0x00100073 args:0x00000000 0x00000000 0x00000000 0x00007c4f 
    kern/init/init.c:40: kern_init+114
ebp:0x00007bf8 eip:0x00007d64 args:0xc031fcfa 0xc08ed88e 0x64e4d08e 0xfa7502a8 
    <unknow>: -- 0x00007d63 --
1: @ring 3
1:  cs = 1b
1:  ds = 23
1:  es = 23
1:  ss = 23
+++ switch to kernel mode +++
ebp:0x00007b98 eip:0x001009c0 args:0x00103608 0x00000010 0x00000020 0x00000000 
    kern/debug/kdebug.c:305: print_stackframe+21
ebp:0x00007bb8 eip:0x0010021c args:0x0010359c 0x00103580 0x0000136a 0x00000000 
    kern/init/init.c:116: lab1_switch_test+59
ebp:0x00007be8 eip:0x00100073 args:0x00000000 0x00000000 0x00000000 0x00007c4f 
    kern/init/init.c:40: kern_init+114
ebp:0x00007bf8 eip:0x00007d64 args:0xc031fcfa 0xc08ed88e 0x64e4d08e 0xfa7502a8 
    <unknow>: -- 0x00007d63 --
2: @ring 0
2:  cs = 8
2:  ds = 10
2:  es = 10
2:  ss = 10

```