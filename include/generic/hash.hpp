#pragma once

#include <sys/types.h>

#include "generic/octet.hpp"
#include "generic/blob.hpp"

extern "C"
{
#include "hashes/rmd160.h"
#include "hashes/sha512.h"
#include "hashes/sha1.h"
}


class Hash_Generic
{
public:
    enum Hash_Type
    {
        RIPEMD160,
        SHA1,
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
    size_t __input_size;

    void setupHasher(Hash_Type type);

public:
    Hash_Generic(Hash_Type type);
    ~Hash_Generic();

    unsigned char * getHash(const unsigned char * source, size_t source_size,
                   unsigned char * buffer, size_t buffer_size) const;

    size_t getHashSize(void) const;
    size_t getInputSize(void) const;
};

class Hash : public Hash_Generic
{
public:
    Hash(Hash::Hash_Type type);

    ByteSeq operator() (const ByteSeq & source) const;
    ByteSeq operator() (const ManagedBlob & source) const;
};
