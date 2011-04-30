#include <string.h>
#include "generic/hash.hpp"

#include <exception>

Hash_Generic::Hash_Generic(Hash_Type type)
{
    setupHasher(type);
}

Hash_Generic::~Hash_Generic(){}

static inline void __rmd160_hash_buffer (unsigned char *source, int source_size,
                                    unsigned char * buffer, int buffer_size)
{
    unsigned char __buffer[20];

    rmd160_hash_buffer((char *)__buffer,
                       (const char *)source,
                       source_size);
    memmove(buffer, __buffer, buffer_size);
}


void Hash_Generic::setupHasher(Hash_Type type)
{
    switch (type)
    {
        case RIPEMD160:
            __hash_function = __rmd160_hash_buffer;
            __hash_size = 20;
            __input_size = 64;
            break;

        case SHA1:
            __hash_function = sha1_hash_buffer;
            __hash_size = 20;
            __input_size = 20; /* FIXME? */
            break;

        case SHA256:
            __hash_function = sha256_hash_buffer;
            __hash_size = 32;
            __input_size = 32; /* FIXME? */
            break;

        case SHA384:
            __hash_function = sha384_hash_buffer;
            __hash_size = 48;
            __input_size = 48; /* FIXME? */
            break;

        case SHA512:
            __hash_function = sha512_hash_buffer;
            __hash_size = 64;
            __input_size = 64; /* FIXME? */
            break;

        default:
            throw std::exception();
    }
}

size_t Hash_Generic::getHashSize(void) const
{
    return __hash_size;
}

size_t Hash_Generic::getInputSize(void) const
{
    return __input_size;
}

unsigned char * Hash_Generic::getHash(const unsigned char * source, size_t source_size,
                     unsigned char * buffer, size_t buffer_size) const
{
    int ss = source_size,
        bs = buffer_size;

    // FIX sha512.c: const functions
    __hash_function((unsigned char *) source, ss,
                    buffer, bs);

    return buffer;
}

Hash::Hash(Hash_Generic::Hash_Type type)
    :Hash_Generic(type) {}

ByteSeq Hash::operator() (const ByteSeq & source) const
{
    /* TODO: Make this function friendly to Octet class */
    /* TODO: Count const payment with macros */

    unsigned char buffer[128]; // const payment

    getHash(source.getData(), source.getDataSize(),
            buffer, sizeof(buffer));

    return ByteSeq(buffer, getHashSize());
}

ByteSeq Hash::operator() (const ManagedBlob & source) const
{
    unsigned char buffer[128]; // const payment

    getHash(source.getData(), source.getDataSize(),
            buffer, sizeof(buffer));

    return ByteSeq(buffer, getHashSize());
}
