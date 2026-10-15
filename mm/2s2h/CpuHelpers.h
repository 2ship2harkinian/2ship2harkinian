#ifndef CPU_HELPERS_H
#define CPU_HELPERS_H

// This file is only useful for x86(_64) systems right now. There isn't a standard way to detect these.
// https://xkcd.com/927/ comes to mind...


#if defined (__x86_64__) || defined(_M_X64) || defined(i386) || defined(__i386__) || defined(__i386) || defined(_M_IX86)
#define X86_CPU

#ifdef __cplusplus
extern "C" {
#endif

//Checks if the current CPU supports AVX2 instructions. This function always calls CPUID so its result should be cached
// by the caller.
int Cpu_SupportsAVX2(void);


#ifdef __cplusplus
}
#endif


#endif

#endif
