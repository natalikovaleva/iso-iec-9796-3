#pragma once

#include <ZZ.h>
#include <sys/types.h>

#include "sha512.h"


class Hash
{
    unsigned char data[128];

    void (__hash_buffer *) (unsigned char *ib,
                            int ile,
                            unsigned char *ob,
                            int ole);

    void setupHasher(Hash_Type type);

    const char * toChar();

    const union 
    {
        char * _asChar;
        ZZ_p & _asZZ_p;
        ZZ   & _asZZ;
    };

    const int _dataSize;
    
    
    
public:
    enum Hash_Type
    {
        SHA256,
        SHA384,
        SHA512
    }


        
public:
    Hash(Hash_Type type, const char * source, size_t length);
    Hash(Hash_Type type, const ZZ_p & source);
    Hash(Hash_Type type, const ZZ & source);
   
    ~Hash();

    ZZ getZZ() const;
    const char * getData() const;
    
    
};

