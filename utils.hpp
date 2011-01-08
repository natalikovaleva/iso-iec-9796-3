#pragma once

#include <NTL/ZZ_p.h>
#include <sstream>

#include "octet.hpp"
#include "ec.hpp"
#include "ec_compress.hpp"

using namespace NTL;

ZZ_p pi(const EC_Point & X);
ZZ_p pi(const EC_CPoint & X);

/* Trivial faster compression */

enum EC2OSP_COMPRESS_MODE
{
    EC2OSP_UNCOMPRESSED,
    EC2OSP_COMPRESSED,
    EC2OSP_HYBRID
};
    

ByteSeq EC2OSP(const EC_Point & Point,
               EC2OSP_COMPRESS_MODE mode = EC2OSP_COMPRESSED);

ByteSeq I2OSP(unsigned I, size_t pad=0);

inline ZZ_p ZZ_p_str(const char * source)
{
    std::istringstream source_(source);
    ZZ_p result;
    source_ >> result;
    
    return result;
}

inline ByteSeq I2OSP(const ZZ_p & I, size_t pad=0)
{ return ByteSeq(rep(I),pad); }

inline ByteSeq I2OSP(const ZZ & I, size_t pad=0)
{ return ByteSeq(I, pad); }

inline ZZ OS2IP(const ByteSeq & seq)
{ return seq.getZZ(); }

/* Length in bits */
inline size_t Lb(const ZZ & x)
{ return NumBits(x); }

inline size_t Lb(const ZZ_p & x)
{ return NumBits(rep(x)); }

inline size_t Lb(const ByteSeq & x)
{ return x.getDataSize()*8; }

/* Length in bytes */

inline size_t L(const ZZ & x)
{ return NumBytes(x); }

inline size_t L(const ZZ_p & x)
{ return NumBytes(rep(x)); }

inline size_t L(const ByteSeq & x)
{ return x.getDataSize(); }

/* Octet/ByteSeq truncation */
inline ByteSeq Truncate(const ByteSeq & input,
                        const size_t octets)
{
    const size_t input_size = input.getDataSize();
    return ByteSeq(input.getData(),
                   input_size < octets ?
                   input_size : octets);
}

inline ZZ_p InMod(const ZZ & x)
{
    ZZ_p r; conv(r,x); return r;
}

    
     
