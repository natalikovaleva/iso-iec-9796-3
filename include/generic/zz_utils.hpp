#pragma once

#include <NTL/ZZ_p.h>

#include "generic/convhex.h"
#include "generic/octet.hpp"

using namespace NTL;


inline ByteSeq I2OSP(unsigned int I, size_t pad = 0)
{
    const size_t size = NumBytes(I);

    if (I == 0)
    {
        char byte = 0;
        return ByteSeq(&byte, 1, pad);
    }

    return ByteSeq((const char *)&I, size, pad, true);
}

inline ByteSeq I2OSP(const ZZ & I, size_t pad=0)
{
    unsigned char buffer [OCTET_MAX_SIZE];

    /* TODO: FIX BYTE ORDER */

    BytesFromZZ(buffer, I, sizeof(buffer));
    const size_t source_size = NumBytes(I);

    return ByteSeq(buffer, source_size, pad, true);
}

inline ByteSeq I2OSP(const ZZ_p & I, size_t pad=0)
{
    return I2OSP(rep(I), pad);
}

inline ZZ OS2IP(const ByteSeq & seq)
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

    return ZZFromBytes(buffer, __data_size);
}

ZZ_p ZZ_p_str(const char * source);
ZZ ZZ_str(const char * source);

/* Length in bits */
inline size_t Lb(const ZZ & x)
{ return NumBits(x); }

inline size_t Lb(const ZZ_p & x)
{ return NumBits(rep(x)); }

/* Length in bytes */

inline size_t L(const ZZ & x)
{ return NumBytes(x); }

inline size_t L(const ZZ_p & x)
{ return NumBytes(rep(x)); }

inline ZZ_p InMod(const ZZ & x)
{
    ZZ_p r; conv(r,x); return r;
}
