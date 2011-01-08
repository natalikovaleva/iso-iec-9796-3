#include "utils.hpp"

#include "ec_compress.hpp"

/* Smart stupid !! OCTET !! concatentaion. Be carefull! :] */

#include <stdio.h>

ByteSeq I2OSP(unsigned int I, size_t pad)
{
    const size_t size = NumBytes(I);
    const size_t pad_need  = pad ? (size % sizeof(int)) : 0;
    const size_t this_pad = pad_need ? sizeof(int) - pad_need : 0;
    
    unsigned char buffer [sizeof(int)];
   
    memset(buffer, 0x0, sizeof(buffer));

    /* slow memcopy; LE */
    for (unsigned int i = 0; i<size; i++)
    {
        buffer[sizeof(int)-i-1] = *((unsigned char *)(&I) + i);
    }

    return ByteSeq(buffer + (sizeof(int) - (size + this_pad)),
                   size + this_pad, pad);
}


ByteSeq EC2OSP(const EC_Point & Point, EC2OSP_COMPRESS_MODE mode)
{
    const unsigned char tY = EC_CPoint::compress_tY(Point);

    if (Point.isZero())
        return ByteSeq(0);

    const unsigned int U = ((mode == EC2OSP_UNCOMPRESSED) ||
                            (mode == EC2OSP_HYBRID)) ? 1 : 0;
    const unsigned int C = ((mode == EC2OSP_COMPRESSED) ||
                            (mode == EC2OSP_HYBRID)) ? 1 : 0;
    
    const ByteSeq H(I2OSP(4*U+C*(2+tY)));
    const Octet   X(rep(Point.getX()));
    
    if (U)
        return H || X || Octet(rep(Point.getY()));
    else
        return H || X;
}

