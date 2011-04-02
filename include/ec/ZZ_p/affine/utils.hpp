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

        enum EC2OSP_COMPRESS_MODE
        {
            EC2OSP_UNCOMPRESSED,
            EC2OSP_COMPRESSED,
            EC2OSP_HYBRID
        };

        ByteSeq EC2OSP(const EC_Point & Point,
                       EC2OSP_COMPRESS_MODE mode = EC2OSP_COMPRESSED);
        
        inline ByteSeq FE2OSP(const ZZ_p & I, size_t pad=0)
        { return I2OSP(I, pad); }

        inline ZZ_p OS2FEP(const ByteSeq & seq)
        { return InMod(OS2IP(seq)); }
        
    }
}
