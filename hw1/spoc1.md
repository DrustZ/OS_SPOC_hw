#FUNCALL部分
##尝试用简单的xem调试代码：
		
	运行./xem -g funcall 
	即可开启debugger模式，调试说明：
	h:  print help commands.
	q:  quit.
    c:  continue.
    s:  single step for one instruction.
    i:  display registers.
    x:  display memory, the input address is hex number (e.g x 10000)

##1.funcall中的堆栈有多大？是内核态堆栈还是用户态堆栈?
		
	128MB（MEM_SZ）-4MB（FS_SZ）-数据和程序  
	从debug信息可以看出是内核态堆栈

##2.funcall中的全局变量ret放在内存中何处？如何对它寻址？

		ret存在内存地址为0x00000088处
		从汇编代码的SG对全局变量赋值的语句可以判断出来,
		00000084  00000045  SG    0x0 (D 0) 

##3.funcall中的字符串放在内存中何处？如何对它寻址？

	存在内存地址为0xFFFFFFFF处
    从汇编代码的LXC语句中可以看出:
    00000034  ffffff1f  LXC   0xffffffff (D -1) //字符串中对应内容字符存入a

##4.局部变量i在内存中的何处？如何对它寻址？

	SP+0x18（当前sp为0x07bfffd8）
    从LL和SL:
    root/usr/funcall.c  14:   i=n;
    0000001c  0000200e  LL    0x20 (D 32) 
    00000020  00000440  SL    0x4 (D 4) //把n赋值给i
	对i的操作语句中可以推出i地址的表达式（sp+4)，再利用调试找到sp的值


##5.当前系统是处于中断使能状态吗？

	不是,从debugger看出

##6.funcall中的函数参数是如何传递的？函数返回值是如何传递的？

	函数的参数通过栈来传递。调用函数前先将参数压入栈中，在执行函数的时候直接从栈中找到参数。
	函数返回值是通过寄存器a来传递的。

##7.分析并说明funcall执行文件的格式和内容？

	执行文件为二进制文件
	通过分析em.c,然后用
	xem -v funcall 运行verbose模式，可以输出hdr数据中的值
	找到bbs为8，所以从0800开始为数据段
	找到entry为108，所以从109位往后0108开始为程序段
	
	
===
	
#OS0部分

##1.何处设置的中断使能？

	在asm(STI);一句，此时将iena设置为1
		

##系统何时处于中断屏蔽状态？

	在asm(STI);之前，也就是iena为0的时候
	以及在em.c处理interrupt代码段的时候（也就是处理中断）

##如果系统处于中断屏蔽状态，如何让其中断使能？
	
	1.执行STI操作（如上问）
	2.在em.c文件中，分析代码，在case RTI:中，如果处于内核态且iena为0，ipend=0的时候，可以让其中断使能
	
	
##系统产生中断后，CPU会做哪些事情？（在没有软件帮助的情况下）

	在em.c中，
		xpc = (int *)(ivec + tpc);
    	goto fixpc;
    可看到，cpu将会把xpc的值（栈顶指针）修改为偏移量为ivec的地址，在没有分页的情况下，就是ivec的值
    而之前由
    	    if (user) { usp = xsp; xsp = ssp; user = 0; tr = trk; tw = twk; trap |= USER; }
	可以看出，如果是用户态，则先切换到内核态再进行之后操作
    
##CPU执行RTI指令的具体完成工作是哪些？
	
	return from interrupt, set pc, sp, may switch user/kernel mode;
	if has pending interrupt, process the interrupt
	
	
===
#OS1 OS3部分

##os1中的task1和task2的堆栈的起始和终止地址是什么？
	
	task1的起始地址为全局变量task1_stack的地址+48
	
##os1是如何实现任务切换的？

	首先打开中断使能，执行task0，然后在每次timeout时会interrupt并且执行alltraps，
	在alltraps中的trap中交换两个指针的值，具体做法在swtch：
	也就是两种情况：
		1.将task0_sp的内容改为sp的内容（下一条指令），然后sp内容改为task1_sp的内容
		2.将task1_sp的内容改为sp的内容（下一条指令），然后sp内容改为task0_sp的内容
		
	于是顺序执行的时候，首先task0被timeout，触发中断，接着进行第一次swth交换：
		首先task0_sp变为sp，此时sp内容为
		
		task0()
		{
		  while(current < 10) {
		    write(1, "0", 1);
		  }
		  write(1,"task0 exit\n", 11);
		  halt(0);
		}
		中的 write(1, "0", 1); 语句（因为current此时<10，所以一直执行该语句）
		因此在交换后，sp指向了task1函数，也就开始输出1.直到下一次timeout中断，如此往复。
		
		
##os3中的task1和task2的堆栈的起始和终止地址是什么？

	
##os3是如何实现任务切换的？

	
##os3的用户态task能够破坏内核态的系统吗？

	可以
	由trap函数的case判断语句可知道，如果syscall的类型不是S_write，则会造成panic；
	或者中断不属于FSYS + USER/FTIMER/FTIMER + USER 也会造成panic
	因此，我如果添加函数：
	read() {asm(TRAP,S_read);}
	并且调用，例如在task0中调用：
	task0()
		{
		  while(current < 10)
		    read();
		    // write(1, "00", 2);
		
		  write(1,"task0 exit\n", 11);
		  halt(0);
		}
	则会造成panic unknown syscall 的error，并且直接终止程序。		
