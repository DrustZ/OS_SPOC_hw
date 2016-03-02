#include <stdio.h>
#include <ulib.h>

int
main(void) {
    int pid = 0;
    if ((pid = fork()) == 0) {
        cprintf("I am child %d\n", n);
        exit(0);
    }

    asm volatile ("lgdt (%0)" :: "r" (0));
    return 0;
}

