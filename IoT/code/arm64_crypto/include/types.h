#include <stdio.h>
#include <sys/types.h>
#include <string>

#ifndef ARM64_TYPES__
#define ARM64_TYPES__

#ifndef byte
typedef unsigned char byte;
#endif
using namespace std;

const int num_bits_in_uint64_t = 64;
const int num_bits_in_byte = 8;

#endif

