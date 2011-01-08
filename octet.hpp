#pragma once

#include <NTL/ZZ.h>

#include <string.h>

using namespace NTL;

#define OCTET_MAX_SIZE 8196

class Octet
{
    unsigned char __data[OCTET_MAX_SIZE];
    size_t __data_size;

public:
    static size_t rotateAndPad(unsigned char * dest,
                               size_t dest_size,
                               const unsigned char * source,
                               size_t source_size);
public:
    Octet();
    Octet(const unsigned char * data, size_t data_size);
    Octet(const ZZ & source);

    inline Octet(const Octet & source)
        {
            __data_size = source.__data_size;
            memcpy(__data, source.__data, source.__data_size);
        }
    
        
    inline const unsigned char * getData() const
        { return __data; }
    
    inline size_t getDataSize() const
        { return __data_size; }
    
    Octet operator|| (const Octet & y) const;

    operator ZZ();

};

