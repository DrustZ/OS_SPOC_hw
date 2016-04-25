（spoc)了解race condition. 进入[race-condition代码目录](https://github.com/chyyuu/ucore_lab/tree/master/related_info/lab7/race-condition)。

 - 执行 `./x86.py -p loop.s -t 1 -i 100 -R dx`， 请问`dx`的值是什么？
 
		由于指令中没有对dx进行赋值，因此dx一开始为0，循环一次之后dx=-1 < 0,最后终止
		
 - 执行 `./x86.py -p loop.s -t 2 -i 100 -a dx=3,dx=3 -R dx` ， 请问`dx`的值是什么？
 		
 		加入 -c 后可以看得结果，一开始dx赋值为3，之后开始循环直到dx<0，然后跳到第二个线程开始执行，同样，dx=3，因此最后dx = -1
 		打印如下： 		
 		   dx          Thread 0                Thread 1         
	    3   
	    2   1000 sub  $1,%dx
	    2   1001 test $0,%dx
	    2   1002 jgte .top
	    1   1000 sub  $1,%dx
	    1   1001 test $0,%dx
	    1   1002 jgte .top
	    0   1000 sub  $1,%dx
	    0   1001 test $0,%dx
	    0   1002 jgte .top
	   -1   1000 sub  $1,%dx
	   -1   1001 test $0,%dx
	   -1   1002 jgte .top
	   -1   1003 halt
	    3   ----- Halt;Switch -----  ----- Halt;Switch -----  
	    2                            1000 sub  $1,%dx
	    2                            1001 test $0,%dx
	    2                            1002 jgte .top
	    1                            1000 sub  $1,%dx
	    1                            1001 test $0,%dx
	    1                            1002 jgte .top
	    0                            1000 sub  $1,%dx
	    0                            1001 test $0,%dx
	    0                            1002 jgte .top
	   -1                            1000 sub  $1,%dx
	   -1                            1001 test $0,%dx
	   -1                            1002 jgte .top
	   -1                            1003 halt
 		
 - 执行 `./x86.py -p loop.s -t 2 -i 3 -r -a dx=3,dx=3 -R dx`， 请问`dx`的值是什么？
 		
 		由于这里使用了-r，因此中断是随机的，每次都不相同，其中一次输出结果为：
 		dx          Thread 0                Thread 1         
	    3   
	    2   1000 sub  $1,%dx
	    3   ------ Interrupt ------  ------ Interrupt ------  
	    2                            1000 sub  $1,%dx
	    2   ------ Interrupt ------  ------ Interrupt ------  
	    2   1001 test $0,%dx
	    2   1002 jgte .top
	    2   ------ Interrupt ------  ------ Interrupt ------  
	    2                            1001 test $0,%dx
	    2                            1002 jgte .top
	    2   ------ Interrupt ------  ------ Interrupt ------  
	    1   1000 sub  $1,%dx
	    1   1001 test $0,%dx
	    1   1002 jgte .top
	    2   ------ Interrupt ------  ------ Interrupt ------  
	    1                            1000 sub  $1,%dx
	    1                            1001 test $0,%dx
	    1   ------ Interrupt ------  ------ Interrupt ------  
	    0   1000 sub  $1,%dx
	    1   ------ Interrupt ------  ------ Interrupt ------  
	    1                            1002 jgte .top
	    0                            1000 sub  $1,%dx
	    0   ------ Interrupt ------  ------ Interrupt ------  
	    0   1001 test $0,%dx
	    0   ------ Interrupt ------  ------ Interrupt ------  
	    0                            1001 test $0,%dx
	    0                            1002 jgte .top
	    0   ------ Interrupt ------  ------ Interrupt ------  
	    0   1002 jgte .top
	   -1   1000 sub  $1,%dx
	   -1   1001 test $0,%dx
	    0   ------ Interrupt ------  ------ Interrupt ------  
	   -1                            1000 sub  $1,%dx
	   -1                            1001 test $0,%dx
	   -1   ------ Interrupt ------  ------ Interrupt ------  
	   -1   1002 jgte .top
	   -1   1003 halt
	   -1   ----- Halt;Switch -----  ----- Halt;Switch -----  
	   -1                            1002 jgte .top
	   -1   ------ Interrupt ------  ------ Interrupt ------  
	   -1                            1003 halt
	
	可以看到，上述输出产生了race-condition
 	
 - 变量x的内存地址为2000, `./x86.py -p looping-race-nolock.s -t 1 -M 2000`, 请问变量x的值是什么？
 		
 		由输出：
 		2000          Thread 0         
	    0   
	    0   1000 mov 2000, %ax
	    0   1001 add $1, %ax
	    1   1002 mov %ax, 2000
	    1   1003 sub  $1, %bx
	    1   1004 test $0, %bx
	    1   1005 jgt .top
	    1   1006 halt
 		变量x最后为1
 		
 - 变量x的内存地址为2000, `./x86.py -p looping-race-nolock.s -t 2 -a bx=3 -M 2000`, 请问变量x的值是什么？为何每个线程要循环3次？
 	
 		为何循环3次：因为一开始bx的赋值为3（bx=3）
 		加入-R bx 命令后可以清楚看到结果：
 		2000      bx          Thread 0                Thread 1         
	    0       3   
	    0       3   1000 mov 2000, %ax
	    0       3   1001 add $1, %ax
	    1       3   1002 mov %ax, 2000
	    1       2   1003 sub  $1, %bx
	    1       2   1004 test $0, %bx
	    1       2   1005 jgt .top
	    1       2   1000 mov 2000, %ax
	    1       2   1001 add $1, %ax
	    2       2   1002 mov %ax, 2000
	    2       1   1003 sub  $1, %bx
	    2       1   1004 test $0, %bx
	    2       1   1005 jgt .top
	    2       1   1000 mov 2000, %ax
	    2       1   1001 add $1, %ax
	    3       1   1002 mov %ax, 2000
	    3       0   1003 sub  $1, %bx
	    3       0   1004 test $0, %bx
	    3       0   1005 jgt .top
	    3       0   1006 halt
	    3       3   ----- Halt;Switch -----  ----- Halt;Switch -----  
	    3       3                            1000 mov 2000, %ax
	    3       3                            1001 add $1, %ax
	    4       3                            1002 mov %ax, 2000
	    4       2                            1003 sub  $1, %bx
	    4       2                            1004 test $0, %bx
	    4       2                            1005 jgt .top
	    4       2                            1000 mov 2000, %ax
	    4       2                            1001 add $1, %ax
	    5       2                            1002 mov %ax, 2000
	    5       1                            1003 sub  $1, %bx
	    5       1                            1004 test $0, %bx
	    5       1                            1005 jgt .top
	    5       1                            1000 mov 2000, %ax
	    5       1                            1001 add $1, %ax
	    6       1                            1002 mov %ax, 2000
	    6       0                            1003 sub  $1, %bx
	    6       0                            1004 test $0, %bx
	    6       0                            1005 jgt .top
	    6       0                            1006 halt

 		因此最后变量x为6
 		
 - 变量x的内存地址为2000, `./x86.py -p looping-race-nolock.s -t 2 -M 2000 -i 4 -r -s 0`， 请问变量x的值是什么？
 	
	 	由于开启了random模式，因此其中一次运行结果为：
	 	2000          Thread 0                Thread 1         
	    0   
	    0   1000 mov 2000, %ax
	    0   1001 add $1, %ax
	    1   1002 mov %ax, 2000
	    1   ------ Interrupt ------  ------ Interrupt ------  
	    1                            1000 mov 2000, %ax
	    1                            1001 add $1, %ax
	    2                            1002 mov %ax, 2000
	    2   ------ Interrupt ------  ------ Interrupt ------  
	    2   1003 sub  $1, %bx
	    2   1004 test $0, %bx
	    2   1005 jgt .top
	    2   1006 halt
	    2   ----- Halt;Switch -----  ----- Halt;Switch -----  
	    2   ------ Interrupt ------  ------ Interrupt ------  
	    2                            1003 sub  $1, %bx
	    2                            1004 test $0, %bx
	    2                            1005 jgt .top
	    2                            1006 halt
	    
		x为2,此时没有产生race-condition情况
 
 - 变量x的内存地址为2000, `./x86.py -p looping-race-nolock.s -t 2 -M 2000 -i 4 -r -s 1`， 请问变量x的值是什么？
 
 		这个指令只是修改了随机种子的值。。。我不知道和上一条本质上有什么区别。。。
 		运行其中一次的情况：
 		 2000      bx          Thread 0                Thread 1         
		    0       0   
		    0       0   1000 mov 2000, %ax
		    0       0   ------ Interrupt ------  ------ Interrupt ------  
		    0       0                            1000 mov 2000, %ax
		    0       0   ------ Interrupt ------  ------ Interrupt ------  
		    0       0   1001 add $1, %ax
		    1       0   1002 mov %ax, 2000
		    1      -1   1003 sub  $1, %bx
		    1       0   ------ Interrupt ------  ------ Interrupt ------  
		    1       0                            1001 add $1, %ax
		    1       0                            1002 mov %ax, 2000
		    1      -1   ------ Interrupt ------  ------ Interrupt ------  
		    1      -1   1004 test $0, %bx
		    1      -1   1005 jgt .top
		    1      -1   1006 halt
		    1       0   ----- Halt;Switch -----  ----- Halt;Switch -----  
		    1      -1                            1003 sub  $1, %bx
		    1      -1   ------ Interrupt ------  ------ Interrupt ------  
		    1      -1                            1004 test $0, %bx
		    1      -1                            1005 jgt .top
		    1      -1   ------ Interrupt ------  ------ Interrupt ------  
		    1      -1                            1006 halt

 		x为1，此时产生了race-condition
 		
 - 变量x的内存地址为2000, `./x86.py -p looping-race-nolock.s -t 2 -M 2000 -i 4 -r -s 2`， 请问变量x的值是什么？ 
 	
 		。。。为什么又是改了一次seed然后问。。。
 		其中一次运行情况：
 		2000      bx          Thread 0                Thread 1         
	    0       0   
	    0       0   1000 mov 2000, %ax
	    0       0   1001 add $1, %ax
	    1       0   1002 mov %ax, 2000
	    1       0   ------ Interrupt ------  ------ Interrupt ------  
	    1       0                            1000 mov 2000, %ax
	    1       0   ------ Interrupt ------  ------ Interrupt ------  
	    1      -1   1003 sub  $1, %bx
	    1      -1   1004 test $0, %bx
	    1      -1   1005 jgt .top
	    1       0   ------ Interrupt ------  ------ Interrupt ------  
	    1       0                            1001 add $1, %ax
	    2       0                            1002 mov %ax, 2000
	    2      -1   ------ Interrupt ------  ------ Interrupt ------  
	    2      -1   1006 halt
	    2       0   ----- Halt;Switch -----  ----- Halt;Switch -----  
	    2       0   ------ Interrupt ------  ------ Interrupt ------  
	    2      -1                            1003 sub  $1, %bx
	    2      -1                            1004 test $0, %bx
	    2      -1                            1005 jgt .top
	    2      -1                            1006 halt
	    x = 2
 
 - 变量x的内存地址为2000, `./x86.py -p looping-race-nolock.s -a bx=1 -t 2 -M 2000 -i 1`， 请问变量x的值是什么？ 
 
 		终于有个正常的问题了，这次是1次1打断，所以会产生race-condition，从结果来看，
 		x也只加了一次，为1
 		
		2000      bx          Thread 0                Thread 1         
	    0       1   
	    0       1   1000 mov 2000, %ax
	    0       1   ------ Interrupt ------  ------ Interrupt ------  
	    0       1                            1000 mov 2000, %ax
	    0       1   ------ Interrupt ------  ------ Interrupt ------  
	    0       1   1001 add $1, %ax
	    0       1   ------ Interrupt ------  ------ Interrupt ------  
	    0       1                            1001 add $1, %ax
	    0       1   ------ Interrupt ------  ------ Interrupt ------  
	    1       1   1002 mov %ax, 2000
	    1       1   ------ Interrupt ------  ------ Interrupt ------  
	    1       1                            1002 mov %ax, 2000
	    1       1   ------ Interrupt ------  ------ Interrupt ------  
	    1       0   1003 sub  $1, %bx
	    1       1   ------ Interrupt ------  ------ Interrupt ------  
	    1       0                            1003 sub  $1, %bx
	    1       0   ------ Interrupt ------  ------ Interrupt ------  
	    1       0   1004 test $0, %bx
	    1       0   ------ Interrupt ------  ------ Interrupt ------  
	    1       0                            1004 test $0, %bx
	    1       0   ------ Interrupt ------  ------ Interrupt ------  
	    1       0   1005 jgt .top
	    1       0   ------ Interrupt ------  ------ Interrupt ------  
	    1       0                            1005 jgt .top
	    1       0   ------ Interrupt ------  ------ Interrupt ------  
	    1       0   1006 halt
	    1       0   ----- Halt;Switch -----  ----- Halt;Switch -----  
	    1       0   ------ Interrupt ------  ------ Interrupt ------  
	    1       0                            1006 halt