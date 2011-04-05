#pragma once

#include "dss/datain.hpp"
#include "generic/octet.hpp"
#include "generic/hash.hpp"

#include "generic/octet.hpp"
#include "generic/zz_utils.hpp"
#include "dss/datain.hpp"

class STD4_Input
{
    const DataInputPolicy & _Policy;

public:
    STD4_Input(const DataInputPolicy & Policy)
        : _Policy(Policy)
        {}

    DSSDataInput operator() (const ByteSeq & Message,
                             const ByteSeq & Randomizer) const
        {
            const long L_msg = Message.getDataSize();

            const DataInputHints Hints = _Policy(L_msg);

            const long L_rec = Hints.L_rec;
            const long L_red = Hints.L_red;
            const Hash H = Hints.H;

            const long L_clr = L_msg - L_rec;

            const Octet C_rec = I2OSP(L_rec, 8);
            const Octet C_clr = I2OSP(L_clr, 8);

            const ByteSeq M_rec = ByteSeq(Message.getData(), L_rec);
            const ByteSeq M_clr = ByteSeq(Message.getData() + L_rec,
                                          L_clr);

            const Octet Hash_Input = C_rec || C_clr || M_rec || M_clr || Randomizer;

            const ByteSeq Hash_Token = Truncate(H(Hash_Input), L_red);

            const ByteSeq d = Hash_Token || M_rec;

            return DSSDataInput(d, M_clr);
        }
};

class ECNR_Input
{
    const DataInputPolicy & _Policy;

public:
    ECNR_Input(const DataInputPolicy & Policy)
        : _Policy(Policy)
        {}

    DSSDataInput operator() (const ByteSeq & Message,
                             const ByteSeq & Randomizer) const
        {
            const ByteSeq C = I2OSP(1, 4);

            const long L_msg = Message.getDataSize();

            const DataInputHints Hints = _Policy(L_msg);

            const long L_rec = Hints.L_rec;
            const long L_red = Hints.L_red;
            const Hash H = Hints.H;

            const long L_clr = L_msg - L_rec;

            const Octet C_rec = I2OSP(L_rec, 4);
            const Octet C_clr = I2OSP(L_clr, 4);

            const ByteSeq M_rec = ByteSeq(Message.getData(), L_rec);
            const ByteSeq M_clr = ByteSeq(Message.getData() + L_rec, L_clr);

            const Octet Hash_Input = C_rec || C_clr || M_rec || M_clr || Randomizer || C;

            const ByteSeq Hash_Token = Truncate(H(Hash_Input), L_red);

            const ByteSeq D = Hash_Token || M_rec;

            return DSSDataInput(D, M_clr);
        }
};


class ECMR_Input
{
    const DataInputPolicy & _Policy;

public:
    ECMR_Input(const DataInputPolicy & Policy)
        : _Policy(Policy)
        {}

    DSSDataInput operator() (const ByteSeq & Message,
                             const ByteSeq & Randomizer) const
        {
            const long L_msg = Message.getDataSize();

            const DataInputHints Hints = _Policy(L_msg);

            const long L_rec = Hints.L_rec;
            const long L_red = Hints.L_red;
            const Hash H = Hints.H;

            const long L_clr = L_msg - L_rec;

            const ByteSeq M_rec = ByteSeq(Message.getData(), L_rec);
            const ByteSeq M_clr = ByteSeq(Message.getData() + L_rec,
                                          L_clr);

            const Octet h = Truncate(H(Randomizer || Message), L_red);

            const Octet d = h || M_rec;

            return DSSDataInput(d, M_clr);
        }
};

class ECAO_Input
{
    const DataInputPolicy & _Policy;

public:
    ECAO_Input(const DataInputPolicy & Policy)
        : _Policy(Policy)
        {}

    DSSDataInput operator() (const ByteSeq & Message,
                             const ByteSeq & Randomizer
                             __attribute__ ((unused))) const
        {
            const long L_msg = Message.getDataSize();

            const DataInputHints Hints = _Policy(L_msg);

            const long L_rec = Hints.L_rec;
            const long L_red = Hints.L_red;
            const Hash H = Hints.H;

            const long L_clr = L_msg - L_rec;

            const ByteSeq M_rec = ByteSeq(Message.getData(), L_rec);
            const ByteSeq M_clr = ByteSeq(Message.getData() + L_rec,
                                          L_clr);

            const Octet pad = I2OSP(1, Hints.L_max - L_red + 1 - L_rec);
            const Octet M_pad = pad || M_rec;

            const Octet h = Truncate(H(M_pad), L_red);
            const Octet d = h || (Truncate(H(h),
                                           Hints.L_max + 1 - L_red) ^ M_pad);

            return DSSDataInput(d, M_clr);
        }
};

class ECPV_Input
{
    const DataInputPolicy & _Policy;

public:
    ECPV_Input(const DataInputPolicy & Policy)
        : _Policy(Policy)
        {}

    DSSDataInput operator() (const ByteSeq & Message,
                             const ByteSeq & Randomizer
                             __attribute__ ((unused))) const
        {
            const long L_msg = Message.getDataSize();

            const DataInputHints Hints = _Policy(L_msg);

            const long L_rec = Hints.L_rec - 2 /* Size of DER Pad */;
            const long L_red = Hints.L_red;

            const long L_clr = L_msg - L_rec;

            const Octet DERPrintableString = I2OSP(0x13);
            const Octet DERSize = I2OSP(L_rec);

            const Octet M_rec =
                DERPrintableString ||
                DERSize ||
                Truncate(Message, L_rec);

            const ByteSeq M_clr = ByteSeq(Message.getData() + L_rec,
                                          L_clr);

            const Octet C_red_ = I2OSP(L_red);
            Octet C_red;

            for (int i = 0; i<L_red; i++)
                C_red = C_red || C_red_ ;

            const Octet d = C_red || M_rec;

            return DSSDataInput(d, M_clr);
        }
};
