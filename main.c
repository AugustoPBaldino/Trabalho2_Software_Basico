#include <stdio.h>
#include "Memory.h"
#include "Memory.c"


int main()
{
    memory_t *mem = mymemory_init(1024);

    void *p1 = mymemory_alloc(mem,100)
    void *p2 = mymemory_alloc(mem,200)

    mymemory_display(mem)
    mymemory_stats(mem);

    mymemory_free(mem,p1)
    mymemory_display(mem)
    mymemory_stats(mem);

    mymemory_cleanup(mem)



}