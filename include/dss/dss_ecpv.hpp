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
#include "generic/kdf.hpp"
#include "generic/sym.hpp"

#include "generic/zz_utils.hpp"

#include "dss/datain.hpp"

// DEBUG
#include <iostream>
using namespace std;

/* ---------------- ISO/IEC ECPV DSS ------------------ */

template <class cEC,
          class cECP>

class ECPV : public DigitalSignatureWithRecovery
{
    class ECPVKDF : public KDF
    {
        const size_t L_key;
        const MGF MGF2;
    public:
        ECPVKDF(Hash::Hash_Type Hash_type, size_t L_key)
            : L_key(L_key),
              MGF2(MGF::MGF2, Hash_type) {};

        inline ByteSeq operator() (const ByteSeq & data) const
            {
                return MGF2(data, L_key);
            }
    };

    const ECPVKDF _KDF;
    const Hash _Hash;
    const Sym & _Sym;
        
    cEC _Curve;
    const size_t _Ln;
        
    ZZ   _privateKey;
    cECP _publicKey;

    TDataInput<ECPV_Input> _ECPV_Data;

    bool _isPrivateKeyLoaded;
    bool _isPublicKeyLoaded;
    bool _isReadyToSign;
    bool _isReadyToVerify;

    generateRandomValueCallback & _PRNG;
    
public:
    ECPV(cEC & Curve,
         generateRandomValueCallback & PRNG,
         const DataInputPolicy & Policy,
         const Sym & Sym,
         long Size,
         Hash::Hash_Type Hash_type = Hash::SHA1)
        : _KDF(Hash_type, Size),
          _Hash(Hash_type),
          _Sym(Sym),
          _Curve(Curve),
          _Ln(L(Curve.getOrder())),
          _publicKey(Curve),
          /* FIXME */
          _ECPV_Data(TDataInput<ECPV_Input>(Policy)),
          _isPrivateKeyLoaded(false),
          _isPublicKeyLoaded(false),
          _isReadyToSign(false),
          _isReadyToVerify(false),
          _PRNG(PRNG)
        {}
    
    ~ECPV()
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
            
            const Octet P  = _KDF(FE2OSP(PP.getX()));

            /* FIX IT */
            const DSSDataInput SignData =
                dip == NULL ?
                _ECPV_Data.createInput(data, P) :
                TDataInput<ECPV_Input>(*dip).createInput(data, P);

            _Curve.enter_mod_context(cEC::ORDER_CONTEXT);
            
            const Octet r = _Sym(SignData.d, P, Sym::ENCRYPT);
            const Octet u = _Hash(r || SignData.M_clr);
            const ZZ_p  t = InMod(OS2IP(u));
            const ZZ_p  s = (InMod(k) - InMod(_privateKey)*t);

            _Curve.leave_mod_context();
            
            const ByteSeq S = I2OSP(s,_Ln);
            
            return DigitalSignature(r, S, SignData.M_clr);
        }
    
    VerificationVerdict verify(const DigitalSignature & data, const DataInputPolicy * dip = NULL)
    {
        const ZZ s  = OS2IP(data.S);

        if (s >= _Curve.getOrder())
            return VerificationVerdict();

        const Octet u = _Hash(data.R || data.M_clr);
        const ZZ t = OS2IP(u) % _Curve.getOrder();

        if ( IsZero(t) )
            return VerificationVerdict();
                
        _Curve.enter_mod_context(cEC::FIELD_CONTEXT);
        cECP R = _publicKey * t + _Curve.getBasePoint() * s;
        _Curve.leave_mod_context();
                    
        const Octet P  = _KDF(FE2OSP(R.getX()));
        
        const Octet vdata = _Sym(data.R, P, Sym::DECRYPT);
        
        /* MAKE CHECKS */
        const  DSSDataInput vmsg =
            dip == NULL ?
            _ECPV_Data.createOutput(vdata, P) :
            TDataInput<ECPV_Input>(*dip).createOutput(vdata, P);

        const Octet M = vmsg.d || data.M_clr;
                
        const  DSSDataInput vsign =
            dip == NULL ?
            _ECPV_Data.createInput(M, P) :
            TDataInput<ECPV_Input>(*dip).createInput(M, P);

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
