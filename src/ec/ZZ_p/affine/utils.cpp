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
        
        ZZ ZZ_str(const char * source)
        {
            const size_t source_size = strlen(source);
            
            ZZ r;

            for (unsigned int i=0; i<source_size; i++)
            {
                const unsigned char q = hexc_to_dec(source[i]);

                for (unsigned int j=0; j<4; j++)
                {
                    if ( q & (0x8 >> j) )
                    {
                        SetBit(r, ((source_size-1 - i) * 4) + (4-j-1));
                    }
                }
            }
            
            return r;
        }

        ZZ_p ZZ_p_str(const char * source)
        {
            ZZ_p r;
            conv(r, ZZ_str(source));
            return r;
        }
        
        
        ByteSeq I2OSP(unsigned int I, size_t pad)
        {
            const size_t size = NumBytes(I);
            const size_t pad_need  = pad ? ( size % sizeof(int) ) : 0;
            const size_t this_pad = pad_need ? sizeof(int) - pad_need : 0;

            unsigned char buffer [sizeof(int)];
   
            memset(buffer, 0x0, sizeof(buffer));

            /* If size = 0, then return 00000000 octet */
            
            if (I == 0)
            {
                return ByteSeq(buffer, sizeof(buffer), pad);
            }
            
            /* slow memcopy; LE */
            for (unsigned int i = 0; i<size; i++)
            {
                buffer[sizeof(int)-i-1] = *((unsigned char *)(&I) + i);
            }

            return ByteSeq(buffer + (sizeof(int) - (size + this_pad)),
                           ( size + this_pad ) , pad);
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
    
            const Octet   X(I2OSP(Point.getX(),4));
            const ByteSeq H(I2OSP(4*U+C*(2+tY)));
    
            if (U)
                return H || X || I2OSP(Point.getY(),4);
            else
                return H || X;
        }
    }
}
