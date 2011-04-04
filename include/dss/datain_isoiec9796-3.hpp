#pragma once

#include "dss/datain.hpp"
#include "generic/octet.hpp"
#include "generic/hash.hpp"
#include <string>

class DataInputDefault : public DataInput
{
protected:

    const long _L_rec;
    const long _L_red;
    const Hash _Hash;

public:
    DataInputDefault(long L_rec, long L_red,
                     Hash::Hash_Type Hash_type)
        : _L_rec(L_rec),
          _L_red(L_red),
          _Hash(Hash_type)
          {}

    inline
    DSSDataInput createInput (const std::string & Message,
                              const ByteSeq & Randomizer) const
    { return createInput(Message.c_str(), Randomizer); }

    virtual DSSDataInput createInput (const char * Message,
                                      const ByteSeq & Randomizer) const = 0;
};

class DataInput_STD4 : public DataInputDefault
{
public:
    DataInput_STD4(long L_rec, long L_red,
                     Hash::Hash_Type Hash_type)
        : DataInputDefault(L_rec, L_red, Hash_type)
          {}

    DSSDataInput createInput (const char * Message,
                              const ByteSeq & Randomizer) const;
};


class DataInput_ECNR : public DataInputDefault
{
public:
    DataInput_ECNR(long L_rec, long L_red,
                   Hash::Hash_Type Hash_type)
        : DataInputDefault(L_rec, L_red, Hash_type)
        {}

    DSSDataInput createInput (const char * Message,
                              const ByteSeq & Randomizer) const;
};

class DataInput_ECMR : public DataInputDefault
{
public:
    DataInput_ECMR(long L_rec, long L_red,
                   Hash::Hash_Type Hash_type)
        : DataInputDefault(L_rec, L_red, Hash_type)
        {}

    DSSDataInput createInput (const char * Message,
                              const ByteSeq & Randomizer) const;
};

class DataInput_ECPV : public DataInputDefault
{
public:
    DataInput_ECPV(long L_rec, long L_red,
                   Hash::Hash_Type Hash_type)
        : DataInputDefault(L_rec, L_red, Hash_type)
        {}

    DSSDataInput createInput (const char * Message,
                              const ByteSeq & Randomizer) const;
};


class DataInput_ECAO : public DataInput
{
protected:

    const long _L_rec;
    const long _L_red;
    const long _L_max;
    const Hash _Hash;

public:
    DataInput_ECAO(long L_rec, long L_red, long L_max,
                   Hash::Hash_Type Hash_type)
        : _L_rec(L_rec),
          _L_red(L_red),
          _L_max(L_max),
          _Hash(Hash_type)
          {}

    inline
    DSSDataInput createInput (const std::string & Message,
                              const ByteSeq & Randomizer) const
    { return createInput(Message.c_str(), Randomizer); }

    DSSDataInput createInput (const char * Message,
                              const ByteSeq & Randomizer) const;
};


