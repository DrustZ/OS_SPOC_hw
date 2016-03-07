#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define NALLOC 1024        // minimum #units to request
struct Header { struct Header *ptr; uint size; };

static Header base;                       // empty list to get started
static Header *freep = NULL;              // start of free list

struct {                      // block header
    union header *ptr;        // next block if on free list
    unsigned size;            // size of this block
} s;

void *_malloc(size_t nbytes) {
    Header *p, *prevp;
    unsigned nunits = (nbytes+sizeof(Header)-1)/sizeof(Header) + 1;

    if ((prevp = freep) == NULL) {
        base.s.ptr = freep = prevp = &base;
        base.s.size = 0;
    }

    for (p = prevp->s.ptr ;; prevp = p, p = p->s.ptr) {
        if (p->size >= nunits) {          // big enough
            if (p->s.size == nunits)
                prevp->s.ptr = p->s.ptr;
            else {                        // allocate tail end
                p->size -= nunits;
                p += p->s.size;
                p->s.size = nunits;
            }
            freep = prevp;
            return (void*)(p+1);
        }

        if (p == freep)                  // wrapped around free list
            if ((p = morecore(nunits)) == NULL)
                return NULL;
    }
}

static Header *morecore(unsigned nu){
    char *cp;
    Header *up;

    if (nu < NALLOC)
        nu = NALLOC;
    cp = sbrk(nu * sizeof(Header));
    if (cp == (char*)-1)   // check
        return NULL;
    up = (Header*) cp;
    up->s.size = nu;
    free((void*)(up+1));
    return freep;
}

void _free(void *ap)
{
    Header *bp, *p;
    bp = (Header *)ap - 1; /* point to block header */
    for (p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
        if (p >= p->s.ptr && (bp > p || bp < p->s.ptr))
            break; /* freed block at start or end of arena */

    if (bp + bp->size == p->s.ptr) { /* join to upper nbr */
        bp->s.size += p->s.ptr->s.size;
        bp->s.ptr = p->s.ptr->s.ptr;
    } else
        bp->s.ptr = p->s.ptr;
        if (p + p->size == bp) { /* join to lower nbr */
            p->s.size += bp->s.size;
            p->s.ptr = bp->s.ptr;
        } else
            p->s.ptr = bp;
        freep = p;
}

int main(){
    int *a = _malloc(sizeof(int));
    _free(a);
    printf("(haha!)\n");
    return 0;
}
