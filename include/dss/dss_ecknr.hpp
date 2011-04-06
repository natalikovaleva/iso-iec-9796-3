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
#include "generic/mgf.hpp"

#include "generic/zz_utils.hpp"

#include "dss/datain.hpp"

// DEBUG
#include <iostream>
using namespace std;

/* ---------------- ISO/IEC ECKNR DSS ------------------ */

template <class cEC,
          class cECP>

class ECKNR : public DigitalSignatureWithRecovery
{
    const Hash _Hash;
    const MGF  _MGF;
    
    cEC _Curve;
    const size_t _Ln;
        
    ZZ   _privateKey;
    cECP _publicKey;

    Octet _CertData;
    
    TDataInput<STD4_Input> _ECKNR_Data;

    bool _isPrivateKeyLoaded;
    bool _isPublicKeyLoaded;
    bool _isReadyToSign;
    bool _isReadyToVerify;

    generateRandomValueCallback & _PRNG;
    
public:
    ECKNR(cEC & Curve,
          generateRandomValueCallback & PRNG,
          const DataInputPolicy & Policy,
          Hash::Hash_Type Hash_Type = Hash::RIPEMD160,
          MGF::MGF_Type MGF_Type = MGF::MGF2)
        : _Hash(Hash_Type),
          _MGF(MGF_Type, Hash_Type),
          _Curve(Curve),
          _Ln(L(Curve.getOrder())),
          _publicKey(Curve),
          /* FIXME */
          _ECKNR_Data(TDataInput<STD4_Input>(Policy)),
          _isPrivateKeyLoaded(false),
          _isPublicKeyLoaded(false),
          _isReadyToSign(false),
          _isReadyToVerify(false),
          _PRNG(PRNG)
        {}
    
    ~ECKNR()
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

            genCertData();
        }
    
    Octet generatePublicKey()
        {
            if (! _isPrivateKeyLoaded)
                throw; // Operation unaviable
            
            const ZZ e = InvMod(_privateKey, _Curve.getOrder());

            _Curve.enter_mod_context(cEC::FIELD_CONTEXT);
            _publicKey = _Curve.getBasePoint() * e;
            _Curve.leave_mod_context();

            genCertData();

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
            const cECP PP = _publicKey * k;
            _Curve.leave_mod_context();
            
            const Octet Za = Truncate(_CertData, _Hash.getInputSize());

            const Octet P  = _MGF(EC2OSP(PP, cEC::EC2OSP_COMPRESSED), _Ln);
            
            _Curve.enter_mod_context(cEC::ORDER_CONTEXT);

            /* FIX IT */
            const DSSDataInput SignData =
                dip == NULL ?
                _ECKNR_Data.createInput(data, P) :
                TDataInput<STD4_Input>(*dip).createInput(data, P);
            
            const Octet m = _MGF(Za || SignData.M_clr, _Ln);
            
            const Octet r = SignData.d ^ P ^ m;
            const ZZ_p  t = InMod(OS2IP(r));
            const ZZ_p  s = (InMod(k) - InMod(_privateKey)*t);
            
            const Octet S = I2OSP(s);
            
            return DigitalSignature(r, S, SignData.M_clr);
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
        cECP R = _publicKey * s + _Curve.getBasePoint() * t;
        _Curve.leave_mod_context();
                    
        const Octet P  = _MGF(EC2OSP(R, cEC::EC2OSP_COMPRESSED), _Ln);
        const Octet Za = Truncate(_CertData, _Hash.getInputSize());
        const Octet m = _MGF(Za || data.M_clr, _Ln);

        Octet vdata = data.R ^ P ^ m;

        /* FIX IT ? */
        DataInputHints Hints = _ECKNR_Data._Policy(vdata);

        /* MAKE CHECKS */
        const  DSSDataInput vmsg =
            dip == NULL ?
            _ECKNR_Data.createOutput(vdata, P) :
            TDataInput<STD4_Input>(*dip).createOutput(vdata, P);

        Octet M = vmsg.d || data.M_clr;
        
        const  DSSDataInput vsign =
            dip == NULL ?
            _ECKNR_Data.createInput(M, P) :
            TDataInput<STD4_Input>(*dip).createInput(M, P);

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

private:
    void genCertData()
        {
            _CertData = FE2OSP(_publicKey.getX()) || FE2OSP(_publicKey.getY());
        }
};
