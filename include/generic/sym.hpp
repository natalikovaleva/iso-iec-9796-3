#pragma once

#include "generic/octet.hpp"

class Sym
{
protected:
    inline Sym() {}
public:
    enum DIRECTION
    {
        ENCRYPT,
        DECRYPT
    };
    
public:
    virtual inline ~Sym() {}
    inline ByteSeq operator() (const ByteSeq & data,
                               const ByteSeq & key,
                               DIRECTION direction) const
        {
            if (direction == ENCRYPT)
                return encrypt(data, key);
            else
                return decrypt(data, key);
        }

    virtual ByteSeq encrypt(const ByteSeq & data,
                            const ByteSeq & key) const = 0;
    virtual ByteSeq decrypt(const ByteSeq & data,
                            const ByteSeq & key) const = 0;
    virtual size_t getKeySize() const = 0;
    
};

class SymXor : public Sym
{
    const size_t block_size;
public:
    inline SymXor(size_t block_size)
        : block_size(block_size)
        {}
    
    inline ByteSeq encrypt (const ByteSeq & data,
                            const ByteSeq & key) const
        { return data ^ key; }
    
    inline ByteSeq decrypt (const ByteSeq & data,
                            const ByteSeq & key) const
        { return data ^ key; }
    
    inline size_t getKeySize() const { return block_size; }
};
