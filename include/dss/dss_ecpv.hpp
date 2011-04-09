#pragma once

#include "dss/dss_isoiec9796-3.hpp"
#include "dss/datain_isoiec9796-3.hpp"

/* -------------------------- */

#include "generic/octet.hpp"
#include "generic/hash.hpp"
#include "generic/mgf.hpp"
#include "generic/kdf.hpp"
#include "generic/sym.hpp"

#include "generic/zz_utils.hpp"

/* ---------------- ISO/IEC ECPV DSS ------------------ */

struct DSSECPVDomainParameters : public DSSSchemeParameters
{
    const Hash & H;
    const Sym  & S;
    const size_t KSize;

    DSSECPVDomainParameters(const Hash & Hash,
                            const Sym  & Sym,
                            size_t KSize)
        : H(Hash),
          S(Sym),
          KSize(KSize)
        { SchemeID = ECPV; }
};


template <class EC_Dscr>
class ECPV : public ISOIEC_DSS<EC_Dscr>
{
    class ECPVKDF : public KDF
    {
        const size_t L_key;
        const MGF MGF2;
    public:
        ECPVKDF(const Hash & Hash, size_t L_key)
            : L_key(L_key),
              MGF2(MGF::MGF2, Hash) {};

        inline ByteSeq operator() (const ByteSeq & data) const
            {
                return MGF2(data, L_key);
            }
    };

    const Hash & _Hash;
    const Sym & _Sym;
    const ECPVKDF _KDF;

    TDataInput<ECPV_Input> _ECPV_Data;

#define ENABLE_ISOIEC_DSS_TPL_HACK
#include "dss/dss_isoiec9796-3-hack.hpp"

public:
    ECPV(const DSSDomainParameters<EC_Dscr> & DomainParameters,
         const DSSECPVDomainParameters & DSSSchemeParameters,
         generateRandomValueCallback & PRNG)
        : ISOIEC_DSS<EC_Dscr>(DomainParameters,
                              PRNG),
          _Hash(DSSSchemeParameters.H),
          _Sym(DSSSchemeParameters.S),
          _KDF(_Hash, DSSSchemeParameters.KSize),
          _ECPV_Data(
              TDataInput<ECPV_Input>(
                  DomainParameters.DefaultInputPolicy))
        {}

    ~ECPV()
        {}

    DigitalSignature sign(const ByteSeq & data, const DataInputPolicy * dip = NULL)
        {
            if (! _isPrivateKeyLoaded)
                throw;

            const ZZ k = OS2IP(_PRNG());

            _Curve.enter_mod_context(EC_Dscr::aEC::FIELD_CONTEXT);
            const typename EC_Dscr::aECP PP = toAffine(_BasePoint * k);
            _Curve.leave_mod_context();

            const Octet P  = _KDF(FE2OSP(PP.getX()));

            /* FIX IT */
            const DSSDataInput SignData =
                dip == NULL ?
                _ECPV_Data.createInput(data, P) :
                TDataInput<ECPV_Input>(*dip).createInput(data, P);

            _Curve.enter_mod_context(EC_Dscr::aEC::ORDER_CONTEXT);

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
        if (! _isPrivateKeyLoaded )
            throw;

        const ZZ s  = OS2IP(data.S);

        if (s >= _Curve.getOrder())
            return VerificationVerdict();

        const Octet u = _Hash(data.R || data.M_clr);
        const ZZ t = OS2IP(u) % _Curve.getOrder();

        if ( IsZero(t) )
            return VerificationVerdict();

        _Curve.enter_mod_context(EC_Dscr::aEC::FIELD_CONTEXT);
        const typename EC_Dscr::aECP R = toAffine(_publicKey * t + _BasePoint * s);
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
