-	cvp->count含义是什么？cvp->count是否可能<0, 是否可能>1？请举例或说明原因。

	条件变量的定义中也包含了一系列的成员变量，信号量sem用于让发出wait_cv操作的等待某个条件C为真的进程睡眠，而让发出signal_cv操作的进程通过这个sem来唤醒睡眠的进程。count表示等在这个条件变量上的睡眠进程的个数。
	可能大于1，因为可能有多个进程由于不满足某个条件变量的条件而等待，这时候他们会睡着。
	不会小于0，因为对count的操作是成对出现的，先加后减。
	
-	cvp->owner->next_count含义是什么？cvp->owner->next_count是否可能<0, 是否可能>1？请举例或说明原因。
		
	owner表示此条件变量的宿主是哪个管程。发出signal_cv的进程A会唤醒睡眠进程B，进程B执行会导致进程A睡眠，直到进程B离开管程，进程A才能继续执行，这个同步过程是通过信号量next完成的；而next_count表示了由于发出singal_cv而睡眠的进程个数。
	
	不会大于1。因为一旦进程A signal就会被wait的进程B接受，然后B进程最后会唤醒A，A于是继续执行，于是把next_count-1,所以是配对出现的。
	不会小于0，因为对next_count的操作是成对出现的，先加后减。

---
###我认为实现并没有bug
-	第一个问题：count next_count是否会<0？

		不会。首先我们看到，代码中加与减操作是配对进行的，也就是说，即使减法被打断，出现竞争现象，也只可能少减1，而非减到小于0。
		
-	第二个问题：为什么这里的条件变量会在wait的时候释放mutex？
	
		注意到代码中，在cond_wait一块里，有这样一个判断：
			if (cvp->owner->next_count > 0)
		        up(&(cvp->owner->next));
		    else 
		        up(&(cvp->owner->mutex));
		也就是说，如果没有因为signal睡着的进程在等待的话，就会把mutex给释放掉。这里有两个疑问：如果有睡着的进程，我们要把它唤醒，这里为什么没有mutex的释放？这样的话另一个程序不就没有这个锁了么？第二个疑问：释放了mutex之后，下面那个减法操作不就可能发生竞争么？
		
		
	我们来看调用这个cond_wait的地方：在例程中（check_sync.c），在哲学家拿叉子的函数：

	
		void phi_take_forks_condvar(int i) {
		  down(&(mtp->mutex));
		//--------into routine in monitor--------------
		  state_condvar[i] = HUNGRY;
		  phi_test_condvar(i);
		  while(state_condvar[i] != EATING){
		     cprintf("phi_take_forks_condvar: %d didn't get fork and will wait\n",i);
		     cond_wait(&mtp->cv[i]);
		  }
		//--------leave routine in monitor--------------
		   if(mtp->next_count>0)
		      up(&(mtp->next));
		   else
		      up(&(mtp->mutex));
		}


	这里，首先占有了mutex，确保自己进入了管程，然后进行拿叉子操作，如果失败了，就会执行cond_wait。因此，在cond_wait里，一开始mutex是锁住的。
	
			首先我们解答第二个疑问，这里一个哲学家对应一个condition，因此访问cvp->count--的只能是该condition variable对应的哲学家进程，因此即使被打断，也不会有其他的进程来同时操作这个变量（其他进程可能执行该信号量的signal，但其中不涉及对count的操作）。
		
			然后看第一个疑问：在up操作执行后，因为signal而等待的进程会被唤醒，然后执行cond_signal接下来的语句之后返回。而我们看到，对于cond_signal()，在执行这个函数过程中（直到返回），一直没有释放mutex的操作。也就是说，如果有进程能够执行cond_signal，则他一定是占有mutex的，而此时其他进程不可能进入到cond_wait(),因为在call cond_wait之前也是需要占有mutex的————所以在cond_wait中的第一个判断（cvp->owner->next_count>0）应该是永远不成立的。
		
			这时候也许会有新的疑问：如果cond_signal()一直不释放锁，那被他唤醒的程序怎么执行呢?我们主要到，在cond_signal（）中的语句up(&(cvp->sem));执行后，相应等待的程序被唤醒，然后执行down(&(cvp->sem))之后的语句，直到返回。这些语句没有down（mutex）的操作，因此它们不会被阻塞。当这个程序返回的时候，回到phi_take_forks_condvar中的	
		//--------leave routine in monitor--------------
		之后的语句。这里，如果这个进程是被唤醒的，那么就必然有因为用signal唤醒它而睡着的另一个进程，所以next_count>0，于是会唤醒那个进程，也就是占有锁的进程。这时候此进程醒来，继续执行，最终把锁释放。

	
	
---

-	目前的lab7-answer中管程的实现是Hansen管程类型还是Hoare管程类型？请在lab7-answer中实现另外一种类型的管程。
	
	实现的是Hoare管程类型