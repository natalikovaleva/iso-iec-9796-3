#pragma once

#include <NTL/GF2X.h>
#include <sstream>

#include "generic/octet.hpp"
#include "generic/convhex.h"
#include "generic/gf2x_utils.hpp"
#include "ec/GF2X/affine/ec.hpp"
#include "ec/GF2X/affine/ec_compress.hpp"

#include "ec/ZZ_p/affine/utils.hpp"

using namespace NTL;

namespace ECGF2X
{
    namespace Affine
    {
        using NTL::GF2X;
        
        GF2X pi(const EC_Point & X);

        /* Trivial faster compression */

        enum EC2OSP_COMPRESS_MODE
        {
            EC2OSP_UNCOMPRESSED,
            EC2OSP_COMPRESSED,
            EC2OSP_HYBRID
        };

        ByteSeq EC2OSP(const EC_Point & Point, EC2OSP_COMPRESS_MODE mode);

        inline ByteSeq FE2OSP(unsigned int I, size_t pad=0)
        { return GF2X2OSP(I, pad); }
        
        inline ByteSeq FE2OSP(const GF2X & I, size_t pad=0)
        { return GF2X2OSP(I, pad); }
        
        inline GF2X OS2FEP(const ByteSeq & seq)
        { return OS2GF2XP(seq); }
    }
}

