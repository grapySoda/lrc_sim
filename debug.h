#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <stdio.h>

// #define DEBUG
// #define DEBUG_SH
// #define DEBUG_RT

#ifdef DEBUG
#define pr_debug(M, ...) fprintf(stdout, "[log] " M , ##__VA_ARGS__)
#else
#define pr_debug(M, ...)
#endif

#ifdef DEBUG_SH
#define pr_debug_sh(M, ...) fprintf(stdout, "[log] " M , ##__VA_ARGS__)
#else
#define pr_debug_sh(M, ...)
#endif

#ifdef DEBUG_RT
#define pr_debug_rt(M, ...) fprintf(stdout, "[log] " M , ##__VA_ARGS__)
#else
#define pr_debug_rt(M, ...)
#endif

#endif