#pragma once

#include <exception>

#include "dss/dss_isoiec9796-3.hpp"
#include "dss/datain_isoiec9796-3.hpp"

/* -------------------------- */

#include "generic/octet.hpp"
#include "generic/hash.hpp"
#include "generic/mgf.hpp"

#include "generic/zz_utils.hpp"

/* ---------------- ISO/IEC ECKNR DSS ------------------ */

struct DSSECKNRDomainParameters : public DSSSchemeParameters
{
    const Hash & H;
    const MGF  & M;

    DSSECKNRDomainParameters(const Hash & Hash,
                             const MGF  & MGF)
        : H(Hash),
          M(MGF)
        { SchemeID = ECKNR; }
};

template <class EC_Dscr>
class ECKNR : public ISOIEC_DSS<EC_Dscr>
{
    const Hash & _Hash;
    const MGF  & _MGF;

    Octet _CertData;
    TDataInput<STD4_Input> _ECKNR_Data;

#define ENABLE_ISOIEC_DSS_TPL_HACK
#include "dss/dss_isoiec9796-3-hack.hpp"

public:
    ECKNR(const DSSDomainParameters<EC_Dscr> & DomainParameters,
          const DSSECKNRDomainParameters & DSSSchemeParameters,
          generateRandomValueCallback & PRNG)
        : ISOIEC_DSS<EC_Dscr>(DomainParameters,
                              PRNG),
          _Hash(DSSSchemeParameters.H),
          _MGF(DSSSchemeParameters.M),
          _ECKNR_Data(
              TDataInput<STD4_Input>(
                  DomainParameters.DefaultInputPolicy))
        {}

    ~ECKNR()
        {}

    Octet generatePublicKey()
        {
            if (! _isPrivateKeyLoaded)
                throw std::exception(); // Operation unaviable

            const ZZ e = InvMod(_privateKey, _Curve.getOrder());

            _Curve.enter_mod_context(EC_Dscr::aEC::FIELD_CONTEXT);
            _publicKey = _BasePoint * e;
            _Curve.leave_mod_context();

            _isPublicKeyLoaded = true;

            setPublicKeyHook();

            _Curve.enter_mod_context(EC_Dscr::aEC::FIELD_CONTEXT);
            typename EC_Dscr::aECP AffinePublicKey = Algorithm::toAffine(_publicKey);
            _Curve.leave_mod_context();

            using namespace ECGF2X;
            using namespace ECZZ_p;
            return FE2OSP(AffinePublicKey.getX(), _Lcm) ||
                FE2OSP(AffinePublicKey.getY(), _Lcm);
        }

    DigitalSignature sign(const ManagedBlob & data, const DataInputPolicy * dip = NULL)
        {
            if (! (_isPublicKeyLoaded && _isPrivateKeyLoaded))
                throw std::exception();

            const ZZ k = OS2IP(_PRNG()) % _Curve.getOrder();

            _Curve.enter_mod_context(EC_Dscr::aEC::FIELD_CONTEXT);
            const typename EC_Dscr::aECP PP = Algorithm::toAffine(_publicKey * k);
            _Curve.leave_mod_context();

            const Octet Za = Truncate(_CertData, _Hash.getInputSize());

            const Octet P  = _MGF(EC2OSP(PP, EC_Dscr::aEC::EC2OSP_COMPRESSED), _Ln);

            _Curve.enter_mod_context(EC_Dscr::aEC::ORDER_CONTEXT);

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

            _Curve.leave_mod_context();

            return DigitalSignature(r, S, SignData.M_clr);
        }

    VerificationVerdict verify(const DigitalSignature & data, const DataInputPolicy * dip = NULL)
    {
        if ( ! _isPublicKeyLoaded)
            throw std::exception();

        ZZ t = OS2IP(data.R);
        ZZ s  = OS2IP(data.S);

        if ((L(t) > L(_Curve.getOrder()) ||
            (IsZero(t))) ||
            (s > _Curve.getOrder()))
            return VerificationVerdict();

        t %= _Curve.getOrder();

        _Curve.enter_mod_context(EC_Dscr::aEC::FIELD_CONTEXT);
        typename EC_Dscr::aECP R = Algorithm::toAffine(_publicKey * s + _BasePoint * t);
        _Curve.leave_mod_context();

        const Octet P  = _MGF(EC2OSP(R, EC_Dscr::aEC::EC2OSP_COMPRESSED), _Ln);
        const Octet Za = Truncate(_CertData, _Hash.getInputSize());
        const Octet m = _MGF(Za || data.M_clr, _Ln);

        Octet vdata = data.R ^ P ^ m;

        /* MAKE CHECKS */
        const  DSSDataOutput vmsg =
            dip == NULL ?
            _ECKNR_Data.createOutput(vdata) :
            TDataInput<STD4_Input>(*dip).createOutput(vdata);

        ManagedBlob M = vmsg.M_rec || data.M_clr;

        const  DSSDataInput vsign =
            dip == NULL ?
            _ECKNR_Data.createInput(M, P) :
            TDataInput<STD4_Input>(*dip).createInput(M, P);

        if (vsign.d == vmsg.d_pad)
        {
            return VerificationVerdict(M);
        }
        else
        {
            return VerificationVerdict();
        }
    }

private:
    void setPublicKeyHook()
        {
            using namespace ECGF2X;
            using namespace ECZZ_p;
            _Curve.enter_mod_context(EC_Dscr::aEC::FIELD_CONTEXT);

            typename EC_Dscr::aECP _AffinePublicKey = Algorithm::toAffine(_publicKey);
            _CertData = FE2OSP(_AffinePublicKey.getX()) || FE2OSP(_AffinePublicKey.getY());

            _Curve.leave_mod_context();
        }

#undef ENABLE_ISOIEC_DSS_TPL_HACK
#include "dss/dss_isoiec9796-3-hack.hpp"
};
