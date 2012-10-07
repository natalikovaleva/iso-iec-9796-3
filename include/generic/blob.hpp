#pragma once

#include <malloc.h>
#include <string.h>
#include <sys/types.h>
#include "generic/octet.hpp"

#include <exception>

#define MANAGED_BLOB_STATIC_BUFFER 8196
#define MANAGED_DEFAULT_LPAD 512
#define MANAGED_DEFAULT_RPAD 512

class ManagedBlob
{
    unsigned char * __data;
    size_t __data_size;

    unsigned char __data_buffer[MANAGED_BLOB_STATIC_BUFFER+
                                MANAGED_DEFAULT_LPAD+
                                MANAGED_DEFAULT_RPAD];

    size_t  __lpad;
    size_t  __rpad;

protected:
    void setData(const unsigned char * data,
                 size_t data_size,
                 size_t lpad = 0,
                 size_t rpad = 0)
        {
            if (data_size + lpad + rpad > sizeof(__data_buffer)
                + MANAGED_DEFAULT_LPAD
                + MANAGED_DEFAULT_RPAD)
            {
                __data = (unsigned char *)
                    malloc(data_size + lpad + rpad
                           + MANAGED_DEFAULT_LPAD
                           + MANAGED_DEFAULT_RPAD);
            }
            else
            {
                __data = (unsigned char *) __data_buffer;
            }

            __lpad = MANAGED_DEFAULT_LPAD;
            __rpad = MANAGED_DEFAULT_RPAD;
            __data += __lpad;

            __data_size = data_size + lpad + rpad;

            bzero(__data, data_size + lpad + rpad);
            memcpy(__data + lpad, data, data_size);
        }

public:
    ManagedBlob()
        : __data(NULL),
          __data_size(0),
          __lpad(0),
          __rpad(0)
        {}

    ManagedBlob(const char * data,
                size_t data_size,
                size_t lpad = 0,
                size_t rpad = 0)
        {
            setData((const unsigned char *) data,
                    data_size, lpad, rpad);
        }

    ManagedBlob(const unsigned char * data,
                size_t data_size,
                size_t lpad = 0,
                size_t rpad = 0)
        {
            setData(data, data_size,
                    lpad, rpad);
        }

    ManagedBlob(const ByteSeq & data,
                size_t lpad = 0,
                size_t rpad = 0)
        {
            setData(data.getData(),
                    data.getDataSize(),
                    lpad, rpad);
        }

    ManagedBlob(const unsigned char * data1,
                size_t data1_size,
                const unsigned char * data2,
                size_t data2_size)
        {
            if (data1_size+data2_size > sizeof(__data_buffer)
                + MANAGED_DEFAULT_LPAD
                + MANAGED_DEFAULT_RPAD)
            {
                __data = (unsigned char *)
                    malloc (data1_size+data2_size
                            + MANAGED_DEFAULT_LPAD
                            + MANAGED_DEFAULT_RPAD);
            }
            else
            {
                __data = (unsigned char *) __data_buffer;
            }

            __lpad = MANAGED_DEFAULT_LPAD;
            __rpad = MANAGED_DEFAULT_RPAD;

            __data += __lpad;

            __data_size = data1_size + data2_size;

            memcpy(__data, data1, data1_size);
            memcpy(__data+data1_size, data2, data2_size);
        }

    ManagedBlob (const ManagedBlob & Other)
        {
            setData(Other.getData(), Other.getDataSize());
        }

    ManagedBlob(const ManagedBlob & Other,
                size_t data_size,
                size_t lpad = 0,
                size_t rpad = 0)
        {
            if (data_size == 0)
            {
                setData(Other.getData(),
                        Other.getDataSize(),
                        lpad, rpad);
            }
            else
            {
                if(data_size < Other.getDataSize())
                    setData(Other.getData(),
                            data_size,
                            lpad, rpad);
                else
                    setData(Other.getData(),
                            Other.getDataSize(),
                            lpad,
                            data_size - Other.getDataSize());
            }
        }

    inline const unsigned char * getData() const
        { return (unsigned char *) __data; }

    inline size_t getDataSize() const
        { return __data_size; }

    ~ManagedBlob()
        {
            if ((__data - __lpad) != __data_buffer)
                free (__data - __lpad);
        }

    inline ManagedBlob operator|| (const ByteSeq & y) const
        {
            return ManagedBlob(__data, __data_size,
                               y.getData(), y.getDataSize());
        }

    inline const ManagedBlob & operator>>(size_t size)
        {
            if (size > __lpad)
            {
                unsigned char *
                    reallocated_block = __data - __lpad;

                __data = (unsigned char *)
                    malloc(__data_size
                           + size
                           + MANAGED_DEFAULT_LPAD
                           + MANAGED_DEFAULT_RPAD);

                __lpad = MANAGED_DEFAULT_LPAD;
                __rpad = MANAGED_DEFAULT_RPAD;

                __data += __lpad;
                bzero(__data, __data_size + size);
                memcpy(__data + size, reallocated_block + __lpad, __data_size);

                __data_size += size;

                if (reallocated_block == __data_buffer)
                {
                    free(reallocated_block);
                }
            }
            else
            {
                __data -= size;
                __lpad -= size;
                __data_size += size;

                bzero(__data, size);
            }

            return *this;
        }

    inline const ManagedBlob & operator<<(unsigned long size)
        {
            if (size > __rpad)
            {
                unsigned char *
                    reallocated_block = __data - __lpad;

                __data = (unsigned char *)
                    malloc(__data_size
                           + size
                           + MANAGED_DEFAULT_LPAD
                           + MANAGED_DEFAULT_RPAD);

                __lpad = MANAGED_DEFAULT_LPAD;
                __rpad = MANAGED_DEFAULT_RPAD;

                __data += __lpad;
                bzero(__data, __data_size + size);
                memcpy(__data, reallocated_block + __lpad, __data_size);

                __data_size += size;

                if (reallocated_block == __data_buffer)
                {
                    free(reallocated_block);
                }
            }
            else
            {
                bzero(__data + __data_size, size);

                __rpad -= size;
                __data_size += size;

            }

            return *this;
        }

    friend std::ostream& operator<<(std::ostream& s, const ManagedBlob & octet);

private:
    ManagedBlob operator= (const ManagedBlob & Other __attribute__ ((unused)))
        { throw std::exception(); }

};

std::ostream& operator<<(std::ostream& s, const ManagedBlob & octet);

inline ManagedBlob Truncate(const ManagedBlob & input,
                            const size_t octets)
{
    return ManagedBlob(input, octets);
}

inline Octet Truncate1(const ManagedBlob & input,
                            const size_t octets)
{
    if (octets > OCTET_MAX_SIZE)
        throw std::exception();

    return Octet(input.getData(), input.getDataSize() > octets
                 ? octets : input.getDataSize());
}

