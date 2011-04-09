#pragma once

#include <string>
#include "generic/octet.hpp"
#include "generic/hash.hpp"

struct DataInputHints
{
    const long L_rec;
    const long L_red;
    const long L_max;
    const long L_add; /* Recovering size hint */
    const Hash::Hash_Type H;

    inline DataInputHints(const long L_rec,
                          const long L_red,
                          const long L_max,
                          const Hash::Hash_Type H)
        : L_rec(L_rec),
          L_red(L_red),
          L_max(L_max),
          L_add(0),
          H(H)
        {}
    
    inline DataInputHints(const DataInputHints & Hints,
                          long L_add)
        : L_rec(Hints.L_rec),
          L_red(Hints.L_red),
          L_max(Hints.L_max),
          L_add(L_add),
          H(Hints.H)
        {}
            
};

struct DSSDataInput
{
    const Octet d;
    const Octet M_clr;

    inline DSSDataInput(const Octet & d,
                        const Octet & M_clr)
        : d(d),
          M_clr(M_clr)
        {}
};

class DataInputPolicy
{
protected:
    DataInputPolicy() {}

public:
    virtual ~DataInputPolicy() {}
    virtual DataInputHints getHints(long L_msg) const = 0;
    virtual DataInputHints getParseHints(const Octet & data) const = 0;

    inline DataInputHints operator() (long L_msg) const
        { return getHints(L_msg); };

    inline DataInputHints operator() (const Octet & data) const
        { return getParseHints(data); };
};

class StaticDataInputPolicy : public DataInputPolicy
{
protected:
    const DataInputHints _staticHints;

public:
    StaticDataInputPolicy(long L_rec, long L_red, long L_max,
                          Hash::Hash_Type Hash_type)
        : _staticHints(DataInputHints(L_rec, L_red, L_max, Hash_type))
        {}
    inline DataInputHints getHints(long L_msg) const
        {
            if (L_msg < _staticHints.L_rec)
            {
                throw;
            }

            return _staticHints;
        }

    inline DataInputHints getParseHints(const Octet & data) const
        {
            if (data.getDataSize() < (_staticHints.L_rec + _staticHints.L_red))
            {
                return DataInputHints(_staticHints,
                                      (_staticHints.L_rec + _staticHints.L_red) -
                                      data.getDataSize());
            }
            return _staticHints;
        }
};

class DataInput
{
public:
    enum LOGIC
    {
        SIGN_LOGIC,
        VERIFY_LOFIC
    };

    const DataInputPolicy & _Policy;

public:
    inline DataInput(const DataInputPolicy & Policy)
        : _Policy(Policy)
        {}

    virtual inline ~DataInput() {}

    virtual DSSDataInput createInput (const ByteSeq & Message,
                                      const ByteSeq & Randomizer) const = 0;

    virtual DSSDataInput createInput (const std::string & Message,
                                      const ByteSeq & Randomizer) const = 0;

    virtual DSSDataInput createInput (const char * Message,
                                      const ByteSeq & Randomizer) const = 0;

    virtual DSSDataInput createOutput (const ByteSeq & Message,
                                       const ByteSeq & Randomizer) const = 0;

    virtual DSSDataInput createOutput (const std::string & Message,
                                       const ByteSeq & Randomizer) const = 0;

    virtual DSSDataInput createOutput (const char * Message,
                                       const ByteSeq & Randomizer) const = 0;

};

template <class Logic>
class TDataInput : public DataInput
{
    const Logic logic;

public:
    TDataInput(const DataInputPolicy & Policy)
        : DataInput(Policy),
          logic(DataInput::_Policy)
        {}

    inline DSSDataInput createInput (const std::string & Message,
                                      const ByteSeq & Randomizer) const
        { return createInput(ByteSeq(Message.c_str(), Message.length()),
                             Randomizer); }


    inline DSSDataInput createInput (const char * Message,
                                      const ByteSeq & Randomizer) const
        { return createInput(ByteSeq(Message, strlen(Message)),
                             Randomizer); }


    inline DSSDataInput createInput (const ByteSeq & Message,
                                     const ByteSeq & Randomizer) const
        { return logic(Message, Randomizer, SIGN_LOGIC); }

    inline DSSDataInput createOutput (const std::string & Message,
                                      const ByteSeq & Randomizer) const
        { return createOutput(ByteSeq(Message.c_str(), Message.length()),
                              Randomizer); }


    inline DSSDataInput createOutput (const char * Message,
                                      const ByteSeq & Randomizer) const
        { return createOutput(ByteSeq(Message, strlen(Message)),
                              Randomizer); }


    inline DSSDataInput createOutput (const ByteSeq & Message,
                                      const ByteSeq & Randomizer) const
        { return logic(Message, Randomizer, VERIFY_LOFIC); }

};
