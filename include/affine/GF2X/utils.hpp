#pragma once

#include <NTL/GF2X.h>
#include <sstream>

#include "generic/octet.hpp"
#include "affine/GF2X/ec.hpp"
#include "affine/GF2X/ec_compress.hpp"

using namespace NTL;

namespace Affine
{
    namespace GF2X
    {

        using NTL::GF2X;
        using NTL::GF2X;
        
        GF2X pi(const EC_Point & X);

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

        inline GF2X GF2X_str(const char * source)
        {
            std::istringstream source_(source);
            GF2X result;
            source_ >> result;
    
            return result;
        }

        inline ByteSeq I2OSP(const GF2X & I, size_t pad=0)
        {
            unsigned char buffer [OCTET_MAX_SIZE];
    
            /* TODO: FIX BYTE ORDER */
    
            BytesFromGF2X(buffer, I, sizeof(buffer));
            const size_t source_size = NumBytes(I);

            return ByteSeq(buffer, source_size, pad, true);
        }

        inline GF2X OS2IP(const ByteSeq & seq)
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

        inline size_t Lb(const ByteSeq & x)
        { return x.getDataSize()*8; }

        /* Length in bytes */

        inline size_t L(const GF2X & x)
        { return NumBytes(x); }

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

    }
}
