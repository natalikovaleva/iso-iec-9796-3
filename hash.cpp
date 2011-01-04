#include "hash.hpp"

Hash::Hash(Hash_Type type)
{
    setupHasher(type);
}

Hash::~Hash(){}

void Hash::setupHasher(Hash_Type type)
{
    switch (type)
    {
        case SHA256:
            __hash_function = sha256_hash_buffer;
            __hash_size = 32;
            break;

        case SHA384:
            __hash_function = sha384_hash_buffer;
            __hash_size = 48;
            break;

        case SHA512:
            __hash_function = sha512_hash_buffer;
            __hash_size = 64;
            break;

        default:
            throw;
    }
}

size_t Hash::getHashSize(void) const
{
    return __hash_size;
}


unsigned char * Hash::getHash(const unsigned char * source, size_t source_size,
                     unsigned char * buffer, size_t buffer_size) const
{
    int ss = source_size,
        bs = buffer_size;
    
    // FIX sha512.c: const functions
    __hash_function((unsigned char *) source, ss,
                    buffer, bs);

    return buffer;
}

const unsigned  char * Hash::getHash(const unsigned char * source, size_t source_size)
{
    int ss = source_size;

    // FIX sha512.c: const functions
    __hash_function((unsigned char *) source, ss,
                    __hash_buffer, sizeof(__hash_buffer));

    return __hash_buffer;
}

/* =================== HASH: ZZ_p ===================== */

Hash_ZZ_p::Hash_ZZ_p(Hash::Hash_Type type)
    :Hash(type) {}

Hash_ZZ_p::~Hash_ZZ_p() {}

ZZ Hash_ZZ_p::toZZ(const unsigned char * hash, size_t hash_size) const
{
    // Fix const in NTL ?
    return ZZFromBytes((unsigned char *) hash, getHashSize());
}


ZZ_p Hash_ZZ_p::toZZ_p(const unsigned char * hash, size_t hash_size) const
{
    ZZ_p result;
    
    conv(result, toZZ(hash, hash_size));

    return result;
}

ZZ_p Hash_ZZ_p::operator() (const unsigned char * source, size_t source_size) const
{
    unsigned char buffer[128]; // const payment

    unsigned char * hash = getHash(source, source_size,
                                   buffer, sizeof(buffer));
    
    return toZZ_p(hash, getHashSize());
}

ZZ_p Hash_ZZ_p::operator() (const ZZ_p & ZZ_p_source) const
{
    const ZZ & ZZ_source = rep(ZZ_p_source);

    return operator()(ZZ_source);
}

ZZ_p Hash_ZZ_p::operator() (const ZZ   & ZZ_source) const
{
    unsigned char buffer[128]; // const payment

    unsigned char _source[1024]; // Max size: 1024 bytes == 8192 bits.. enougth for EC
    long _source_size = max(NumBytes(ZZ_source), (long int) sizeof(_source));
    
    BytesFromZZ(_source, ZZ_source, _source_size);
    
    unsigned char * hash = getHash(_source, _source_size,
                                   buffer, sizeof(buffer));
    
    return toZZ_p(hash, getHashSize());
}

