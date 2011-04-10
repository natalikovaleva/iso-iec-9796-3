#include "ec/ZZ_p/affine/utils.hpp"
#include "generic/convhex.h"

using namespace std;
using namespace NTL;
using namespace ECZZ_p::Affine;

/* Smart stupid !! OCTET !! concatentaion. Be carefull! :] */

#include <stdio.h>

namespace ECZZ_p
{
    namespace Affine
    {
        ByteSeq EC2OSP(const EC_Point & Point, EC::EC2OSP_COMPRESS_MODE mode)
        {
            const unsigned char tY = EC_CPoint::compress_tY(Point);

            if (Point.isZero())
                return ByteSeq(0);

            const unsigned int U = ((mode == EC::EC2OSP_UNCOMPRESSED) ||
                                    (mode == EC::EC2OSP_HYBRID)) ? 1 : 0;
            const unsigned int C = ((mode == EC::EC2OSP_COMPRESSED) ||
                                    (mode == EC::EC2OSP_HYBRID)) ? 1 : 0;
    
            const Octet   X(I2OSP(Point.getX(),4));
            const ByteSeq H(I2OSP(4*U+C*(2+tY)));
    
            if (U)
                return H || X || I2OSP(Point.getY(),4);
            else
                return H || X;
        }
    }
}

