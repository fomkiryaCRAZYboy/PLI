#include "mem.h"
#include "errs.h"

#include <stdlib.h>
#include <string.h>
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

void* pli_realloc(void* old_ptr, size_t new_size, size_t old_size)
{
    if (!old_ptr)
        return pli_alloc(new_size);

    if(new_size == 0)
    {
        pli_free(old_ptr);
        return NULL ;
    }

    void* new_ptr = pli_alloc(new_size);
    if(!new_ptr)
        return NULL;

    size_t copy_size = (new_size > old_size) ? old_size : new_size;

    if(copy_size > 0)
        memcpy(new_ptr, old_ptr, copy_size);
    
    pli_free(old_ptr);

    return new_ptr;
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