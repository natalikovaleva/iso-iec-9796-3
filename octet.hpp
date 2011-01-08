#pragma once

#include <NTL/ZZ.h>

#include <string.h>

using namespace NTL;

#define OCTET_MAX_SIZE 8196

class ByteSeq
{
    const long __pad;
    unsigned char __data[OCTET_MAX_SIZE];
    size_t __data_size;

public:
    size_t rotateAndPad(unsigned char * dest,
                        size_t dest_size,
                        const unsigned char * source,
                        size_t source_size) const;
protected:
    void setData(const unsigned char * data, size_t data_size);
    void setData(const ZZ & source);

private:
    ByteSeq()
        : __pad(0) {}
    
public:
    ByteSeq(long pad = 0)
        : __pad(pad), __data_size(0) {}

    ByteSeq(const unsigned char * data, size_t data_size, long pad = 0)
        : __pad(pad) 
        { setData(data, data_size); }
    
    ByteSeq(const ZZ & source, long pad = 0)
        : __pad(pad)
        { setData(source); }

    inline ByteSeq(const ByteSeq & source, long pad = 0)
        : __pad(pad)
        {
            __data_size = source.__data_size;
            memcpy(__data, source.__data, source.__data_size);
        }
    
        
    inline const unsigned char * getData() const
        { return __data; }
    
    inline size_t getDataSize() const
        { return __data_size; }

    ZZ getZZ(void) const;
    
    ByteSeq operator|| (const ByteSeq & y) const;

    inline operator ZZ()
        { return getZZ(); }

    friend std::ostream& operator<<(std::ostream& s, const ByteSeq & octet);
};

std::ostream& operator<<(std::ostream& s, const ByteSeq & octet);

/* OCTET Specification */

class Octet : public ByteSeq
{
public:
    
    Octet()
        : ByteSeq(4) {}

    Octet(const unsigned char * data, size_t data_size)
        : ByteSeq(data, data_size, 4) {}
    
    Octet(const ZZ & source)
        : ByteSeq(source, 4) {}

    Octet(const Octet & source)
        : ByteSeq(source, 4) {}

    Octet(const ByteSeq & source)
        : ByteSeq(source, 4) {}
};

