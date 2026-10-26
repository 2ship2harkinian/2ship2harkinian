// This file is only useful for x86(_64) systems. There isn't a standard way to detect these.
// https://xkcd.com/927/ comes to mind...
#include "CpuHelpers.h"
#if defined (X86_CPU)

#if defined (__unix__)
#include <cpuid.h>
#elif defined (_WIN32)
// From the GCC cpuid.h header
/* Extended Features Leaf (%eax == 7, %ecx == 0) */
/* %ebx */
#define bit_AVX2	(1 << 5)
#include <intrin.h>
#endif

// Other checks can be added as needed.

int Cpu_SupportsAVX2(void) {
#ifdef _WIN32
    int cpuidData[4];
    __cpuid(cpuidData, 7);
#else
    unsigned int cpuidData[4];
    __get_cpuid(7, &cpuidData[0], &cpuidData[1], &cpuidData[2], &cpuidData[3]);
#endif
    if (cpuidData[1] & bit_AVX2) {
        return 1;
    }
    return 0;
}

#endif