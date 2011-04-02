#include "generic/zz_utils.hpp"

using namespace NTL;

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
