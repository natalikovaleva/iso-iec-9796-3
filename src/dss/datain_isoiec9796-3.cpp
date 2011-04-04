#include <string>
#include "generic/octet.hpp"
#include "generic/zz_utils.hpp"
#include "dss/datain.hpp"
#include "dss/datain_isoiec9796-3.hpp"

DataInput::DSSDataInput DataInput_STD4::createInput (const char * Message,
                                                     const ByteSeq & Randomizer) const
{

    const long L_msg = strlen(Message);

    /* TODO: Smart message size handling */

    const long L_rec = _L_rec;
    const long L_red = _L_red;

    /* --------------------------------- */

    const long L_clr = L_msg - L_rec;

    const Octet C_rec = I2OSP(L_rec, 8);
    const Octet C_clr = I2OSP(L_clr, 8);

    const ByteSeq M_rec = ByteSeq(Message, L_rec);
    const ByteSeq M_clr = ByteSeq(Message + L_rec,
                                  L_clr);

    Octet Hash_Input = C_rec || C_clr || M_rec || M_clr || Randomizer;

    ByteSeq Hash_Token = Truncate(_Hash(Hash_Input), L_red);

    ByteSeq d = Hash_Token || M_rec;

    DSSDataInput RetVal;

    RetVal.d = d;
    RetVal.M_clr = M_clr;

    return RetVal;
}

DataInput::DSSDataInput DataInput_ECNR::createInput (const char * Message,
                                                     const ByteSeq & Randomizer) const
{
    const ByteSeq C = I2OSP(1, 4);

    const long L_msg = strlen(Message);

    /* TODO: Smart message size handling */

    const long L_rec = _L_rec;
    const long L_red = _L_red;
    // const long L_max = _L_max;

    /* --------------------------------- */

    const long L_clr = L_msg - L_rec;

    const Octet   C_rec = I2OSP(L_rec, 4);
    const Octet   C_clr = I2OSP(L_clr, 4);

    const ByteSeq M_rec = ByteSeq(Message, L_rec);
    const ByteSeq M_clr = ByteSeq(Message + L_rec, L_clr);

    Octet Hash_Input = C_rec || C_clr || M_rec || M_clr || Randomizer || C;

    ByteSeq Hash_Token = Truncate(_Hash(Hash_Input), L_red);

    ByteSeq D = Hash_Token || M_rec;

    DSSDataInput RetVal;

    RetVal.d = D;
    RetVal.M_clr = M_clr;

    return RetVal;
}

DataInput::DSSDataInput DataInput_ECMR::createInput (const char * Message,
                                                     const ByteSeq & Randomizer) const
{
    const long L_msg = strlen(Message);

    /* TODO: Smart message size handling */

    const long L_rec = _L_rec;
    const long L_red = _L_red;
    // const long L_max = _L_max;

    /* --------------------------------- */

    const long L_clr = L_msg - L_rec;

    const ByteSeq M_rec = ByteSeq(Message, L_rec);
    const ByteSeq M_clr = ByteSeq(Message + L_rec,
                                  L_clr);

    const Octet h = Truncate(_Hash(Randomizer || ByteSeq(Message, L_msg)), L_red);

    const Octet d = h || M_rec;

    DSSDataInput RetVal;

    RetVal.d = d;
    RetVal.M_clr = M_clr;

    return RetVal;
}

DataInput::DSSDataInput DataInput_ECAO::createInput (const char * Message,
                                                     const ByteSeq & Randomizer
                                                     __attribute__ ((unused))) const
{
    const long L_msg = strlen(Message);

    /* TODO: Smart message size handling */

    const long L_rec = _L_rec;
    const long L_red = _L_red;

    /* --------------------------------- */

    const long L_clr = L_msg - L_rec;

    if (_L_max == -1)
        throw;

    const ByteSeq M_rec = ByteSeq(Message, L_rec);
    const ByteSeq M_clr = ByteSeq(Message + L_rec,
                                  L_clr);

    const Octet pad = I2OSP(1, _L_max - L_red + 1 - L_rec);
    const Octet M_pad = pad || M_rec;

    const Octet h = Truncate(_Hash(M_pad), L_red);
    const Octet d = h || (Truncate(_Hash(h), _L_max + 1 - L_red) ^ M_pad);

    DSSDataInput RetVal;

    RetVal.d = d;
    RetVal.M_clr = M_clr;

    return RetVal;
}


DataInput::DSSDataInput DataInput_ECPV::createInput (const char * Message,
                                                     const ByteSeq & Randomizer
                                                     __attribute__ ((unused))) const
{
    const long L_msg = strlen(Message);

    /* TODO: Smart message size handling */

    const long L_rec = _L_rec - 2 /* Size of DER Pad */;
    const long L_red = _L_red;

    /* --------------------------------- */

    const long L_clr = L_msg - L_rec;

    const Octet DERPrintableString = I2OSP(0x13);
    const Octet DERSize = I2OSP(L_rec);

    const Octet M_rec =
        DERPrintableString ||
        DERSize ||
        ByteSeq(Message, L_rec);

    const ByteSeq M_clr = ByteSeq(Message + L_rec,
                                  L_clr);

    const Octet C_red_ = I2OSP(L_red);
    Octet C_red;

    for (int i = 0; i<L_red; i++)
        C_red = C_red || C_red_ ;

    const Octet d = C_red || M_rec;

    DSSDataInput RetVal;

    RetVal.d = d;
    RetVal.M_clr = M_clr;

    return RetVal;
}
