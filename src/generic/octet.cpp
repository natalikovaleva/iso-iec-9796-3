#include <iomanip>

#include "generic/octet.hpp"


void ByteSeq::setData(const unsigned char * data, size_t data_size, bool rotate)
{
    __data_size = data_size < OCTET_MAX_SIZE ? data_size : OCTET_MAX_SIZE;

    const long pad_need = __pad ? __data_size%__pad : 0;
    const long pad_size = pad_need ? __pad-pad_need : 0;

    if (pad_size)
        memset(__data, 0x0, pad_size);

    const size_t copy_size =
        __data_size + pad_size <= OCTET_MAX_SIZE ?
        __data_size : OCTET_MAX_SIZE - pad_size;

    if (rotate)
        for (unsigned int i=0; i<copy_size; i++)
            __data[i + pad_size] = data[copy_size - i - 1];
    else
        memcpy(__data + pad_size, data, data_size);
    
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

