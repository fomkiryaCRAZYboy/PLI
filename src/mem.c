#include "mem.h"
#include "errs.h"

#include <stdlib.h>
#include <stdio.h>

/* start of memory chain, it leads to NULL */
static MEMORY_BLOCK* memory_chain = NULL;

void* pli_alloc(size_t size) 
{
    MEMORY_BLOCK* block = malloc (sizeof(MEMORY_BLOCK));
    if (!block)
        return NULL ; 
    

    void* ptr = malloc (size);
    if (!ptr)
    {
        free (block);
        return NULL ;
    }

    block -> ptr  = ptr;
    block -> prev = memory_chain;
    memory_chain  = block;

    return ptr ;
}

void pli_free(void* ptr)
{
    if (!ptr) 
        return ;

    MEMORY_BLOCK** current = &memory_chain;  
    
    while (*current) 
    {
        if ((*current)->ptr == ptr) 
        {
            MEMORY_BLOCK* to_free = *current;
            free(to_free->ptr);
            *current = to_free->prev;
            free(to_free);

            return ;
        }

        current = &(*current)->prev;
    }
    
    add_err_code(PLI_FREE_func_UNTRACKED_POINTER, 0, true);
    return ; /* trying to free untracked pointer */
}

/* clearing all memory */
void emergency_cleanup() 
{
    while (memory_chain)
    {
        free (memory_chain -> ptr);

        MEMORY_BLOCK* tmp = memory_chain;
        memory_chain = memory_chain -> prev;
        free (tmp);
    }
}