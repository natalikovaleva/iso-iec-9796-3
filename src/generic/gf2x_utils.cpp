#include "generic/gf2x_utils.hpp"

using namespace NTL;

ByteSeq GF2X2OSP(unsigned int I, size_t pad)
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
