#include "ec/GF2X/affine/utils.hpp"
#include "generic/gf2x_utils.hpp"

using namespace std;
using namespace NTL;
using namespace ECGF2X::Affine;

/* Smart stupid !! OCTET !! concatentaion. Be carefull! :] */

#include <stdio.h>

namespace ECGF2X
{
    namespace Affine
    {
        ByteSeq EC2OSP(const EC_Point & Point, EC::EC2OSP_COMPRESS_MODE mode)
        {
            const unsigned char tY =
                ((mode == EC::EC2OSP_COMPRESSED) ||
                 (mode == EC::EC2OSP_HYBRID)) ?
                EC_CPoint::compress_tY(Point) : 0;
            
            if (Point.isZero())
                return ByteSeq(0);

            /* Pad - according to example of 2^m/ECNR.
             * Check this. Don't see much sense */
            
            const long Pad = L(Point.getEC().getModulus());
            
            const unsigned int U = ((mode == EC::EC2OSP_UNCOMPRESSED) ||
                                    (mode == EC::EC2OSP_HYBRID)) ? 1 : 0;
            const unsigned int C = ((mode == EC::EC2OSP_COMPRESSED) ||
                                    (mode == EC::EC2OSP_HYBRID)) ? 1 : 0;
    
            const ByteSeq X(FE2OSP(Point.getX(), Pad));
            const ByteSeq H(FE2OSP(4*U+C*(2+tY)));
    
            if (U)
                return H || X || FE2OSP(Point.getY(), Pad);
            else
                return H || X;
        }
    }
}
