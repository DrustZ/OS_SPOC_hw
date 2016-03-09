##什么是recursive mapping？

-	recursive page directory 是指 ， 因为在寻址过程中需要动态变换PDT和PT的内容，于是为了简便查找page directory\page table的地址，让page directory的最后一个entry设置为指向page directory本身的地址。这样如果我们访问了最后一个entry，就可以循环指向page directory的基址，这次把page directory看成是一个page table，然后就可以根据offset查找物理页的基地址。

-	如果最后的offset为0，则可以找到page table的基地址

-	如果page table的offset为0，则相当于访问了两次page directory（第一次把它当做page table，第二次把它当做物理页page），然后就可以获得page directory的地址。

-	这样的话，就可以通过虚拟地址来取得每个page table/directory的物理地址，从而修改其中的内容，达到快捷转变物理地址映射的效果。

参考资源：

<http://www.rohitab.com/discuss/topic/31139-tutorial-paging-memory-mapping-with-a-recursive-page-directory/>

<http://www.thomasloven.com/blog/2012/06/Recursive-Page-Directory/>