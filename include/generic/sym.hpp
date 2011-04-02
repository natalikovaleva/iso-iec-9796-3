#pragma once

#include "generic/octet.hpp"

class Sym
{
protected:
    inline Sym() {}
public:
    virtual inline ~Sym() {}
    virtual inline ByteSeq operator() (const ByteSeq & data,
                                       const ByteSeq & key) const = 0;
    virtual inline size_t getKeySize() = 0;
};

class SymXor : public Sym
{
    const size_t block_size;
public:
    inline SymXor(size_t block_size)
        : block_size(block_size)
        {}
    
    inline ByteSeq operator() (const ByteSeq & data,
                               const ByteSeq & key) const
        { return data ^ key; }
    
    inline size_t getKeySize(){ return block_size; }
};
