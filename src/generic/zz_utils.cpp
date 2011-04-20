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
