#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <stdio.h>

#define DEBUG

#ifdef DEBUG
#define pr_debug(M, ...) fprintf(stdout, M , ##__VA_ARGS__)
#else
#define pr_debug(M, ...)
#endif

#endif