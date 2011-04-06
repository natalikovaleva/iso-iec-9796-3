#pragma once

#include "dss/dss.hpp"
#include "dss/datain_isoiec9796-3.hpp"

/* Include all aviable basies */
#include "ec/ZZ_p/affine/ec.hpp"
#include "ec/ZZ_p/affine/ec_compress.hpp"
#include "ec/ZZ_p/affine/ec_defaults.hpp"
#include "ec/ZZ_p/affine/utils.hpp"

#include "ec/ZZ_p/projective/ec.hpp"

#include "ec/GF2X/affine/ec.hpp"
#include "ec/GF2X/affine/ec_compress.hpp"
#include "ec/GF2X/affine/ec_defaults.hpp"
#include "ec/GF2X/affine/utils.hpp"

#include "ec/GF2X/projective/ec.hpp"
/* -------------------------- */

#include "generic/octet.hpp"
#include "generic/hash.hpp"

#include "generic/zz_utils.hpp"

#include "dss/datain.hpp"

// DEBUG
#include <iostream>
using namespace std;

/* ---------------- ISO/IEC ECAO DSS ------------------ */

template <class cEC,
          class cECP>

class ECAO : public DigitalSignatureWithRecovery
{
    cEC _Curve;
    const size_t _Ln;

    MGF  _MGF;

    ZZ   _privateKey;
    cECP _publicKey;

    TDataInput<ECAO_Input> _ECAO_Data;

    bool _isPrivateKeyLoaded;
    bool _isPublicKeyLoaded;
    bool _isReadyToSign;
    bool _isReadyToVerify;

    generateRandomValueCallback & _PRNG;

public:
    ECAO(cEC & Curve,
         generateRandomValueCallback & PRNG,
         const DataInputPolicy & Policy,
         Hash::Hash_Type Hash_type = Hash::SHA256,
         MGF::MGF_Type MGF_type = MGF::MGF1)
        : _Curve(Curve),
          _Ln(L(Curve.getOrder())),
          _MGF(MGF_type, Hash_type),
          _publicKey(Curve),
          /* FIXME */
          _ECAO_Data(TDataInput<ECAO_Input>(Policy)),
          _isPrivateKeyLoaded(false),
          _isPublicKeyLoaded(false),
          _isReadyToSign(false),
          _isReadyToVerify(false),
          _PRNG(PRNG)
        {}

    ~ECAO()
        {}

    void setPrivateKey(const Octet & PrivateKey)
        {
            _privateKey = OS2IP(PrivateKey);

            _isPrivateKeyLoaded = true;

            if (_isPublicKeyLoaded)
                _isReadyToSign = true;
        }

    void setPublicKey(const Octet & PublicKey)
        {
            const unsigned long Lcm = L(_Curve.getModulus());

            if (PublicKey.getDataSize() != ( Lcm * 2 ))
                throw;

            _Curve.enter_mod_context(cEC::FIELD_CONTEXT);

            tOS2FEP<cECP> OS2FEP;

            _publicKey = _Curve.create(OS2FEP(ByteSeq(PublicKey.getData(), Lcm)),
                                       OS2FEP(ByteSeq(PublicKey.getData() + Lcm, Lcm)));
            _Curve.leave_mod_context();

            _isPublicKeyLoaded = true;
            if (_isPrivateKeyLoaded)
                _isReadyToSign = true;
            _isReadyToVerify = true;
       }

    Octet generatePublicKey()
        {
            if (! _isPrivateKeyLoaded)
                throw; // Operation unaviable

            _Curve.enter_mod_context(cEC::FIELD_CONTEXT);
            _publicKey = _Curve.getBasePoint() * _privateKey;
            _Curve.leave_mod_context();

            _isPublicKeyLoaded = true;
            _isReadyToVerify   = true;
            _isReadyToSign     = true;

            return FE2OSP(_publicKey.getX(), L(_Curve.getModulus())) ||
                FE2OSP(_publicKey.getY(), L(_Curve.getModulus()));
        }

    DigitalSignature sign(const ByteSeq & data, const DataInputPolicy * dip = NULL)
        {
            if (! _isReadyToSign)
                throw;

            const ZZ k = OS2IP(_PRNG());
            const size_t K = _Ln; // Security parameter

            _Curve.enter_mod_context(cEC::FIELD_CONTEXT);
            const cECP PP = _Curve.getBasePoint() * k;
            _Curve.leave_mod_context();

            const Octet P = EC2OSP(PP, cEC::EC2OSP_COMPRESSED);

            /* FIX IT */
            const DSSDataInput SignData =
                dip == NULL ?
                _ECAO_Data.createInput(data, P) :
                TDataInput<ECAO_Input>(*dip).createInput(data, P);

            _Curve.enter_mod_context(cEC::ORDER_CONTEXT);

            const Octet r = SignData.d ^ P;
            const Octet u = _MGF(r || SignData.M_clr, _Ln + K);
            const ZZ_p  t = InMod(OS2IP(u));
            const ZZ_p  s = (InMod(k) - InMod(_privateKey)*t);
            _Curve.leave_mod_context();

            const ByteSeq S = I2OSP(s,_Ln);

            return DigitalSignature(r, S, SignData.M_clr);
        }

    VerificationVerdict verify(const DigitalSignature & data, const DataInputPolicy * dip = NULL)
    {

        const ZZ t = OS2IP(data.R);
        const ZZ s = OS2IP(data.S);

        if ((L(t) != L(_Curve.getOrder()) + 1) ||
             (IsZero(t)) ||
            (s > _Curve.getOrder()))
        {
            return VerificationVerdict();
        }

        const size_t K = _Ln; // Security parameter
        const ZZ u = OS2IP(_MGF(data.R || data.M_clr, _Ln + K)) % _Curve.getOrder();
        
        if (IsZero(u))
        {
            return VerificationVerdict();
        }
        
        _Curve.enter_mod_context(cEC::FIELD_CONTEXT);
        const cECP R = _publicKey * u + _Curve.getBasePoint() * s;
        _Curve.leave_mod_context();
        
        const Octet P  = EC2OSP(R, cEC::EC2OSP_COMPRESSED);
        const Octet vdata =  data.R ^ P;

        /* FIX IT ? */
        DataInputHints Hints = _ECAO_Data._Policy(vdata);

        /* MAKE CHECKS */
        const  DSSDataInput vmsg =
            dip == NULL ?
            _ECAO_Data.createOutput(vdata, P) :
            TDataInput<ECAO_Input>(*dip).createOutput(vdata, P);

        Octet M = vmsg.d || data.M_clr;
        
        const  DSSDataInput vsign =
            dip == NULL ?
            _ECAO_Data.createInput(M, P) :
            TDataInput<ECAO_Input>(*dip).createInput(M, P);

        if (vsign.d == vdata)
        {
            return VerificationVerdict(vmsg.d);
        }
        else
        {
            return VerificationVerdict();
        }
    }
    
    void buildPrecomputationTables()
        {
            throw; /* NOT IMPLEMENTED YET */
        }
};
