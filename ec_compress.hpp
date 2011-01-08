#pragma once

/* Compressed EC Points. For data transmission.  */

#include "ec.hpp"

class EC_CPoint
{
    typedef long Sign;
    
    const ZZ X;
    const Sign tY;

public:
    
    static size_t serializeSize(const unsigned char * data, size_t size);
    
    static inline ZZ deserialize_X(const unsigned char * data, size_t size)
        {
            return ZZFromBytes(data+1, serializeSize(data, size));
        }
    
    static inline unsigned char deserialize_tY(const unsigned char * data, size_t size)
        {
            if (size < 1) return 0; // throw ?
            return ( data[0] & 0x1 );
        }
    
    static inline unsigned char compress_tY(const EC_Point & X)
        {
            return (unsigned char) IsOdd(rep(X.getY()));
        }
    
public:
    EC_CPoint(const EC_Point & Point);
    EC_CPoint(const ZZ_p & X, unsigned char tY);
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

    
