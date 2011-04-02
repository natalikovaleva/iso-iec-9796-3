#pragma once

#include "generic/octet.hpp"

class KDF
{
protected:
    inline KDF() {}
public:
    virtual inline ~KDF() {}
    virtual inline ByteSeq operator() (const ByteSeq & data) const = 0;
};


