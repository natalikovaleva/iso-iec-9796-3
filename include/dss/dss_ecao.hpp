#pragma once

#include <exception>

#include "dss/dss_isoiec9796-3.hpp"
#include "dss/datain_isoiec9796-3.hpp"

/* -------------------------- */

#include "generic/octet.hpp"
#include "generic/hash.hpp"
#include "generic/mgf.hpp"

#include "generic/zz_utils.hpp"

/* ---------------- ISO/IEC ECAO DSS ------------------ */

template<class EC_Dscr>
class ECAODataInputPolicy : public StaticDataInputPolicy
{
public:
    ECAODataInputPolicy(unsigned long L_rec, unsigned long L_pad,
                        const typename EC_Dscr::aEC & EC,
                        Hash::Hash_Type Hash_type)
        : StaticDataInputPolicy(L_rec,
                                L(EC.getModulus()) - L_rec - L_pad + 1,
                                L(EC.getModulus()),
                                Hash_type)
        {
            if (L_pad < 1)
                throw std::exception();
        }
};

struct DSSECAODomainParameters : public DSSSchemeParameters
{
    const Hash & H;
    const MGF  & M;

    DSSECAODomainParameters(const Hash & Hash,
                            const MGF  & MGF)
        : H(Hash),
          M(MGF)
        { SchemeID = ECAO; }
};


template <class EC_Dscr>
class ECAO : public ISOIEC_DSS<EC_Dscr>
{
    const Hash & _Hash;
    const MGF  & _MGF;

    Octet _CertData;
    TDataInput<ECAO_Input> _ECAO_Data;

#define ENABLE_ISOIEC_DSS_TPL_HACK
#include "dss/dss_isoiec9796-3-hack.hpp"

public:
    ECAO(const DSSDomainParameters<EC_Dscr> & DomainParameters,
         const DSSECAODomainParameters & DSSSchemeParameters,
         generateRandomValueCallback & PRNG)
        : ISOIEC_DSS<EC_Dscr>(DomainParameters,
                              PRNG),
          _Hash(DSSSchemeParameters.H),
          _MGF(DSSSchemeParameters.M),
          _ECAO_Data(
              TDataInput<ECAO_Input>(
                  DomainParameters.DefaultInputPolicy))
        {}

    ~ECAO()
        {}

    DigitalSignature sign(const ManagedBlob & data, const DataInputPolicy * dip = NULL)
        {
            if (! _isPrivateKeyLoaded)
                throw std::exception();

            const ZZ k = (OS2IP(_PRNG()) % _Curve.getOrder());
            const size_t K = _Ln; // Security parameter

            _Curve.enter_mod_context(EC_Dscr::aEC::FIELD_CONTEXT);
            const typename EC_Dscr::aECP PP = toAffine(_BasePoint * k);
            _Curve.leave_mod_context();
            const Octet P = EC2OSP(PP, EC_Dscr::aEC::EC2OSP_COMPRESSED);

            /* FIX IT */
            const DSSDataInput SignData =
                dip == NULL ?
                _ECAO_Data.createInput(data, P) :
                TDataInput<ECAO_Input>(*dip).createInput(data, P);

            _Curve.enter_mod_context(EC_Dscr::aEC::ORDER_CONTEXT);

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
        if (! _isPublicKeyLoaded)
            throw std::exception();

        const ZZ t = OS2IP(data.R);
        const ZZ s = OS2IP(data.S);

        if ((L(t) != _Ln + 1) ||
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

        _Curve.enter_mod_context(EC_Dscr::aEC::FIELD_CONTEXT);
        const typename EC_Dscr::aECP R = toAffine(_publicKey * u + _BasePoint * s);
        _Curve.leave_mod_context();

        const Octet P  = EC2OSP(R, EC_Dscr::aEC::EC2OSP_COMPRESSED);
        const Octet vdata =  data.R ^ P;

        /* FIX IT ? */
        const  DSSDataOutput vmsg =
            dip == NULL ?
            _ECAO_Data.createOutput(vdata) :
            TDataInput<ECAO_Input>(*dip).createOutput(vdata);

        if (vmsg.invalid)
        {
            return VerificationVerdict();
        }

        ManagedBlob M = vmsg.M_rec || data.M_clr;

        const  DSSDataInput vsign =
            dip == NULL ?
            _ECAO_Data.createInput(M, P) :
            TDataInput<ECAO_Input>(*dip).createInput(M, P);

        if (vsign.d == vmsg.d_pad)
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
