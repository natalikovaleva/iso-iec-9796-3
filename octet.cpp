#include "octet.hpp"

#include <iomanip>

void ByteSeq::setData(const unsigned char * data, size_t data_size)
{
    __data_size = data_size < OCTET_MAX_SIZE ? data_size : OCTET_MAX_SIZE;

    const long pad_need = __pad ? __data_size%__pad : 0;
    const long pad_size = pad_need ? __pad-pad_need : 0;

    if (pad_size)
        memset(__data, 0x0, pad_size);
    
    if (__data_size + pad_size <= OCTET_MAX_SIZE)
        memcpy(__data + pad_size, data, __data_size);
    else
        memcpy(__data + pad_size, data, OCTET_MAX_SIZE - pad_size);

    __data_size += pad_size;
}

size_t ByteSeq::rotateAndPad(
    unsigned char * dest,
    size_t dest_size,
    const unsigned char * source,
    size_t source_size) const
{
    const long rotate_size = source_size < dest_size ?
        source_size : dest_size;
    
    const long pad_need = __pad ? source_size%__pad : 0;
    const long rotate_pad  = pad_need ? __pad-pad_need : 0;
    
    memset(dest, 0x0, rotate_pad);
    
    for (unsigned int i=0; i<source_size; i++)
    {
        dest[i + rotate_pad] =
            source[rotate_size - i - 1];
    }

    return ( rotate_size + rotate_pad );
}

void ByteSeq::setData(const ZZ & source)
{
    unsigned char buffer [OCTET_MAX_SIZE-__pad];

    /* TODO: FIX BYTE ORDER */
    
    BytesFromZZ(buffer, source, sizeof(buffer));
    const size_t source_size = NumBytes(source);
    
    __data_size = rotateAndPad(__data, sizeof(__data),
                               buffer, (source_size < sizeof(buffer) ?
                                        source_size : sizeof(buffer)));
}

ZZ ByteSeq::getZZ(void) const
{
    unsigned char buffer [OCTET_MAX_SIZE];

    size_t size = rotateAndPad(buffer, sizeof(buffer),
                               __data, __data_size);

    return ZZFromBytes(buffer, size);

}

ByteSeq ByteSeq::operator|| (const ByteSeq & y) const
{
    ByteSeq result(*this);
    
    if (result.__data_size + y.__data_size < OCTET_MAX_SIZE)
    {
        memcpy(result.__data+__data_size, y.__data, y.__data_size);
        result.__data_size = result.__data_size + y.__data_size;
        
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
            result.__data_size = OCTET_MAX_SIZE;
        }
    }

    return result;
}

std::ostream& operator<<(std::ostream& s, const ByteSeq & octet)
{

    std::ios_base::fmtflags original_flags = s.flags();

    s << std::hex;
    
    for (unsigned int i=0; i<octet.__data_size; i++)
    {
        s << std::setfill('0') << std::setw(2) <<
            (unsigned int) octet.__data[i];
        if ((i+1)%4==0)
            s << " ";
    }

    s.flags(original_flags);

    return s;
}

