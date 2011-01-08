#include "hash.hpp"
#include "string.h"

Hash::Hash(Hash_Type type)
{
    setupHasher(type);
}

Hash::~Hash(){}

static inline void __rmd160_hash_buffer (unsigned char *source, int source_size,
                                    unsigned char * buffer, int buffer_size)
{
    unsigned char __buffer[20];
    
    rmd160_hash_buffer((char *)__buffer,
                       (const char *)source,
                       source_size);
    memcpy(buffer, __buffer, buffer_size);
}


void Hash::setupHasher(Hash_Type type)
{
    switch (type)
    {
        case RIPEMD160:
            __hash_function = __rmd160_hash_buffer;
            __hash_size = 20;
            break;
            
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

/* =================== HASH: ZZ_p ===================== */

Hash_ZZ_p::Hash_ZZ_p(Hash::Hash_Type type)
    :Hash(type) {}

Hash_ZZ_p::~Hash_ZZ_p() {}

ZZ Hash_ZZ_p::toZZ(const unsigned char * hash, const size_t hash_size) const
{
    // Fix const in NTL ?

    const size_t this_hash_size = getHashSize();

    unsigned char _hash_reverse[1024 + 4]; // Buffer for strict order data

    for (unsigned int i=0; i<hash_size; i++)
        _hash_reverse[i] = hash[hash_size-i-1];
    
    return ZZFromBytes((unsigned char *) _hash_reverse,
                       this_hash_size < hash_size ?
                       this_hash_size :
                       hash_size);
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
    unsigned char _source_reverse[1024 + 4]; // Buffer for strict order data
    
    long _source_size = min(NumBytes(ZZ_source), (long int) sizeof(_source));
    long _source_pad = 4 - (_source_size % 4);
    
    /* TODO: FIX BYTE ORDER */
    
    BytesFromZZ(_source, ZZ_source, _source_size);

    memset(_source_reverse, 0x0, _source_pad);
    
    for (unsigned int i=0; i<_source_size; i++)
    {
        _source_reverse[i + _source_pad] =
            _source[_source_size - i - 1];
    }
    


    unsigned char * hash = getHash(_source_reverse,
                                   _source_size + _source_pad,
                                   buffer, sizeof(buffer));
    
    return toZZ_p(hash, getHashSize());
}

