#include "generic/mgf.hpp"
#include "generic/zz_utils.hpp"

using namespace NTL;

MGF::MGF(MGF_Type type, const Hash & Hash)
    : _Hash(Hash), _Type(type)
{}

MGF::MGF(MGF_Type type, Hash::Hash_Type Hash_type)
    : _Hash(Hash_type), _Type(type)
{}

ByteSeq MGF::getMGF1(const ByteSeq & source, const size_t octets_count) const
{
    const long hash_iterations =
        (long)   (octets_count + 1) / _Hash.getHashSize()
        + (long) ((octets_count + 1) % _Hash.getHashSize() ? 1 : 0) ;

    ByteSeq result = _Hash(source || I2OSP(0, 4));

    for ( long k = 1; k <= hash_iterations - 1; k ++)
    {
        result = result || _Hash(source || I2OSP(k, 4));
    }

    return Truncate(result, octets_count);
}

ByteSeq MGF::getMGF2(const ByteSeq & source, const size_t octets_count) const
{
    const long hash_iterations = (long) (octets_count + 1) / _Hash.getHashSize();

    ByteSeq result = _Hash(source || I2OSP(1, 4));

    for ( long k = 2; k <= hash_iterations; k ++)
    {
        result = result || _Hash(source || I2OSP(k, 4));
    }

    return Truncate(result, octets_count);
}

