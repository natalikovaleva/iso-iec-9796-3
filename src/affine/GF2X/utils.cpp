#include "affine/GF2X/utils.hpp"

using namespace std;
using namespace NTL;
using namespace Affine::GF2X;

/* Smart stupid !! OCTET !! concatentaion. Be carefull! :] */

#include <stdio.h>

namespace Affine
{
    namespace GF2X
    {

        ByteSeq FE2OSP(unsigned int I, size_t pad)
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

        
        GF2X GF2X_str(const char * source)
        {
            const size_t source_size = strlen(source);
            
            GF2X r;

            for (unsigned int i=0; i<source_size; i++)
            {
                const unsigned char q = hexc_to_dec(source[i]);

                for (unsigned int j=0; j<4; j++)
                {
                    if ( q & (0x8 >> j) )
                    {
                        SetCoeff(r, ((source_size-1 - i) * 4) + (4-j-1));
                    }
                }
            }
            
            return r;
        }

        int tY_cnt;
        
        ByteSeq EC2OSP(const EC_Point & Point, EC2OSP_COMPRESS_MODE mode)
        {
            const unsigned char tY =
                ((mode == EC2OSP_COMPRESSED) ||
                 (mode == EC2OSP_HYBRID)) ?
                EC_CPoint::compress_tY(Point) : 0;
            
            if (Point.isZero())
                return ByteSeq(0);

            /* Pad - according to example of 2^m/ECNR.
             * Check this. Don't see much sense */
            
            const long Pad = L(Point.getEC().getModulus());
            
            const unsigned int U = ((mode == EC2OSP_UNCOMPRESSED) ||
                                    (mode == EC2OSP_HYBRID)) ? 1 : 0;
            const unsigned int C = ((mode == EC2OSP_COMPRESSED) ||
                                    (mode == EC2OSP_HYBRID)) ? 1 : 0;
    
            const ByteSeq X(FE2OSP(Point.getX(), Pad));
            const ByteSeq H(FE2OSP(4*U+C*(2+tY)));
    
            if (U)
                return H || X || FE2OSP(Point.getY(), Pad);
            else
                return H || X;
        }
        
        GF2X Half_Trace(const GF2X & a, const GF2XModulus & m)
        {
            GF2X H = PowerMod(a % m, 4, m) + a;

            /* Iteration 1 */

            for (long i = 2; i<(deg(a)-1) / 2; i += 2)
            {
                PowerMod(H, H % m, 4, m);
                H += a;
            }

            return H;
        }

        GF2X Z2ZB_Solve(const GF2X & b, const GF2XModulus & m)
        {
            /* TOOD: ADD ODD Check. If odd => use half_trace */
            do
            {
                const GF2X rho = random_GF2X(deg(m)) % m;
                GF2X z;
                GF2X w = rho;

                for (long i = 1; i < deg(m); i++)
                {
                    z = SqrMod(z,m) + MulMod(SqrMod(w,m), b, m);
                    w = SqrMod(w,m) + rho;
                }

                if (! IsZero(w))
                {
                    // FIXME: Remove this check? Remove throw?
                    if ( SqrMod(z,m) + z != b)
                    {
                        throw;
                    }

                    return z;
                }

            } while (1);
        }
    }
}
