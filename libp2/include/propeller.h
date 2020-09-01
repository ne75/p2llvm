/**
 * @file include/propeller.h
 * @brief Provides Propeller specific functions.
 *
 * Copyright (c) 2011-2013 by Parallax, Inc.
 * Copyright (c) 2019 Total Spectrum Software Inc.
 *
 * copied from spin2cpp and modified for p2llvm
 *
 * MIT Licensed
 */
#ifndef _PROPELLER_H
#define _PROPELLER_H

#pragma once

#ifdef __propeller2__
#define __PROPELLER2__
#endif

#ifdef __PROPELLER2__
#define _PROP1_COMPATIBLE
#include <propeller2.h>
#define getcnt() _cnt()
#define waitcnt(x) _waitcnt(x)
#define CNT _cnt()
#else
#include <propeller1.h>
#endif

#define CLKFREQ _clkfreq
#define clkset(a, b) _clkset(a, b)


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Start a new propeller LMM function/thread in another COG.
 *
 * @details This function starts a new LMM VM kernel in a new COG with
 * func as the start function. The stack size must be big enough to hold
 * the struct _thread_state_t, the initial stack frame, and other stack
 * frames used by called functions.
 *
 * @details This function can be used instead of _start_cog_thread.
 *
 * @param func LMM start function
 * @param par Value of par parameter usually an address
 * @param stack Address of user defined stack space.
 * @param stacksize Size of user defined stack space.
 * @returns COG ID allocated by the function or -1 on failure.
 */
#ifdef __FLEXC__
#define cogstart(func, par, stack, stacksize) __builtin_cogstart(func(par), stack)
#else
int cogstart(void (*func)(void *), int par, int *stack, unsigned int stacksize);
#endif

#ifdef __cplusplus
}
#endif

#endif