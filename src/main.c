#include "lexer.h"
#include "mem.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main() {    
    atexit (print_errors);      /* 
                                    If there are no errors, 
                                    the output will be empty.     
                                */
    atexit (emergeny_cleanup);  /* 
                                    If the memory is not occupied, 
                                    nothing will happen 
                                */

    char* a = pli_alloc(10);
    char* b = pli_alloc(10); 
    char* c = pli_alloc(10);
    
    strcpy(a, "A");
    strcpy(b, "B");
    strcpy(c, "C");
    
    exit(EXIT_SUCCESS);
}