/* Include the sbrk function */
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define NALLOC 1024                                     /* 最小分配单元 */

typedef long Align;                                     /* 内存对齐 */

union header {                                          /* 内存块前缀 */
  struct {
    union header *ptr;                                  
    unsigned size;                                      
  } s;
  Align x;                                              /* 强制对齐 */
};

typedef union header Header;

static Header base;                                     //分配起点
static Header *freep = NULL;                            

void free(void * ap)
{
  Header *bp, *p;

  if(ap == NULL) return;                                

  bp = (Header *) ap - 1;                               /* 指向前缀 */

  for(p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
    if(p >= p->s.ptr && (bp > p || bp < p->s.ptr))
      break;                                            /* 将p定位到 下一个/上一个 free的区域 */

  if(bp + bp->s.size == p->s.ptr) {                     /* 如果p是上一个紧挨着此区域（将要释放区域）的空闲区域
                                                         * 而且正好此块释放区域与下一块free区域相邻 
                                                         */
    bp->s.size += p->s.ptr->s.size;                     //那么将上一个free区域的指针指向的下一块free区域和当前区域合并
    bp->s.ptr = p->s.ptr->s.ptr;                        //并切用此块区域代替下一块区域
  }
  else
    bp->s.ptr = p->s.ptr;                               

  if(p + p->s.size == bp) {                             /* 如果上一块free区域和此块区域相邻，则合并这两块 */
    p->s.size += bp->s.size;
    p->s.ptr = bp->s.ptr;
  } else
    p->s.ptr = bp;                                        //如果不相邻，则只把bp加入到free链表中去。
  freep = p;
}

/* morecore: 分配更多内存 */

static Header *morecore(unsigned nu)
{
  void *cp;
  Header *up;

  if(nu < NALLOC)
    nu = NALLOC;

  cp = sbrk(nu*sizeof(Header));
  if(cp == (void *) -1){                                 /* no space at all */
    perror("failed to get more memory");
    return NULL;
  }
  up = (Header *) cp;
  up->s.size = nu;
  free((void *)(up+1));
  return freep;
}

void * returnblock(Header *ptr, Header *prevp, unsigned nunits){
    
    if (ptr->s.size == nunits)                          /* exactly */
            prevp->s.ptr = ptr->s.ptr;
        else {                                            /* allocate tail end */
            ptr->s.size -= nunits;
            ptr += ptr->s.size;
            ptr->s.size = nunits;
      }
      freep = prevp;
      return (void *)(ptr+1);
    
}

void * best_fit(size_t nunits){
    Header *p, *prevp, *smallest, *smallestprev;
    
    smallest = NULL;
    
    prevp = freep;  
    p = prevp->s.ptr;
    
    for(; ; prevp = p, p = p->s.ptr) {

        /* give the smallest element to the variable "smallest" */
        if( p->s.size >= nunits){
        
            if(NULL == smallest || p->s.size < smallest->s.size){
                smallest = p;
                smallestprev = prevp;
            }
            
        }
    
     /* wrapped around free list */
         if(p == freep){
            /* check if smallest is small enough, else give more core */    
            if(NULL != smallest)                            /* big enough */
                  return returnblock(smallest,smallestprev,nunits);     
             else 
                if((p = morecore(nunits)) == NULL)
                    return NULL; 
            
         }

    }
    
}

void * malloc(size_t nbytes)
{
  
   /*  */
  Header * morecore(unsigned);
  unsigned nunits;

  if(nbytes == 0) return NULL;

  nunits = (nbytes+sizeof(Header)-1)/sizeof(Header) +1;

  if(freep == NULL) {
    base.s.ptr = freep = &base;
    base.s.size = 0;
  }
  
  return best_fit(nunits);
}
