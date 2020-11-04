#include <stdio.h>

//
//      CPUID Decode
//

#ifndef _TYPES_H
#define _TYPES_H

typedef unsigned                u32;
typedef int                     i32;
typedef long long unsigned      u64;
typedef long long int           i64;
#endif


/*
 *      CPUID Documentation
 *
 *  The CPUID instruction takes no parameters as CPUID implicitly uses the EAX register.
 *
 *  EAX=0: Get vendor ID
 *      This returns the CPU's manufacturer ID string
 *  EAX=1: Processor Info and Feature Bits
 *      3:0 - Stepping
 *      7:4 - Model
 *      11:8 - Family
 *      13:12 - Processor Type
 *      19:16 - Extended Model
 *      27:20 - Extended Family
 *
 *      Bit     EDX                                 ECX
 *      0       fpu Onboard x87 FPU                 sse3   Prescott New Instructions-SSE3 (PNI)
 *      1       vme Virtual 8086 mode extension     pclmulqdq PCLMULQDQ support
 *      2       de Debugging extensions             dtes64   64-bit debug store (edx bit 21)
 *      3       pse Page Size Extension             MONITOR and MWAIT instructions (SSE3)
 *      4       tsc Time Stamp Counter              ds-cpl   CPL qualified debug store
 *      5       msr Model-specific registers        vmx   Virtual Machine eXtensions
 *      6       pae Physical Address Extension      smx   Safer Mode Extensions (LaGrande)
 *      7       mce Machine Check Exception         est   Enhanced SpeedStep
 *      8       cx8 CMPXCHG8 (compare-and-swap)     tm2   Thermal Monitor 2
 *      9       apic Onboard APIC                   ssse3   Supplemental SSE3 instructions
 *      11      sep SYSENTER and SYSEXIT            sdbg   Silicon Debug interface
 *      12      mtrr Memory Type Range Registers    fma   Fused multiply-add (FMA3)
 *      13      pge Page Global Enable bit in CR4   cx16   CMPXCHG16B instruction
 *      14      mca Machine check architecture      xtpr   Can disable sending task priority messages
 *      15      cmov Conditional move and FCMOV     pdcm   Perfmon & debug capability
 *      16      pat  Page Attribute Table 
 *      17      pse-36  36-bit page size extension  pcid   Process context identifiers (CR4 bit 17)
 *      18      psn                                 dca   Direct cache access for DMA writes[12][13]
 *      19      clfsh                               sse4.1
 *      21      ds                                  x2apic
 *      22      acpi thermal MSRs for ACPI          movbe (big-endian)
 *      23      mmx                                 popcnt 
 *      24      fxsr                                tsc-deadline
 *      25      sse (Katmai) Instructions           aes
 *      26      sse2                                xsave
 *      27      ss CPU cache supports self-snoop    osxsave XSAVE enabled by OS
 *      28      htt Hyper-threading                 avx 
 *      29      tm Thermal monitor limits temp      f16c half-precision FP support
 *      30      ia64 processor emulating x86        rdrnd 
 *      31      pbe                                 hypervisor Running on a hypervisor
 *
 *  EAX=2: Cache and TLB Descriptor information
 *    This returns a list of cache and TLB capabilities in EAX, EBX, ECX and EDX registers.
 *
 *  EAX=3: Processor Serial Number
 *
 *  EAX=4 and EAX=Bh: Intel thread/core and cache topology
 *
 *  EAX=7, ECX=0: Extended Features
 *    Bit     EBX                                     ECX
 *    0       fsgsbase                                prefetchwt1   PREFETCHWT1 instruction
 *    1       IA32_TSC_ADJUST                         avx512vbmi
 *    2       sgx Software Guard Extensions           umip   User-mode Instruction Prevention
 *    3       bmi1  Bit Manipulation Set 1            pku   Memory Protection Keys for User-mode pages
 *    4       hle Transactional Synchronization       ospke   PKU enabled by OS
 *    5       avx2 Advanced Vector Extensions 2 
 *    7       smep Supervisor-Mode Execution Prevent
 *    8       bmi2 Bit Manipulation                   gfni  Galois Field instructions
 *    9       erms Enhanced REP MOVSB/STOSB           vaes  Vector AES instruction set (VEX-256/EVEX)
 *    10      invpcid                                 vpclmulqdq 
 *    11      rtm  Transactional Synchronization      avx512vnni 
 *    12      pqm  Platform Service Monitoring        avx512bitalg   AVX-512 BITALG instructions
 *    14      mpx  Intel MPX                          avx512vpopcntdq  
 *    15      pqe  Platform QOS
 *    16      avx512f 
 *    17      avx512dq 
 *    18      rdseed RDSEED instruction
 *    19      adx 
 *    20      smap Supervisor Mode Access Prevention
 *    21      avx512ifma   
 *    22      pcommit                                 rdpid   Read Processor ID
 *    23      clflushopt 
 *    24      clwb 
 *    25      intel_pt Processor Trace 
 *    26      avx512pf  Prefetch Instructions 
 *    27      avx512er  Exponential and Reciprocal
 *    28      avx512cd  Conflict Detection 
 *    29      sha 
 *    30      avx512bw 
 *    31      avx512vl
 *
 *    Bit     EDX
 *    2       avx512_4vnniw   AVX-512 4-register Neural Network Instructions
 *    3       avx512_4fmaps   AVX-512 4-register Multiply Accumulation Single precision
 *    18      pconfig   Memory Encryption Technologies Instructions
 *    26      spec_ctrl
 *    29     IA32_ARCH_CAPABILITIES
 *    31     Speculative Store Bypass Disable (SSBD)
 *
 *  EAX=80000000h: Get Highest Extended Function Supported
 *    The highest calling parameter is returned in EAX.
 *
 *   EAX=2: Cache and TLB Descriptor information
 *     Returns descriptors indicating cache and TLB capabilities in EAX, EBX, ECX and EDX 
 *
 *  EAX=3: Processor Serial Number
 *  EAX=80000000h: Get Highest Extended Function Supported
 *     The highest calling parameter is returned in EAX.
 *
 *  EAX=80000001h: Extended Processor Info and Feature Bits
 *     This returns extended feature flags in EDX and ECX.
 *
 *  EAX=80000002h,80000003h,80000004h: Processor Brand String
 *  EAX=80000005h: L1 Cache and TLB Identifiers
 *      This function contains the processor’s L1 cache and TLB characteristics.
 *
 *  EAX=80000006h: Extended L2 Cache Features
 *  EAX=80000007h: Advanced Power Management Information
 *     This function provides advanced power management feature identifiers.
 *
 *  EAX=80000008h: Virtual and Physical address Sizes
 *      Returns largest virtual and physical address sizes in EAX.
 */

#define MAXCALL 32
char* rgCallName[MAXCALL] = {
    (char*) "Vendor ID",        // 0
    (char*) "Capabilities",     // 1
    (char*) "Cache and TLB",    // 2
    (char*) "PSN",              // 3
    (char*) "Deterministic cache parameters",  // 4
    (char*) "Monitor/Mwait",    // 5
    (char*) "Digital thermal temperature",     // 6
    (char*) "Extended features",// 7
    (char*) "reserved",         // 8
    (char*) "Direct cache enable",  // 9
    (char*) "Performance monitor",  // 10
    (char*) "x2APIC",           // 11
    (char*) "reserved",         // 12
    (char*) "xsave",            // 13
    (char*) "Unknown",          // 14
    (char*) "Unknown",          // 15
    (char*) "Architectural performance", // 16
    (char*) "x2APIC",           // 17
    (char*) "Unknown",          // 18
    (char*) "Xsave",            // 19
    (char*) "new",              // 20
    (char*) "new",              // 21
    (char*) "new",              // 22
    (char*) "new",              // 23
    (char*) "new",              // 24
    (char*) "new",              // 25
    (char*) "new",              // 26
    (char*) "new",              // 27
    (char*) "new",              // 28
    (char*) "new",              // 29
    (char*) "new",              // 30
    (char*) "new",              // 31
};
        

struct cpuCapability {
    int     offset;
    char*   feature;
};

struct cpuCapability EDXCapability[32]= {
    0, (char*)"fpu Onboard",
    1, (char*)"vme Virtual mode extensions",
    2, (char*)"de  Debugging extensions",
    3, (char*)"pse Page size extensions",
    4, (char*)"tsc Time Stamp Counter",
    5, (char*)"msr Model-specific registers",
    6, (char*)"pae Physical Address Extension",
    7, (char*)"mce Machine Check Exception  ",
    8, (char*)"cx8 CMPXCGH8 instruction",
    9, (char*)"apic on-chip APIC",
    -10, (char*) "(reserved)",
    11, (char*)"SYSENTER and SYSEXIT instructions",
    12, (char*)"mtrr memory-type range registers",
    13, (char*)"pge TLB not flushed on task transition",
    14, (char*)"mca Machine check",
    15, (char*)"cmov Conditional move",
    16, (char*)"pat Page Attribute Table",
    17, (char*)"pse36 36-bit page huge pages ",
    18, (char*)"pn Processor Serial Number dca",
    19, (char*)"clflush CLFLUSH instruction",
    -20, (char*)"(reserved)",
    21, (char*)"dts save trace of jumps",
    22, (char*)"acp power managementi",
    23, (char*)"mmx MMX instructions",
    24, (char*)"fxsr",
    25, (char*)"sse SSE instructions",
    26, (char*)"sse2 SSE2 instructions ",
    27, (char*)"ss CPU cache supports self-snoop",
    28, (char*)"ht Hyper-threading ",
    29, (char*)"tm Thermal limits temperature ",
    30, (char*)"ia64 processor emulating x86",
    31, (char*)"pbe Pending Break Enable",
};

struct cpuCapability ECXCapability[32]= {
    0, (char*)"pni Prescott SSE",
    1, (char*)"pclmulqdq PCLMULQDQ support",
    2, (char*)"64-bit debug store (edx bit 21)",
    3, (char*)"MONITOR and MWAIT instructions (SSE3)",
    4, (char*)"CPL qualified debug store",
    5, (char*)"vmx Virtual Machine eXtensions",
    6, (char*)"smx Safer Mode Extensions (LaGrande)",
    7, (char*)"est Enhanced SpeedStep",
    8, (char*)"tm2 Thermal Monitor 2",
    9, (char*)"ssse3 Suplemental SSE3 instructions",
    10, (char*)"cid Context ID",
    -11, (char*)"(reserved)",
    12, (char*)"Fused multiply-add (FMA3)",
    13, (char*)"cx16 CMPXCHG16B instruction",
    14, (char*)"xtpr Can disable sending task priority messages",
    15, (char*)"pdcm Perfmon & debug capability",
    -16, (char*)"(reserved)",
    17, (char*)"pcid Process context identifiers (CR4 bit 17)",
    18, (char*)"dca Direct cache access for DMA writes[8][9]",
    19, (char*)"sse4_1 SSE4.1 instructions",
    20, (char*)"sse4_2 SSE4.2 instructions",
    21, (char*)"x2apic x2APIC support",
    22, (char*)"movbe MOVBE instruction (big-endian, Intel Atom only)",
    23, (char*)"popcnt POPCNT instruction",
    24, (char*)"tscdeadline APIC supports one-shot operation ",
    25, (char*)"aes AES instruction set",
    26, (char*)"xsave XSAVE, XRESTOR, XSETBV, XGETBV",
    27, (char*)"osxsave XSAVE enabled by OS",
    28, (char*)"avx Advanced Vector Extensions",
    29, (char*)"f16c CVT16 instruction set",
    30, (char*)"rdrnd RDRAND (on-chip random number generator) support",
    31, (char*)"hypervisor",
};

struct cpuCapability EBX2Capability[32]= {
     0,   (char*)"fsgsbase",
     1,   (char*)"IA32_TSC_ADJUST",
     2,   (char*)"sgx",
     3,   (char*)"bmi1",
     4,   (char*)"hle",
     5,   (char*)"avx2",
     -6,  (char*)"",
     7,   (char*)"smep",
     8,   (char*)"bmi2",
     9,   (char*)"erms",
    10,   (char*)"invpcid",
    11,   (char*)"rtm ",
    12,   (char*)"pqm",
    -13,  (char*)"",
    14,   (char*)"mpx",
    15,   (char*)"pqe",
    16,   (char*)"avx512f",
    17,   (char*)"avx512dq ",
    18,   (char*)"rdseed",
    19,   (char*)"adx",
    20,   (char*)"smap",
    21,   (char*)"avx512ifma",
    22,   (char*)"pcommit",
    23,   (char*)"clflushopt",
    24,   (char*)"clwb ",
    25,   (char*)"intel_pt",
    26,   (char*)"avx512pf",
    27,   (char*)"avx512er",
    28,   (char*)"avx512cd",
    29,   (char*)"sha",
    30,   (char*)"avx512bw ",
    31,   (char*)"avx512vl",
};

struct cpuCapability ECX2Capability[32]= {
     0,   (char*)"prefetchwt1",
     1,   (char*)"avx512vbmi",
     2,   (char*)"umip",
     3,   (char*)"pku",
     4,   (char*)"ospke",
     -5,  (char*)"", 
     -6,  (char*)"", 
     -7,  (char*)"",   
     8,   (char*)"gfni",
     9,   (char*)"vaes",
     10,  (char*)"vpclmulqdq ",
     11,  (char*)"avx512vnni ",
     12,  (char*)"avx512bitalg",
     -13, (char*)"",
     14,  (char*)"avx512vpopcntdq",
     -15, (char*)"",
     -16, (char*)"",
     -17, (char*)"",
     -18, (char*)"",
     -19, (char*)"",
     20, (char*)"smap",
     -21, (char*)"",
     22,  (char*)"rdpid",
     -23,  (char*)"",
     -24,  (char*)"",
     -25,  (char*)"",
     -26,  (char*)"",
     -27,  (char*)"",
     -28,  (char*)"",
     -29,  (char*)"",
     -30,  (char*)"",
     -31,  (char*)"",
};

struct cpuCapability EDX2Capability[32]= {
     0,   (char*)"",
     -1,   (char*)"",
     2,   (char*)"avx512_4vnniw",
     3,   (char*)"avx512_4fmaps",
     -4,   (char*)"",
     -5,  (char*)"", 
     -6,  (char*)"", 
     -7,  (char*)"",   
     -8,   (char*)"",
     -9,   (char*)"",
     -10,  (char*)"",
     -11,  (char*)"",
     -12,  (char*)"",
     -13, (char*)"",
     -14,  (char*)"",
     -15, (char*)"",
     -16, (char*)"",
     -17, (char*)"",
     18, (char*)"pconfig",
     -19, (char*)"",
     -20, (char*)"",
     -21, (char*)"",
     -22,  (char*)"",
     -23,  (char*)"",
     -24,  (char*)"",
     -25,  (char*)"",
     26,  (char*)"spec_ctrl",
     27,  (char*)"IBC",
     -28,  (char*)"",
     29,  (char*)"IA32_ARCH_CAPABILITIES",
     -30,  (char*)"",
     31,  (char*)"SSBD",
};

void printFeatures(u32 capabilities, struct cpuCapability* rg)
{
    int i;

    for(i = 0; i < 32; i++) {
        if(rg[i].offset < 0)
            continue;
        if(((capabilities>>rg[i].offset)&1)!=0)
            printf("\t");
        else
            printf("\tNO ");
        printf("%s\n", rg[i].feature);
    }
}


// ---------------------------------------------------------------------------------------------


void callcpuid(u32 a, u32 b, u32 c, u32 d, u32 out[4]) {
    u32     a1, a2, a3, a4;

    asm volatile(
        "\tmovl    %[a], %%eax\n" \
        "\tmovl    %[b], %%ebx\n" \
        "\tmovl    %[c], %%ecx\n" \
        "\tmovl    %[d], %%edx\n" \
        "\tcpuid   \n" \
        "\tmovl    %%eax, %[a1]\n" \
        "\tmovl    %%ebx, %[a2]\n" \
        "\tmovl    %%ecx, %[a3]\n" \
        "\tmovl    %%edx, %[a4]\n" \
        : [a1] "=m"(a1), [a2] "=m"(a2), [a3] "=m"(a3), [a4] "=m"(a4)
        : [a] "m" (a), [b] "m" (b), [c] "m" (c), [d] "m" (d)
        : "%eax", "%ebx", "%ecx", "%edx");

        out[0]= a1;
        out[1]= a2;
        out[2]= a3;
        out[3]= a4;
}


int main(int an, char** av) {
    u32     a, b;
    u32     retValues[4];
    u32     maxArg;
    char*   p;
    char*   pLabel;
    u32     ui;

    a= 0;
    callcpuid(a, 0, 0, 0,retValues);
    printf("cpuid(%d): %08x %08x %08x %08x   %s\n", a,
            retValues[0], retValues[1], retValues[2], retValues[3],
            rgCallName[a]);
    maxArg= retValues[0];
    if (maxArg > MAXCALL)
      maxArg = MAXCALL;
    printf("\tmaximum input value: %d, ", maxArg);
    p=(char*) &retValues[1];
    printf("%c%c%c%c", *p,*(p+1), *(p+2), *(p+3));
    p=(char*) &retValues[3];
    printf("%c%c%c%c", *p,*(p+1), *(p+2), *(p+3));
    p=(char*) &retValues[2];
    printf("%c%c%c%c", *p,*(p+1), *(p+2), *(p+3));
    printf("\n\n");

    a= 1;
    callcpuid(a, 0, 0, 0, retValues);
    printf("cpuid(%d): %08x %08x %08x %08x   %s\n", a,
            retValues[0], retValues[1], retValues[2], retValues[3],
            rgCallName[a]);
    b= retValues[0];
    printf("\tExtended family: %x\n", (b>>20)&0xff);
    printf("\tExtended model: %x\n", (b>>16)&0xf);
    printf("\tProcessor type: %x\n", (b>>12)&0x3);
    printf("\tFamily: %x\n", (b>>8)&0xf);
    printf("\tModel: %x\n", (b>>4)&0xf);
    printf("\tStepping: %x\n", b&0xf);
    printf("\nFeatures:\n");
    printFeatures(retValues[3], EDXCapability);
    printf("\n");
    printFeatures(retValues[2], ECXCapability);
    printf("\n");

    a= 7;
    callcpuid(a, 0, 0, 0, retValues);
    printf("cpuid(%d): %08x %08x %08x %08x   %s\n", a,
            retValues[0], retValues[1], retValues[2], retValues[3],
            rgCallName[a]);
    printFeatures(retValues[1], EBX2Capability);
    printf("\n");
    printFeatures(retValues[2], ECX2Capability);
    printf("\n");
    printFeatures(retValues[3], EDX2Capability);
    printf("\n");

    for(ui=0; ui <= maxArg; ui++) {
        callcpuid(ui, 0, 0, 0, retValues);
        printf("cpuid(%2d): %08x %08x %08x %08x   %s\n", ui,
                retValues[0], retValues[1], retValues[2], retValues[3],
                rgCallName[ui]);
    }

    printf("\n");
    ui= 0x80000006;
    pLabel= (char*)"Extended cache Features";
    callcpuid(ui, 0, 0, 0, retValues);
    printf("cpuid(%08x): %08x %08x %08x %08x   %s\n", ui,
           retValues[0], retValues[1], retValues[2], retValues[3],
           pLabel);
    ui= 0x80000007;
    pLabel= (char*)"Advanced Power Management";
    callcpuid(ui, 0, 0, 0, retValues);
    printf("cpuid(%08x): %08x %08x %08x %08x   %s\n", ui,
           retValues[0], retValues[1], retValues[2], retValues[3],
           pLabel);
    ui= 0x80000008;
    pLabel= (char*)"Virtual and physical memory size";
    callcpuid(ui, 0, 0, 0, retValues);
    printf("cpuid(%08x): %08x %08x %08x %08x   %s\n", ui,
           retValues[0], retValues[1], retValues[2], retValues[3],
           pLabel);

    return(0);
}


/* --------------------------------------------------------------------- */


