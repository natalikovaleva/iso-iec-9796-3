#pragma once

#include "ec.hpp"

class EC_Defaults
{
   
public:
    enum Sizes
    {
        EC0   = 0,   /* Broken EC */
        EC128 = 128,
        EC192 = 192,
        EC256 = 256,
        EC512 = 512
    };

    static Sizes getSize(unsigned int id);
    static unsigned int getId(Sizes size);
    static inline EC_Defaults::Sizes toSizes(long X)
        { return (EC_Defaults::Sizes) X; }
    static inline long toByteSize(const EC_Defaults::Sizes Size)
        { return (long) (Size+7)/8; }
    
    
private:
    const Sizes __this_size;
    static const Sizes __size_matrix[];
    
public:

    EC create(void) const;

    EC_Defaults(Sizes size);
    ~EC_Defaults();
};
