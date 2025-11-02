#include "lexer.h"
#include "mem.h"

#include <stdio.h>
#include <string.h>


int main() {    
    char* a = pli_alloc(10);
    char* b = pli_alloc(10); 
    char* c = pli_alloc(10);
    
    strcpy(a, "A");
    strcpy(b, "B");
    strcpy(c, "C");

    emergeny_cleanup();
    
    return 0;
}