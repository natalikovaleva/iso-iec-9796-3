#pragma once

#include <NTL/ZZ_p.h>
#include <sstream>

#include "generic/octet.hpp"
#include "generic/zz_utils.hpp"
#include "ec/ZZ_p/affine/ec.hpp"
#include "ec/ZZ_p/affine/ec_compress.hpp"

using namespace NTL;

namespace ECZZ_p
{
    namespace Affine
    {
        /* Trivial faster compression */

        ByteSeq EC2OSP(const EC_Point & Point,
                       EC::EC2OSP_COMPRESS_MODE mode = EC::EC2OSP_COMPRESSED);

        inline ByteSeq FE2OSP(const ZZ_p & I, size_t pad=0)
        { return I2OSP(I, pad); }

        inline ZZ FE2IP(const ZZ_p & I)
        { return rep(I); }

        inline ZZ_p OS2FEP(const ByteSeq & seq)
        { return InMod(OS2IP(seq)); }
    }
}

#ifdef OS2FEP_TPL
template <>
struct tOS2FEP<ECZZ_p::Affine::EC_Point>
{
    inline ECZZ_p::Affine::EC_Point::FE operator()(const ByteSeq & From)
        { return ECZZ_p::Affine::OS2FEP(From); }
};
#endif

