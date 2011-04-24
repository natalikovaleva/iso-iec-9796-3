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
                             const ByteSeq & Randomizer,
                             DataInput::LOGIC logic) const
        {
            const unsigned long L_msg = Message.getDataSize();

            if (logic == DataInput::VERIFY_LOFIC)
            {
                const DataInputHints Hints = _Policy.getParseHints(Message);
                
                return DSSDataInput(ByteSeq(Message.getData() +
                                            (Hints.L_red - Hints.L_add),
                                            Message.getDataSize() -
                                            (Hints.L_red - Hints.L_add)),
                                    Hints.L_add == 0 ?
                                    Message :
                                    ByteSeq(Message.getData(),
                                            Message.getDataSize(),
                                            Message.getDataSize() + Hints.L_add)
                                     );
            }

            const DataInputHints Hints = _Policy(L_msg);

            const unsigned long L_rec = Hints.L_rec;
            const unsigned long L_red = Hints.L_red;

            const Hash H = Hints.H;

            const unsigned long L_clr = L_msg - L_rec;

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
                             const ByteSeq & Randomizer,
                             DataInput::LOGIC logic) const
        {
            const ByteSeq C = I2OSP(1, 4);

            const unsigned long L_msg = Message.getDataSize();

            if (logic == DataInput::VERIFY_LOFIC)
            {
                const DataInputHints Hints = _Policy.getParseHints(Message);
                
                return DSSDataInput(ByteSeq(Message.getData() +
                                            (Hints.L_red - Hints.L_add),
                                            Message.getDataSize() -
                                            (Hints.L_red - Hints.L_add)),
                                    Hints.L_add == 0 ?
                                    Message :
                                    ByteSeq(Message.getData(),
                                            Message.getDataSize(),
                                            Message.getDataSize() + Hints.L_add)
                                     );
            }

            const DataInputHints Hints = _Policy(L_msg);
            
            const unsigned long L_rec = Hints.L_rec;
            const unsigned long L_red = Hints.L_red;
            
            const Hash H = Hints.H;

            const unsigned long L_clr = L_msg - L_rec;

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
                             const ByteSeq & Randomizer,
                             DataInput::LOGIC logic) const
        {
            const unsigned long L_msg = Message.getDataSize();

            if (logic == DataInput::VERIFY_LOFIC)
            {
                const DataInputHints Hints = _Policy.getParseHints(Message);
                
                return DSSDataInput(ByteSeq(Message.getData() +
                                            (Hints.L_red - Hints.L_add),
                                            Message.getDataSize() -
                                            (Hints.L_red - Hints.L_add)),
                                    Hints.L_add == 0 ?
                                    Message :
                                    ByteSeq(Message.getData(),
                                            Message.getDataSize(),
                                            Message.getDataSize() + Hints.L_add)
                                     );
            }

            const DataInputHints Hints = _Policy(L_msg);
            
            const unsigned long L_rec = Hints.L_rec;
            const unsigned long L_red = Hints.L_red;

            
            const Hash H = Hints.H;

            const unsigned long L_clr = L_msg - L_rec;

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
                             __attribute__ ((unused)),
                             DataInput::LOGIC logic) const
        {
            const unsigned long L_msg = Message.getDataSize();

            const DataInputHints Hints =
                logic == DataInput::SIGN_LOGIC ?
                _Policy(L_msg) : _Policy(Message);
            
            const unsigned long L_rec = Hints.L_rec;
            const unsigned long L_red = Hints.L_red;
            const Hash H = Hints.H;

            const unsigned long L_clr = L_msg - L_rec;

            if (logic == DataInput::SIGN_LOGIC)
            {
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
            else
            {
                /* Return M_rec via d */
                const Octet PMessage =
                    Hints.L_add == 0 ? Message : ByteSeq(Message.getData(),
                                                         Message.getDataSize(),
                                                         Message.getDataSize() + Hints.L_add);
                
                const Octet h = Truncate(PMessage, L_red);
                const Octet M_h = ByteSeq(PMessage.getData() + L_red,
                                          PMessage.getDataSize() - L_red);
                const Octet M_rec_pad = M_h ^ Truncate(H(h), PMessage.getDataSize() - L_red);

                const unsigned long pad_size = Hints.L_max - L_red + 1 - L_rec;

                const Octet M_rec = ByteSeq(M_rec_pad.getData() + pad_size,
                                            M_rec_pad.getDataSize() - pad_size);

                /* If message have proper padding, then return without padding,
                 * else return as-is. Probably bad behaviour.  */

                if (IsOne(OS2IP(Truncate(M_rec_pad, pad_size))))
                    return DSSDataInput(M_rec, ByteSeq(PMessage.getData(),
                                                       PMessage.getDataSize()));
                else
                {
                    return DSSDataInput();
                }
            }
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
                             __attribute__ ((unused)),
                             DataInput::LOGIC logic) const
        {
            const unsigned long L_msg = Message.getDataSize();

            const DataInputHints Hints =
                logic == DataInput::SIGN_LOGIC ?
                _Policy(L_msg) : _Policy(Message);

            if (logic == DataInput::VERIFY_LOFIC)
            {
                return DSSDataInput(ByteSeq(Message.getData() +
                                            (Hints.L_red - Hints.L_add),
                                            Message.getDataSize() -
                                            (Hints.L_red - Hints.L_add)),
                                    Hints.L_add == 0 ?
                                    Message :
                                    ByteSeq(Message.getData(),
                                            Message.getDataSize(),
                                            Message.getDataSize() + Hints.L_add)
                                     );
            }

            const unsigned long L_rec = Hints.L_rec;
            const unsigned long L_red = Hints.L_red - 2;

            const unsigned long L_clr = L_msg - L_rec;

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

            for (unsigned int i = 0; i<L_red; i++)
                C_red = C_red || C_red_ ;

            const Octet d = C_red || M_rec;

            return DSSDataInput(d, M_clr);
        }
};
