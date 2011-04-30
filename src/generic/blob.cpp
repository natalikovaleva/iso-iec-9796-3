#include <iomanip>
#include <iostream>

#include "generic/blob.hpp"

std::ostream& operator<<(std::ostream& s, const ManagedBlob & octet)
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


