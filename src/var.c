#include "var.h"

void var_hash_init()
{

}

bool var_hash_lookup(const char* varname)
{
    return 1;
}

int var_hash_function(const char* varname)
{
    int hash_code = 0;

    for (int i = 0; varname[i] != '\0'; ++i) 
    {
        hash_code += varname[i];
    }

    return hash_code /* % sizeof hashmap */ ;
}