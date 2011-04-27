#pragma once

#include <exception>

#include "dss/dss.hpp"
#include "generic/octet.hpp"
#include "generic/hash.hpp"

/* TODO: Merge+move it to dss_ec.hpp */
#define OS2FEP_TPL
template <class ECP>
struct tOS2FEP
{
    inline typename ECP::FE operator()(const ByteSeq & From);
};

#include "ec/ZZ_p/dss_tpl.hpp"
#include "ec/GF2X/dss_tpl.hpp"

/* TODO: Merge it with DSSDomainParameters, or make it parent class for it */
template <class EC_Dscr>
struct ECDSSDomainParameters
{
    typename EC_Dscr::aEC & EC;
    const Algorithm::Precomputations_Method
    <typename EC_Dscr::pECP, ZZ> & PrecomputationMethod;

    ECDSSDomainParameters(
        typename EC_Dscr::aEC & EC,
        const Algorithm::Precomputations_Method
        <typename EC_Dscr::pECP, ZZ> & PrecomputationMethod)
        : EC(EC),
          PrecomputationMethod(PrecomputationMethod)
        {}
};


/* TODO: Merge it with ISOIEC_DSS */
template <class EC_Dscr>
class GENERIC_DSS : public DigitalSignatureWithAddition
{
protected:

    ECDSSDomainParameters<EC_Dscr> _DomainParameters;
    const Hash _Hash;

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

    generateRandomValueCallback & _PRNG;

protected:
    GENERIC_DSS(const GENERIC_DSS & Other);

    GENERIC_DSS(const ECDSSDomainParameters<EC_Dscr> Parameters,
                const Hash H,
                generateRandomValueCallback & PRNG)
        : _DomainParameters(Parameters),
          _Hash(H),
          _Curve(_DomainParameters.EC),
          _PCurve(_DomainParameters.EC),
          _privateKey(ZZ()),
          _publicKey(_PCurve.create()),
          _isPrivateKeyLoaded(false),
          _isPublicKeyLoaded(false),
          _Ln(L(_DomainParameters.EC.getOrder())),
          _Lcm(L(_DomainParameters.EC.getModulus())),
          _BasePoint(_PCurve.getBasePoint()),
          _PRNG(PRNG)
        {}

public:
    virtual bool setPrivateKey(const Octet & PrivateKey)
        {
            _privateKey = OS2IP(PrivateKey);

            if (IsZero(_privateKey) ||
                _privateKey > this->_Curve.getOrder())
                return false;

            _isPrivateKeyLoaded = true;

            setPrivateKeyHook();

            return _isPrivateKeyLoaded;
        }
    virtual bool setPublicKey(const Octet & PublicKey)
        {
            if (PublicKey.getDataSize() != ( _Lcm * 2 ))
                return false;

            tOS2FEP<typename EC_Dscr::aECP> OS2FEP;

            _Curve.enter_mod_context(EC_Dscr::aEC::FIELD_CONTEXT);

            _publicKey =
                toProjective(_Curve
                             .create(OS2FEP(ByteSeq(PublicKey.getData(), _Lcm)),
                                     OS2FEP(ByteSeq(PublicKey.getData() + _Lcm, _Lcm))),
                             _PCurve);

            _Curve.leave_mod_context();

            _isPublicKeyLoaded = true;

            setPublicKeyHook();

            return _isPublicKeyLoaded;
        }

    virtual Octet generatePublicKey()
        {
            if (! _isPrivateKeyLoaded)
                throw std::exception(); // Operation unaviable

            _Curve.enter_mod_context(EC_Dscr::aEC::FIELD_CONTEXT);
            _publicKey = _BasePoint * _privateKey;

            typename EC_Dscr::aECP _AffinePublicKey = toAffine(_publicKey);

            _Curve.leave_mod_context();

            _isPublicKeyLoaded = true;

            setPublicKeyHook();

            return FE2OSP(_AffinePublicKey.getX(), _Lcm) ||
                FE2OSP(_AffinePublicKey.getY(), _Lcm);
        }

    void buildPrecomputationTables()
        {
            _Curve.enter_mod_context(EC_Dscr::aEC::FIELD_CONTEXT);

            if (_isPublicKeyLoaded)
                _publicKey.precompute(_DomainParameters.PrecomputationMethod);

            _BasePoint.precompute(_DomainParameters.PrecomputationMethod);

            _Curve.leave_mod_context();
        }

public:
    virtual bool isReadyToSign()
        { return _isPrivateKeyLoaded; }
    virtual bool isReadyToVerify()
        { return _isPublicKeyLoaded; }

public:
    virtual ~GENERIC_DSS(){}

    virtual DigitalSignature sign(const ByteSeq & data) = 0;
    virtual VerificationVerdict verify(const DigitalSignature & data) = 0;

protected:
    virtual void setPrivateKeyHook() {}
    virtual void setPublicKeyHook(){}
};
