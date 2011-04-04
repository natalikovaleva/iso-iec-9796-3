#pragma once

#include <string>
#include "generic/octet.hpp"
#include "generic/hash.hpp"

class DataInput
{
protected:
    inline DataInput() {}

public:
    struct DSSDataInput
    {
        Octet d;
        Octet M_clr;
    };

public:
    virtual inline ~DataInput() {}

    virtual DSSDataInput createInput (const std::string & Message,
                                      const ByteSeq & Randomizer) const = 0;

    virtual DSSDataInput createInput (const char * Message,
                                      const ByteSeq & Randomizer) const = 0;
};

class DataInputPolitic
{
    const long _L_rec, _L_red, _L_max, _L_clr;
    Hash::Hash_Type _Hash_type;

protected:
    DataInputPolitic(long L_rec, long L_red,
                     Hash::Hash_Type Hash_type,
                     long L_clr,
                     long L_max)
        : _L_rec(L_rec),
          _L_red(L_red),
          _L_max(L_max),
          _L_clr(L_clr),
          _Hash_type(Hash_type)
        {}

public:
    virtual ~DataInputPolitic() {}

    inline virtual long getRecoverableSize() const     { return _L_rec; }
    inline virtual long getCleartextSize() const       { return _L_clr; }
    inline virtual long getRedundancySize() const      { return _L_red; }
    inline virtual long getMaximalRedundancySize()const{ return _L_max; }
    inline virtual Hash::Hash_Type getHashType() const { return _Hash_type; }
};

class StaticDataInputPolitic : public DataInputPolitic
{
public:
    StaticDataInputPolitic(long L_rec, long L_red,
                           Hash::Hash_Type Hash_type,
                           long L_clr = -1,
                           long L_max = -1)
        : DataInputPolitic(L_rec, L_red, Hash_type, L_clr, L_max)
        {}
};


class DataInputProvider
{
    const DataInputPolitic & _dip;

public:
    enum DataInputType
    {
        DATA_ECKNR = 0,
        DATA_ECNR,
        DATA_ECMR,
        DATA_ECAO,
        DATA_ECPV,
        DATA_USER
    };

public:
    DataInputProvider(const DataInputPolitic & dip)
        : _dip (dip)
        {}

    ~DataInputProvider() {};

public:
    DataInput * newDataInput(DataInputType type) const;
};
