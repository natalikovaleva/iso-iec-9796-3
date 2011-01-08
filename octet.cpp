#include "octet.hpp"

Octet::Octet()
    : __data_size(0)
{}

Octet::Octet(const unsigned char * data, size_t data_size)
{
    __data_size = data_size < OCTET_MAX_SIZE ? data_size : OCTET_MAX_SIZE;
    memcpy(__data, data, __data_size);
}


size_t Octet::rotateAndPad(
    unsigned char * dest,
    size_t dest_size,
    const unsigned char * source,
    size_t source_size)
{
    const long rotate_size = source_size < dest_size ?
        source_size : dest_size;
    const long rotate_pad  = 4 - (rotate_size % 4);
    
    memset(dest, 0x0, rotate_pad);
    
    for (unsigned int i=0; i<source_size; i++)
    {
        dest[i + rotate_pad] =
            source[rotate_size - i - 1];
    }

    return ( rotate_size + rotate_pad );
}

Octet::Octet(const ZZ & source)
{
    unsigned char buffer [OCTET_MAX_SIZE-4];

    /* TODO: FIX BYTE ORDER */
    
    BytesFromZZ(buffer, source, sizeof(buffer));
    const size_t source_size = NumBytes(source);
    
    __data_size = rotateAndPad(__data, sizeof(__data),
                               buffer, (source_size < sizeof(buffer) ?
                                        source_size : sizeof(buffer)));
}

Octet::operator ZZ()
{
    unsigned char buffer [OCTET_MAX_SIZE];

    size_t size = rotateAndPad(buffer, sizeof(buffer),
                               __data, __data_size);

    return ZZFromBytes(buffer, size);
}

Octet Octet::operator|| (const Octet & y) const
{
    Octet result(*this);
    
    if (result.__data_size + y.__data_size < OCTET_MAX_SIZE)
    {
        memcpy(result.__data+__data_size, y.__data, y.__data_size);
    }
    else
    {
        if (y.__data_size == OCTET_MAX_SIZE)
        {
            memcpy(result.__data, y.__data, OCTET_MAX_SIZE);
            result.__data_size = OCTET_MAX_SIZE;
        }
        else
        {
            const size_t overflow = y.__data_size + result.__data_size - OCTET_MAX_SIZE;
            memmove(result.__data, result.__data+overflow, result.__data_size-overflow);
            memcpy(result.__data+result.__data_size-overflow, y.__data, y.__data_size);
        }
    }

    return result;
}

