#pragma once

#include <string>
#include <exception>
#include "generic/octet.hpp"
#include "generic/blob.hpp"
#include "generic/hash.hpp"

// DEBUG
#include <iostream>

struct DataInputHints
{
    const unsigned long L_rec;
    const unsigned long L_red;
    const unsigned long L_max;
    const unsigned long L_add; /* Recovering size hint */
    const Hash::Hash_Type H;

    inline DataInputHints(const unsigned long L_rec,
                          const unsigned long L_red,
                          const unsigned long L_max,
                          const Hash::Hash_Type H)
        : L_rec(L_rec),
          L_red(L_red),
          L_max(L_max),
          L_add(0),
          H(H)
        {}

    inline DataInputHints(const DataInputHints & Hints,
                          unsigned long L_add)
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
    const ManagedBlob & M_clr;
    const bool  invalid;

    inline DSSDataInput(const Octet & d,
                        const ManagedBlob & M_clr,
                        bool invalid = false)
        : d(d),
          M_clr(M_clr),
          invalid(invalid)
        {}

    inline DSSDataInput()
        : d(Octet()),
          M_clr(ManagedBlob()),
          invalid(true)
        {}
};

struct DSSDataOutput
{
    const Octet M_rec;
    const Octet d_pad;
    const bool  invalid;

    inline DSSDataOutput(const Octet & M_rec,
                         const Octet & d_pad,
                         bool invalid = false)
        : M_rec(M_rec),
          d_pad(d_pad),
          invalid(invalid)
        {}

    inline DSSDataOutput()
        : M_rec(Octet()),
          d_pad(Octet()),
          invalid(true)
        {}
};


class DataInputPolicy
{
protected:
    DataInputPolicy() {}

public:
    virtual ~DataInputPolicy() {}
    virtual DataInputHints getHints(unsigned long L_msg) const = 0;
    virtual DataInputHints getParseHints(const ManagedBlob & data) const = 0;

    inline DataInputHints operator() (unsigned long L_msg) const
        { return getHints(L_msg); };

    inline DataInputHints operator() (const ManagedBlob & data) const
        { return getParseHints(data); };
};

class StaticDataInputPolicy : public DataInputPolicy
{
protected:
    const DataInputHints _staticHints;

public:
    StaticDataInputPolicy(unsigned long L_rec, unsigned long L_red, unsigned long L_max,
                          Hash::Hash_Type Hash_type)
        : _staticHints(DataInputHints(L_rec, L_red, L_max, Hash_type))
        {}
    inline DataInputHints getHints(unsigned long L_msg) const
        {
            if (L_msg < _staticHints.L_rec)
            {
                std::cout << L_msg << " < " << _staticHints.L_rec << std::endl;
                throw std::exception();
            }

            return _staticHints;
        }

    inline DataInputHints getParseHints(const ManagedBlob & data) const
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
    const DataInputPolicy & _Policy;

public:
    inline DataInput(const DataInputPolicy & Policy)
        : _Policy(Policy)
        {}

    virtual inline ~DataInput() {}

    virtual DSSDataInput createInput (const ManagedBlob & Message,
                                      const ByteSeq & Randomizer) const = 0;

    virtual DSSDataInput createInput (const std::string & Message,
                                      const ByteSeq & Randomizer) const = 0;

    virtual DSSDataInput createInput (const char * Message,
                                      const ByteSeq & Randomizer) const = 0;

    virtual DSSDataOutput createOutput (const ByteSeq & Message) const = 0;

    virtual DSSDataOutput createOutput (const std::string & Message) const = 0;

    virtual DSSDataOutput createOutput (const char * Message) const = 0;
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


    inline DSSDataInput createInput (const ManagedBlob & Message,
                                     const ByteSeq & Randomizer) const
        { return logic.input(Message, Randomizer); }

    inline DSSDataOutput createOutput (const std::string & Message) const
        { return createOutput(ByteSeq(Message.c_str(), Message.length())); }


    inline DSSDataOutput createOutput (const char * Message) const
        { return createOutput(ByteSeq(Message, strlen(Message))); }


    inline DSSDataOutput createOutput (const ByteSeq & Message) const
        { return logic.output(Message); }
};
