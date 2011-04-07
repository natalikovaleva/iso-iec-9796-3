#pragma once

/* Field convertor helper */

#include "generic/octet.hpp"

#define OS2FEP_TPL
template <class ECP>
struct tOS2FEP
{
    inline typename ECP::FE operator()(const ByteSeq & From);
};

#include "ec/ZZ_p/dss_tpl.hpp"
#include "ec/GF2X/dss_tpl.hpp"

#include "dss/dss_ecknr.hpp"
#include "dss/dss_ecnr.hpp"
#include "dss/dss_ecmr.hpp"
#include "dss/dss_ecao.hpp"
#include "dss/dss_ecpv.hpp"

class fixedGenerator : public generateRandomValueCallback
{
    const Octet _FixedValue;
public:
    fixedGenerator(const Octet & FixedValue)
        : _FixedValue(FixedValue)
        {}
    Octet getRandomValue()
        { return _FixedValue; }
};

struct DSSSchemeDomainParameters
{
    enum DSS_TYPE
    {
        ECNR,
        ECMR,
        ECAO,
        ECPV,
        ECKNR
    };
};

template <class EC_Dscr>
struct DSSDomainParameters
{
    typename EC_Dscr::aEC & EC;
    const DataInputPolicy & DefaultInputPolicy;
    const DSSSchemeDomainParameters & DSSSchemeParameters;
    const Algorithm::Precomputations_Method<typename EC_Dscr::aEC, ZZ> & PrecomputationMethod;
};

/* ----------------  GENERIC DSS ------------------ */

template <class EC_Dscr>
class ISOIEC_DSS : public DigitalSignatureWithRecovery
{
protected:
    
    DSSDomainParameters<EC_Dscr> _DomainParameters;

    typename EC_Dscr::aEC  & _Curve;
    typename EC_Dscr::pEC  _PCurve;
    
    ZZ   _privateKey;
    typename EC_Dscr::pECP  _publicKey;
    
    bool _isPrivateKeyLoaded;
    bool _isPublicKeyLoaded;

    /* Quick access */
    const unsigned long _Ln;
    const unsigned long _Lcm;

    
    typename EC_Dscr::pECP _BasePoint;
        
public:
    virtual bool isReadyToSign()
        { return _isPrivateKeyLoaded; }
    virtual bool isReadyToVerify()
        { return _isPublicKeyLoaded; }
    
public:
    ISOIEC_DSS(const DSSDomainParameters<EC_Dscr> & DomainParameters)
        : _DomainParameters(DomainParameters),
          _Curve(_DomainParameters.EC),
          _PCurve(_DomainParameters.EC),
          _privateKey(ZZ()),
          _publicKey(_PCurve.create()),
          _Ln(L(_DomainParameters.EC.getOrder())),
          _Ln(L(_DomainParameters.EC.getModulus())),
          _BasePoint(_PCurve.getBasePoint()),
          _isPrivateKeyLoaded(false),
          _isPublicKeyLoaded(false),
          _PRNG(PRNG)
        {
            /* TODO: Input policy sanity checks */
        }
    
    virtual ~ECNR() {}

    virtual void setPrivateKeyHook() {}
    virtual void setPrivateKey(const Octet & PrivateKey)
        {
            _privateKey = OS2IP(PrivateKey);
            _isPrivateKeyLoaded = true;
            
            setPrivateKeyHook();
        }

    virtual void setPublicKeyHook(){}
    virtual void setPublicKey(const Octet & PublicKey)
        {
            if (PublicKey.getDataSize() != ( _Lcm * 2 ))
                throw;

            tOS2FEP<cECP> OS2FEP;
            
            _Curve.enter_mod_context(cEC::FIELD_CONTEXT);
            
            _publicKey = _PCurve
                .create(OS2FEP(ByteSeq(PublicKey.getData(), _Lcm)),
                        OS2FEP(ByteSeq(PublicKey.getData() + _Lcm, _Lcm)));
            
            _Curve.leave_mod_context();
            
            _isPublicKeyLoaded = true;

            setPublicKeyHook();
        }
    
    virtual Octet generatePublicKey()
        {
            if (! _isPrivateKeyLoaded)
                throw; // Operation unaviable

            _Curve.enter_mod_context(cEC::FIELD_CONTEXT);
            _publicKey = _BasePoint() * _privateKey;

            typename EC_Dscr::aECP _AffinePublicKey = toAffine(_publicKey);
            
            _Curve.leave_mod_context();

            _isPublicKeyLoaded = true;
            _isReadyToVerify   = true;
            _isReadyToSign     = true;
            
            return FE2OSP(_AffinePublicKey.getX(), _Lcm) ||
                FE2OSP(_AffinePublicKey.getY(), _Lcm);
        }

    void buildPrecomputationTables()
        {
            if (_isPublicKeyLoaded)
                _publicKey.precompute(_DomainParameters.PrecomputationMethod);

            _BasePoint.precompute(_DomainParameters.PrecomputationMethod);
        }

    
    virtual DigitalSignature sign(const ByteSeq & data, const DataInputPolicy * dip = NULL) = 0;
    virtual VerificationVerdict verify(const DigitalSignature & data, const DataInputPolicy * dip = NULL) = 0;
    
};
