#ifndef MAIN_HEAD_H
#define MAIN_HEAD_H

/* FILE_INPUT is controlled by CMake via -DFILE_INPUT=ON/OFF */
#ifdef LIBPLI
#include "parser.h"
program_t* get_ast(char* program);
#endif

#endif /* MAIN_HEAD_H */