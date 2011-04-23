#pragma once

#include "dss/dss_isoiec9796-3.hpp"
#include "dss/datain_isoiec9796-3.hpp"

/* -------------------------- */

#include "generic/octet.hpp"
#include "generic/hash.hpp"
#include "generic/mgf.hpp"

#include "generic/zz_utils.hpp"

/* ---------------- ISO/IEC ECMR DSS ------------------ */

template<class EC_Dscr>
class ECMRDataInputPolicy : public StaticDataInputPolicy
{
public:
    ECMRDataInputPolicy(long L_rec,
                        const typename EC_Dscr::aEC & EC,
                        Hash::Hash_Type Hash_type)
        : StaticDataInputPolicy(L_rec,
                                L(EC.getOrder()) - L_rec,
                                L(EC.getOrder()),
                                Hash_type)
        {}
};

struct DSSECMRDomainParameters : public DSSSchemeParameters
{
    const Hash & H;
    const MGF  & M;

    DSSECMRDomainParameters(const Hash & Hash,
                            const MGF  & MGF)
        : H(Hash),
          M(MGF)
        { SchemeID = ECMR; }
};

template <class EC_Dscr>
class ECMR : public ISOIEC_DSS<EC_Dscr>
{
    const Hash & _Hash;
    const MGF  & _MGF;

    Octet _CertData;
    TDataInput<ECMR_Input> _ECMR_Data;

#define ENABLE_ISOIEC_DSS_TPL_HACK
#include "dss/dss_isoiec9796-3-hack.hpp"

public:
    ECMR(const DSSDomainParameters<EC_Dscr> & DomainParameters,
         const DSSECMRDomainParameters & DSSSchemeParameters,
         generateRandomValueCallback & PRNG)
        : ISOIEC_DSS<EC_Dscr>(DomainParameters,
                              PRNG),
          _Hash(DSSSchemeParameters.H),
          _MGF(DSSSchemeParameters.M),
          _ECMR_Data(
              TDataInput<ECMR_Input>(
                  DomainParameters.DefaultInputPolicy))
        {}

    ~ECMR()
        {}

    DigitalSignature sign(const ByteSeq & data, const DataInputPolicy * dip = NULL)
        {
            if (! _isPrivateKeyLoaded)
                throw;

            const ZZ k = OS2IP(_PRNG()) % _Curve.getOrder();

            _Curve.enter_mod_context(EC_Dscr::aEC::FIELD_CONTEXT);
            const typename EC_Dscr::aECP PP = toAffine(_BasePoint * k);
            _Curve.leave_mod_context();

            const Octet P = _MGF(EC2OSP(PP, EC_Dscr::aEC::EC2OSP_UNCOMPRESSED), _Ln);

            /* FIX IT */
            const DSSDataInput SignData =
                dip == NULL ?
                _ECMR_Data.createInput(data, P) :
                TDataInput<ECMR_Input>(*dip).createInput(data, P);

            _Curve.enter_mod_context(EC_Dscr::aEC::ORDER_CONTEXT);

            const Octet r = SignData.d ^ P;
            const ZZ_p s = InMod(OS2IP(r) * k - OS2IP(r) - 1) / InMod(_privateKey + 1);

            _Curve.leave_mod_context();

            const ByteSeq S = I2OSP(s,_Ln);

            return DigitalSignature(r, S, SignData.M_clr);
        }

    VerificationVerdict verify(const DigitalSignature & data, const DataInputPolicy * dip = NULL)
    {
        if (! _isPublicKeyLoaded)
            throw;

        ZZ t = OS2IP(data.R);
        ZZ s = OS2IP(data.S);

        if ((L(t) > _Ln) ||
            (IsZero(t)) ||
            (s > _Curve.getOrder()))
            return VerificationVerdict();

        ZZ u1, u2;

        {
            _Curve.enter_mod_context(EC_Dscr::aEC::ORDER_CONTEXT);

            const ZZ_p rP = InMod(t);
            const ZZ_p sP = InMod(s);

            u1 = rep((1 + rP + sP) /rP);
            u2 = rep(sP/rP);

            _Curve.leave_mod_context();
        }

        _Curve.enter_mod_context(EC_Dscr::aEC::FIELD_CONTEXT);
        const typename EC_Dscr::aECP R = toAffine(_publicKey * u2 + _BasePoint * u1);
        _Curve.leave_mod_context();

        const Octet P  = _MGF(EC2OSP(R, EC_Dscr::aEC::EC2OSP_UNCOMPRESSED), _Ln);
        const Octet vdata =  data.R ^ P;

        /* FIX IT ? */
        const  DSSDataInput vmsg =
            dip == NULL ?
            _ECMR_Data.createOutput(vdata, P) :
            TDataInput<ECMR_Input>(*dip).createOutput(vdata, P);

        Octet M = vmsg.d || data.M_clr;

        const  DSSDataInput vsign =
            dip == NULL ?
            _ECMR_Data.createInput(M, P) :
            TDataInput<ECMR_Input>(*dip).createInput(M, P);

        if (vsign.d == vmsg.M_clr)
        {
            return VerificationVerdict(M);
        }
        else
        {
            return VerificationVerdict();
        }
    }

#undef ENABLE_ISOIEC_DSS_TPL_HACK
#include "dss/dss_isoiec9796-3-hack.hpp"

};
