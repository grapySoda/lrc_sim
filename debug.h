#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <stdio.h>

// #define DEBUG
// #define DEBUG_SH
#define DEBUG_RT
#define DEBUG_Z
#define REVERSED

#ifdef BUG
#define bug(M, ...) fprintf(stdout, "[log] " M , ##__VA_ARGS__)
#else
#define bug(M, ...)
#endif

#ifdef DEBUG
#define pr_debug(M, ...) fprintf(stdout, "[log] " M , ##__VA_ARGS__)
#else
#define pr_debug(M, ...)
#endif

#ifdef DEBUG_SH
#define pr_debug_sh(M, ...) fprintf(stdout, M , ##__VA_ARGS__)
#else
#define pr_debug_sh(M, ...)
#endif

#ifdef DEBUG_RT
#define pr_debug_rt(M, ...) fprintf(stdout, "[log] " M , ##__VA_ARGS__)
#else
#define pr_debug_rt(M, ...)
#endif

#ifdef DEBUG_Z
#define pr_debug_z(M, ...) fprintf(stdout, M , ##__VA_ARGS__)
#else
#define pr_debug_z(M, ...)
#endif

#endif