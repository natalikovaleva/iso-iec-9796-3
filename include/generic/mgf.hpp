#pragma once

#include "generic/octet.hpp"
#include "generic/hash.hpp"

class MGF
{
public:
    enum MGF_Type
    {
        MGF1,
        MGF2
    };

private:
    const Hash _Hash;
    MGF_Type   _Type;

    ByteSeq getMGF1(const ByteSeq & source, const size_t octets_count) const;
    ByteSeq getMGF2(const ByteSeq & source, const size_t octets_count) const;

public:
    MGF(MGF_Type type, const class Hash & Hash);
    MGF(MGF_Type type, Hash::Hash_Type Hash_Type);

    inline ByteSeq operator() (const ByteSeq & source, const size_t octets_count) const
        {
            switch(_Type)
            {
                case MGF1: return getMGF1(source, octets_count);
                case MGF2: return getMGF2(source, octets_count);
                default: return ByteSeq(0); //trow ??
            }
        }
};
