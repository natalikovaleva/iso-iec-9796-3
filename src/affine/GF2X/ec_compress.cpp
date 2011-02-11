#include "NTL/GF2X.h"

#include "affine/GF2X/ec.hpp"
#include "affine/GF2X/ec_compress.hpp"
#include "affine/GF2X/ec_defaults.hpp"
#include "affine/GF2X/utils.hpp"

using namespace NTL;
using namespace Affine::GF2X;

EC_CPoint::~EC_CPoint()
{}

EC_CPoint::EC_CPoint(const EC_Point & Point)
    : X(Point.getX()), tY(compress_tY(Point))
{}

EC_CPoint::EC_CPoint(const GF2X & X, unsigned char tY)
    : X(X), tY(tY)
{}

EC_CPoint::EC_CPoint(const unsigned char * data, size_t size)
    : X(deserialize_X(data,size)), tY(deserialize_tY(data,size))
{}

EC_Point EC_CPoint::decompress(const EC & EC) const
{
    const GF2XModulus & P = EC.getModulus();
    const GF2X & A = EC.getA();
    const GF2X & B = EC.getB();

    if (IsZero(X))
    {
        const GF2X Y = InvMod(SqrMod(
                                  A,  P),
                                      P);
        return EC.create(X, Y);
    }
    else
    {
        const GF2X X3 = PowerMod(X, 3, P);
        const GF2X X2 = SqrMod(X, P);

        const GF2X a = X3 + MulMod(A,X2,P) + B;
        const GF2X b = MulMod(InvMod(X2, P), a, P);

        const GF2X z = Z2ZB_Solve(b, P);

        const long tZ = rep(coeff(z, 0));

        const GF2X Y = MulMod(z + tY + tZ, X, P);

        return EC.create(X, Y);
    }
}

unsigned char * EC_CPoint::serialize(unsigned char * buffer,
                                     size_t size) const
{
    const size_t reqSize = serializeSize();
    
    if (size < reqSize)
        /* No sence to use broken points */
        return NULL;

    /* 8 | Modulus, no way to inject EC info to zeros */
    
    /* We can use 16 precalculated EC's (4 bit),
     * and reserve 3 bits for some usefull  */
    
    const unsigned char Metadata =
          (tY & 1)
        | ((EC_Defaults::getId(EC_Defaults::EC0) & 0xF) << 1);
    // 16 curves. For this time

    BytesFromGF2X(buffer+1, X, reqSize-1);
    buffer[0] = Metadata;

    return buffer;
}

unsigned char * EC_CPoint::serialize(unsigned char * buffer,
                                     size_t size, const EC & EC) const
{
    if (serialize(buffer, size) == NULL)
        return NULL;

    /* Add default EC info */
    const unsigned char Metadata = buffer[0];
    
    buffer[0] =
        ( EC_Defaults::getId(EC_Defaults::toSizes(NumBits(EC.getModulus()))) << 1 )
        | ( Metadata & 0x1 );

    return buffer;
    
}

size_t EC_CPoint::serializeSize(const unsigned char * data, size_t size)
{
    if (size < 1) return 0;

    const unsigned int IncludedSizeID = (data[0] >> 1) & 0xF;
    const EC_Defaults::Sizes Size = EC_Defaults::getSize(IncludedSizeID);

    if (IncludedSizeID == EC_Defaults::EC0)
        return size-1;
    
    return EC_Defaults::toByteSize(Size);
}
