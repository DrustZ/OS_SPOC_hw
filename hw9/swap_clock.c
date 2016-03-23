#include <defs.h>
#include <x86.h>
#include <stdio.h>
#include <string.h>
#include <swap.h>
#include <swap_clock.h>
#include <list.h>

list_entry_t pra_list_head;
list_entry_t *pra_list_current = NULL;
/* 我们使用Page 来作为clock的每一个节点，page结构体中，用对应的pte作为访问标记*/
static int
_clock_init_mm(struct mm_struct *mm)
{     
     list_init(&pra_list_head);
     pra_list_current = &pra_list_head;
     mm->sm_priv = &pra_list_head;
     //cprintf(" mm->sm_priv %x in fifo_init_mm\n",mm->sm_priv);
     return 0;
}

/*添加的时候只需要在页的后面进行添加*/
static int
_clock_map_swappable(struct mm_struct *mm, uintptr_t addr, struct Page *page, int swap_in)
{
    // list_entry_t *head=(list_entry_t*) mm->sm_priv;
    list_entry_t *entry=&(page->pra_page_link);
 
    assert(entry != NULL && head != NULL);
    //record the page access situlation
    /*LAB3 EXERCISE 2: YOUR CODE*/ 
    list_add(pra_list_current, entry);
    return 0;
}


/*换出的时候，需要遍历列表，一直找到visit为false的page*/
static int
_clock_swap_out_victim(struct mm_struct *mm, struct Page ** ptr_page, int in_tick)
{
     assert(in_tick==0);
     /* Select the victim */
     
     pra_list_current = pra_list_current->next;
     struct Page *pg = le2page(pra_list_current, pra_page_link);
     while(1){
        pte_t *pte = get_pte(mm->pgdir, p->pra_vaddr, 0);
        if ((*pte) & PTE_A){
            (*pte) &= (~PTE_A);
            pra_list_current = pra_list_current->next;
            if (pra_list_current == &pra_list_head) pra_list_current = pra_list_head.next;
            pg = le2page(pra_list_current->next, pra_page_link);
        } else break;
     }

     list_entry_t * curr = pra_list_current->next;
     assert(pra_list_current!=pra_list_head);
     struct Page *p = le2page(pra_list_current, pra_page_link);
     list_del(pra_list_current);
     pra_list_current = curr;
     assert(p !=NULL);
     *ptr_page = p;
     return 0;
}

static int
_clock_check_swap(void) {
    // cprintf("write Virt Page c in fifo_check_swap\n");
    // *(unsigned char *)0x3000 = 0x0c;
    // assert(pgfault_num==4);
    // cprintf("write Virt Page a in fifo_check_swap\n");
    // *(unsigned char *)0x1000 = 0x0a;
    // assert(pgfault_num==4);
    // cprintf("write Virt Page d in fifo_check_swap\n");
    // *(unsigned char *)0x4000 = 0x0d;
    // assert(pgfault_num==4);
    // cprintf("write Virt Page b in fifo_check_swap\n");
    // *(unsigned char *)0x2000 = 0x0b;
    // assert(pgfault_num==4);
    // cprintf("write Virt Page e in fifo_check_swap\n");
    // *(unsigned char *)0x5000 = 0x0e;
    // assert(pgfault_num==5);
    // cprintf("write Virt Page b in fifo_check_swap\n");
    // *(unsigned char *)0x2000 = 0x0b;
    // assert(pgfault_num==5);
    // cprintf("write Virt Page a in fifo_check_swap\n");
    // *(unsigned char *)0x1000 = 0x0a;
    // assert(pgfault_num==6);
    // cprintf("write Virt Page b in fifo_check_swap\n");
    // *(unsigned char *)0x2000 = 0x0b;
    // assert(pgfault_num==7);
    // cprintf("write Virt Page c in fifo_check_swap\n");
    // *(unsigned char *)0x3000 = 0x0c;
    // assert(pgfault_num==8);
    // cprintf("write Virt Page d in fifo_check_swap\n");
    // *(unsigned char *)0x4000 = 0x0d;
    // assert(pgfault_num==9);
    // cprintf("write Virt Page e in fifo_check_swap\n");
    // *(unsigned char *)0x5000 = 0x0e;
    // assert(pgfault_num==10);
    // cprintf("write Virt Page a in fifo_check_swap\n");
    // assert(*(unsigned char *)0x1000 == 0x0a);
    // *(unsigned char *)0x1000 = 0x0a;
    // assert(pgfault_num==11);
    return 0;
}

static int
_clock_init(void)
{
    return 0;
}

static int
_clock_set_unswappable(struct mm_struct *mm, uintptr_t addr)
{
    return 0;
}

static int
_clock_tick_event(struct mm_struct *mm)
{ return 0; }


struct swap_manager swap_manager_clock =
{
     .name            = "_clock_init swap manager",
     .init            = &_clock_init,
     .init_mm         = &_clock_init_mm,
     .tick_event      = &_clock_tick_event,
     .map_swappable   = &_clock_map_swappable,
     .set_unswappable = &_clock_set_unswappable,
     .swap_out_victim = &_clock_swap_out_victim,
     .check_swap      = &_clock_check_swap,
};
