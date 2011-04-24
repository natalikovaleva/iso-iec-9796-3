#pragma once

/* Field convertor helper */

#include "dss/dss.hpp"
#include "generic/octet.hpp"

#define OS2FEP_TPL
template <class ECP>
struct tOS2FEP
{
    inline typename ECP::FE operator()(const ByteSeq & From);
};

#include "ec/ZZ_p/dss_tpl.hpp"
#include "ec/GF2X/dss_tpl.hpp"

template<class EC_Dscr>
class ECDataInputPolicy : public StaticDataInputPolicy
{
public:
    ECDataInputPolicy(long L_rec,
                      const typename EC_Dscr::aEC & EC,
                      Hash::Hash_Type Hash_type)
        : StaticDataInputPolicy(L_rec,
                                (Lb(EC.getOrder()) / 8) - L_rec,
                                Lb(EC.getOrder()) / 8,
                                Hash_type)
        {}
};

struct DSSSchemeParameters
{
    enum DSS_TYPE
    {
        ECNR,
        ECMR,
        ECAO,
        ECPV,
        ECKNR
    };

    DSS_TYPE SchemeID;
};

template <class EC_Dscr>
struct DSSDomainParameters
{
    typename EC_Dscr::aEC & EC;
    const DataInputPolicy & DefaultInputPolicy;
    const Algorithm::Precomputations_Method<typename EC_Dscr::pECP, ZZ> & PrecomputationMethod;

    DSSDomainParameters(
        typename EC_Dscr::aEC & EC,
        const DataInputPolicy & DefaultInputPolicy,
        const Algorithm::Precomputations_Method<typename EC_Dscr::pECP, ZZ> & PrecomputationMethod)
        : EC(EC),
          DefaultInputPolicy(DefaultInputPolicy),
          PrecomputationMethod(PrecomputationMethod)
        {}
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

    generateRandomValueCallback & _PRNG;

public:
    virtual bool isReadyToSign()
        { return _isPrivateKeyLoaded; }
    virtual bool isReadyToVerify()
        { return _isPublicKeyLoaded; }

public:
    ISOIEC_DSS(const DSSDomainParameters<EC_Dscr> & DomainParameters,
               generateRandomValueCallback & PRNG)
        : _DomainParameters(DomainParameters),
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
        {
            /* TODO: Input policy sanity checks */
        }

    virtual ~ISOIEC_DSS() {}

    virtual bool setPrivateKey(const Octet & PrivateKey)
        {
            _privateKey = OS2IP(PrivateKey);

            if (IsZero(_privateKey) ||
                _privateKey > _Curve.getOrder())
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
                throw; // Operation unaviable

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


    virtual DigitalSignature sign(const ByteSeq & data, const DataInputPolicy * dip = NULL) = 0;
    virtual VerificationVerdict verify(const DigitalSignature & data, const DataInputPolicy * dip = NULL) = 0;

private:
    virtual void setPrivateKeyHook() {}
    virtual void setPublicKeyHook(){}
};

#include "dss/dss_ecknr.hpp"
#include "dss/dss_ecnr.hpp"
#include "dss/dss_ecmr.hpp"
#include "dss/dss_ecao.hpp"
#include "dss/dss_ecpv.hpp"
