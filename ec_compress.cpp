#include "ec_compress.hpp"
#include "ec_defaults.hpp"

EC_CPoint::~EC_CPoint()
{}

EC_CPoint::EC_CPoint(const EC_Point & Point)
    : X(rep(Point.getX())), tY(compress_tY(Point))
{}

EC_CPoint::EC_CPoint(const ZZ_p & X, unsigned char tY)
    : X(rep(X)), tY(tY)
{}

EC_CPoint::EC_CPoint(const unsigned char * data, size_t size)
    : X(deserialize_X(data,size)), tY(deserialize_tY(data,size))
{}

EC_Point EC_CPoint::decompress(const EC & EC) const
{
    const ZZ & A = rep(EC.getA());
    const ZZ & B = rep(EC.getB());
    const ZZ & P = EC.getModulus();

    const ZZ X3 = PowerMod(X, 3, P);

    const ZZ Y =  SqrRootMod((X3 + A*X + B) % P, P);

    return EC.create(X, tY == IsOdd(Y) ? Y : NegateMod(Y,P));
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

    BytesFromZZ(buffer+1, X, reqSize-1);
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
