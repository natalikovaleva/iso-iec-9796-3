#pragma once

#include <NTL/GF2X.h>
#include <sstream>

#include "generic/octet.hpp"
#include "generic/convhex.h"
#include "affine/GF2X/ec.hpp"
#include "affine/GF2X/ec_compress.hpp"

#include "affine/ZZ_p/utils.hpp"

using namespace NTL;

namespace Affine
{
    namespace GF2X
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

        ByteSeq FE2OSP(unsigned I, size_t pad=0);

        GF2X GF2X_str(const char * source);
        
        inline ByteSeq FE2OSP(const GF2X & I, size_t pad=0)
        {
            unsigned char buffer [OCTET_MAX_SIZE];
    
            /* TODO: FIX BYTE ORDER */
    
            BytesFromGF2X(buffer, I, sizeof(buffer));
            const size_t source_size = NumBytes(I);

            return ByteSeq(buffer, source_size, pad, true);
        }

        inline GF2X OS2FEP(const ByteSeq & seq)
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

        /* Some Number Theory functions */

        GF2X Half_Trace(const GF2X & input, const GF2XModulus & m);
        GF2X Z2ZB_Solve(const GF2X & b, const GF2XModulus & m);

        /* Some primitives, that can be used
         * TODO: Move this stuff to other header */

        using Affine::ZZ_p::ZZ_str;
        using Affine::ZZ_p::I2OSP;
        using Affine::ZZ_p::L;
        using Affine::ZZ_p::Lb;
        using Affine::ZZ_p::OS2IP;
        using Affine::ZZ_p::InMod;
    }
}

