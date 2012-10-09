#pragma once

/* Compressed EC Points. For data transmission.  */

#include "ec.hpp"

namespace ECGF2X
{
    namespace Affine
    {

        using NTL::GF2XFromBytes;
        using NTL::IsZero;
    
        class EC_CPoint
        {
            typedef long Sign;
    
            const GF2X X;
            const Sign tY;

        public:
    
            static size_t serializeSize(const unsigned char * data, size_t size);
    
            static inline GF2X deserialize_X(const unsigned char * data, size_t size)
                {
                    return GF2XFromBytes(data+1, serializeSize(data, size));
                }
    
            static inline unsigned char deserialize_tY(const unsigned char * data, size_t size)
                {
                    if (size < 1) return 0; // throw ?
                    return ( data[0] & 0x1 );
                }
    
            static inline unsigned char compress_tY(const EC_Point & X)
                {
                    if (X.isZero()) return 0;
                    const GF2XModulus & P = X.getEC().getModulus();
                    const GF2X xy = MulMod(X.getX(),InvMod(X.getY(), P), P);
                    return (unsigned char) rep(coeff(xy, 0)) & 0x1;
                }
    
        public:
            EC_CPoint(const EC_Point & Point);
            EC_CPoint(const GF2X & X, unsigned char tY);
            EC_CPoint(const unsigned char * data, size_t size);
            
            ~EC_CPoint();
    
            EC_Point decompress(const EC & EC) const;

            unsigned char * serialize(unsigned char * buffer,
                                      size_t size) const;

            unsigned char * serialize(unsigned char * buffer,
                                      size_t size, const EC & EC) const;
    
            inline size_t serializeSize() const
                { return NumBytes(X) + 1; }
    
    
            bool operator== (EC_CPoint & Point) const;
        };

    
    }
}
