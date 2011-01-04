#pragma once

#include "ec.hpp"


class EC_Defaults
{
public:
    enum Sizes
    {
        EC192 = 192
    };
    

    const Sizes __this_size;
    
public:

    EC create(void) const;

    EC_Defaults(Sizes size);
    ~EC_Defaults();
    
};
