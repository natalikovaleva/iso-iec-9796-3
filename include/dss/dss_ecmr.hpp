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

/* ---------------- ISO/IEC ECMR DSS ------------------ */

template <class cEC,
          class cECP>

class ECMR : public DigitalSignatureWithRecovery
{
    cEC _Curve;
    const size_t _Ln;

    MGF  _MGF;
    
    ZZ   _privateKey;
    cECP _publicKey;

    TDataInput<ECMR_Input> _ECMR_Data;

    bool _isPrivateKeyLoaded;
    bool _isPublicKeyLoaded;
    bool _isReadyToSign;
    bool _isReadyToVerify;

    generateRandomValueCallback & _PRNG;
    
public:
    ECMR(cEC & Curve,
         generateRandomValueCallback & PRNG,
         const DataInputPolicy & Policy,
         Hash::Hash_Type Hash_type = Hash::SHA1,
         MGF::MGF_Type MGF_type = MGF::MGF1)
        : _Curve(Curve),
          _Ln(L(Curve.getOrder())),
          _MGF(MGF_type, Hash_type),
          _publicKey(Curve),
          /* FIXME */
          _ECMR_Data(TDataInput<ECMR_Input>(Policy)),
          _isPrivateKeyLoaded(false),
          _isPublicKeyLoaded(false),
          _isReadyToSign(false),
          _isReadyToVerify(false),
          _PRNG(PRNG)
        {}
    
    ~ECMR()
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

            const Octet P = _MGF(EC2OSP(PP, cEC::EC2OSP_UNCOMPRESSED), _Ln);
            
            /* FIX IT */
            const DSSDataInput SignData =
                dip == NULL ?
                _ECMR_Data.createInput(data, P) :
                TDataInput<ECMR_Input>(*dip).createInput(data, P);

            _Curve.enter_mod_context(cEC::ORDER_CONTEXT);

            const Octet r = SignData.d ^ P;
            const ZZ_p s = InMod(OS2IP(r) * k - OS2IP(r) - 1) / InMod(_privateKey + 1);
            
            _Curve.leave_mod_context();
            
            const ByteSeq S = I2OSP(s,_Ln);
            
            return DigitalSignature(r, S, SignData.M_clr);
        }
    
    VerificationVerdict verify(const DigitalSignature & data, const DataInputPolicy * dip = NULL)
    {
        
        ZZ t = OS2IP(data.R);
        ZZ s = OS2IP(data.S);
        
        if ((L(t) > L(_Curve.getOrder()) ||
             (IsZero(t))) ||
            (s > _Curve.getOrder()))
            return VerificationVerdict();

        ZZ u1, u2;

        {
            _Curve.enter_mod_context(cEC::ORDER_CONTEXT);
            
            const ZZ_p rP = InMod(t);
            const ZZ_p sP = InMod(s);

            u1 = rep((1 + rP + sP) /rP);
            u2 = rep(sP/rP); 
            
            _Curve.leave_mod_context();
        }
        
        
        _Curve.enter_mod_context(cEC::FIELD_CONTEXT);
        cECP R = _publicKey * u2 + _Curve.getBasePoint() * u1;
        _Curve.leave_mod_context();
        
        const Octet P  = _MGF(EC2OSP(R, cEC::EC2OSP_UNCOMPRESSED), _Ln);
        const Octet vdata =  data.R ^ P;
        
        /* FIX IT ? */
        DataInputHints Hints = _ECMR_Data._Policy(vdata);

        /* MAKE CHECKS */
        const  DSSDataInput vmsg =
            dip == NULL ?
            _ECMR_Data.createOutput(vdata, P) :
            TDataInput<ECMR_Input>(*dip).createOutput(vdata, P);

        Octet M = vmsg.d || data.M_clr;
        
        const  DSSDataInput vsign =
            dip == NULL ?
            _ECMR_Data.createInput(M, P) :
            TDataInput<ECMR_Input>(*dip).createInput(M, P);

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
