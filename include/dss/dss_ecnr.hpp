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

/* ---------------- ISO/IEC ECNR DSS ------------------ */

template <class cEC,
          class cECP>

class ECNR : public DigitalSignatureWithRecovery
{
    cEC _Curve;
    const size_t _Ln;
        
    ZZ   _privateKey;
    cECP _publicKey;

    TDataInput<ECNR_Input> _ECNR_Data;

    bool _isPrivateKeyLoaded;
    bool _isPublicKeyLoaded;
    bool _isReadyToSign;
    bool _isReadyToVerify;

    generateRandomValueCallback & _PRNG;
    
public:
    ECNR(cEC & Curve,
         generateRandomValueCallback & PRNG,
         const DataInputPolicy & Policy)
        : _Curve(Curve),
          _Ln(L(Curve.getOrder())),
          _publicKey(Curve),
          /* FIXME */
          _ECNR_Data(TDataInput<ECNR_Input>(Policy)),
          _isPrivateKeyLoaded(false),
          _isPublicKeyLoaded(false),
          _isReadyToSign(false),
          _isReadyToVerify(false),
          _PRNG(PRNG)
        {}
    
    ~ECNR()
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

            _Curve.enter_mod_context(cEC::FIELD_CONTEXT);
            const cECP PP = _Curve.getBasePoint() * k;
            _Curve.leave_mod_context();
            
            const Octet P  = EC2OSP(PP, cEC::EC2OSP_COMPRESSED);
            
            /* FIX IT */
            const DSSDataInput SignData =
                dip == NULL ?
                _ECNR_Data.createInput(data, P) :
                TDataInput<ECNR_Input>(*dip).createInput(data, P);

            _Curve.enter_mod_context(cEC::ORDER_CONTEXT);

            const ZZ_p pi = InMod(OS2IP(P));
            const ZZ_p d  = InMod(OS2IP(SignData.d));

            const ZZ_p r = (d + pi);
            const ZZ_p s = (InMod(k) - InMod(_privateKey)*r);

            _Curve.leave_mod_context();
            
            const ByteSeq R = I2OSP(r,_Ln);
            const ByteSeq S = I2OSP(s,_Ln);
            
            return DigitalSignature(R, S, SignData.M_clr);
        }
    
    VerificationVerdict verify(const DigitalSignature & data, const DataInputPolicy * dip = NULL)
    {
        ZZ t = OS2IP(data.R);
        ZZ s  = OS2IP(data.S);

        if ((L(t) > L(_Curve.getOrder()) ||
            (IsZero(t))) ||
            (s > _Curve.getOrder()))
            return VerificationVerdict();
        
        t %= _Curve.getOrder();

        _Curve.enter_mod_context(cEC::FIELD_CONTEXT);
        cECP R = _publicKey * t + _Curve.getBasePoint() * s;
        _Curve.leave_mod_context();
                    
        const Octet P  = EC2OSP(R, cEC::EC2OSP_COMPRESSED);

        _Curve.enter_mod_context(cEC::ORDER_CONTEXT);
        const ZZ_p d = InMod(t) - InMod(OS2IP(P));
        _Curve.leave_mod_context();

        Octet vdata = I2OSP(d);

        /* FIX IT ? */
        DataInputHints Hints = _ECNR_Data._Policy(vdata);

        /* MAKE CHECKS */
        const  DSSDataInput vmsg =
            dip == NULL ?
            _ECNR_Data.createOutput(vdata, P) :
            TDataInput<ECNR_Input>(*dip).createOutput(vdata, P);

        Octet M = vmsg.d || data.M_clr;
                
        const  DSSDataInput vsign =
            dip == NULL ?
            _ECNR_Data.createInput(M, P) :
            TDataInput<ECNR_Input>(*dip).createInput(M, P);

        if (vsign.d == vdata)
        {
            return VerificationVerdict(M);
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
