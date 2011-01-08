#pragma once

#include <NTL/ZZ_p.h>
#include <sys/types.h>

#include "octet.hpp"

extern "C" 
{
#include "rmd160.h"
#include "sha512.h"
}


using namespace NTL;


class Hash
{
public:
    enum Hash_Type
    {
        RIPEMD160,
        SHA256,
        SHA384,
        SHA512
    };

private:
    
    void (* __hash_function) (unsigned char *ib,
                              int ile,
                              unsigned char *ob,
                              int ole);
    
    size_t __hash_size;

    void setupHasher(Hash_Type type);

public:
    Hash(Hash_Type type);
    ~Hash();
    
    unsigned char * getHash(const unsigned char * source, size_t source_size,
                   unsigned char * buffer, size_t buffer_size) const;
    
    inline size_t getHashSize(void) const;
    
};

class Hash_Seq : public Hash
{
public:
    Hash_Seq(Hash::Hash_Type type);

    ByteSeq operator() (const ByteSeq & source) const;
};

    
    
