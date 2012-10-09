#pragma once

#include <NTL/GF2X.h>
#include "generic/convhex.h"
#include "generic/octet.hpp"

using namespace NTL;

ByteSeq GF2X2OSP(unsigned int I, size_t pad);

inline ByteSeq GF2X2OSP(const GF2X & I, size_t pad=0)
{
    unsigned char buffer [OCTET_MAX_SIZE];

    /* TODO: FIX BYTE ORDER */

    BytesFromGF2X(buffer, I, sizeof(buffer));
    const size_t source_size = NumBytes(I);

    return ByteSeq(buffer, source_size, pad, true);
}

inline GF2X OS2GF2XP(const ByteSeq & seq)
{
    unsigned char buffer [OCTET_MAX_SIZE];

    const unsigned char * data = seq.getData();
    const size_t data_size = seq.getDataSize();
    const size_t __data_size =
        data_size > sizeof(buffer) ?
        sizeof(buffer) : data_size;

    for (unsigned int i = 0; i<__data_size; i++)
    {
        buffer[i] = data[__data_size - i - 1];
    }

    return GF2XFromBytes(buffer, __data_size);
}

/* Length in bits */
inline size_t Lb(const GF2X & x)
{ return NumBits(x); }

/* Length in bytes */

inline size_t L(const GF2X & x)
{ return NumBytes(x); }

GF2X GF2X_str(const char * source);

/* Some Number Theory functions */

GF2X Half_Trace(const GF2X & input, const GF2XModulus & m);
GF2X Z2ZB_Solve(const GF2X & b, const GF2XModulus & m);
