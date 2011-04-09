#pragma once

#include "dss/dss_isoiec9796-3.hpp"
#include "dss/datain_isoiec9796-3.hpp"

/* -------------------------- */

#include "generic/octet.hpp"
#include "generic/hash.hpp"
#include "generic/mgf.hpp"

#include "generic/zz_utils.hpp"

/* ---------------- ISO/IEC ECNR DSS ------------------ */

struct DSSECNRDomainParameters : public DSSSchemeParameters
{
    DSSECNRDomainParameters()
        { SchemeID = ECNR; }
};

template <class EC_Dscr>
class ECNR : public ISOIEC_DSS<EC_Dscr>
{
    TDataInput<ECNR_Input> _ECNR_Data;

#define ENABLE_ISOIEC_DSS_TPL_HACK
#include "dss/dss_isoiec9796-3-hack.hpp"

public:
    ECNR(const DSSDomainParameters<EC_Dscr> & DomainParameters,
         generateRandomValueCallback & PRNG)
        : ISOIEC_DSS<EC_Dscr>(DomainParameters,
                              PRNG),
          _ECNR_Data(
              TDataInput<ECNR_Input>(
                  DomainParameters.DefaultInputPolicy))
        {}

    ~ECNR()
        {}

    DigitalSignature sign(const ByteSeq & data, const DataInputPolicy * dip = NULL)
        {
            if (! _isPrivateKeyLoaded)
                throw;

            const ZZ k = OS2IP(_PRNG());

            Octet P;
            
            do
            {
                _Curve.enter_mod_context(EC_Dscr::aEC::FIELD_CONTEXT);
                const typename EC_Dscr::aECP PP = toAffine(_BasePoint * k);
                _Curve.leave_mod_context();

                if (PP.isZero())
                    continue;
                
                P  = EC2OSP(PP, EC_Dscr::aEC::EC2OSP_COMPRESSED);
                break;
            }
            while(1);
            

            /* FIX IT */
            const DSSDataInput SignData =
                dip == NULL ?
                _ECNR_Data.createInput(data, P) :
                TDataInput<ECNR_Input>(*dip).createInput(data, P);

            _Curve.enter_mod_context(EC_Dscr::aEC::ORDER_CONTEXT);

            const ZZ_p pi = InMod(OS2IP(P));
            const ZZ_p d  = InMod(OS2IP(SignData.d));

            std::cout << "d: " << I2OSP(d) << std::endl;
            
            const ZZ_p r = (d + pi);
            const ZZ_p s = (InMod(k) - InMod(_privateKey)*r);
            
            _Curve.leave_mod_context();
            
            const ByteSeq R = I2OSP(r,_Ln);
            const ByteSeq S = I2OSP(s,_Ln);

            return DigitalSignature(R, S, SignData.M_clr);
        }

    VerificationVerdict verify(const DigitalSignature & data, const DataInputPolicy * dip = NULL)
    {
        if ( ! _isPublicKeyLoaded )
            throw;

        ZZ t = OS2IP(data.R);
        ZZ s  = OS2IP(data.S);

        if ((L(t) > _Ln ||
            (IsZero(t))) ||
            (s > _Curve.getOrder()))
            return VerificationVerdict();

        t %= _Curve.getOrder();

        _Curve.enter_mod_context(EC_Dscr::aEC::FIELD_CONTEXT);
        typename EC_Dscr::aECP R = toAffine(_publicKey * t + _BasePoint * s);
        _Curve.leave_mod_context();

        const Octet P  = EC2OSP(R, EC_Dscr::aEC::EC2OSP_COMPRESSED);

        const ZZ d = (t - OS2IP(P)) % _Curve.getOrder();
        std::cout << "d: " << I2OSP(d) << std::endl;

        Octet vdata = I2OSP(d);

        /* MAKE CHECKS */
        const  DSSDataInput vmsg =
            dip == NULL ?
            _ECNR_Data.createOutput(vdata, P) :
            TDataInput<ECNR_Input>(*dip).createOutput(vdata, P);

        Octet M = vmsg.d || data.M_clr;

        std::cerr << "M: " << M << std::endl;
        
        const  DSSDataInput vsign =
            dip == NULL ?
            _ECNR_Data.createInput(M, P) :
            TDataInput<ECNR_Input>(*dip).createInput(M, P);

        if (! vmsg.M_clr.isEmpty())
        {
            std::cerr << "Checking '" << vmsg.M_clr << "' instead of '" << vdata << "'" << std::endl;
        }
        
        
        if (vsign.d == vmsg.M_clr)
        {
            return VerificationVerdict(M);
        }
        else
        {
            std::cerr << vsign.d << " != " << vmsg.M_clr << std::endl;
            return VerificationVerdict();
        }
    }

#undef ENABLE_ISOIEC_DSS_TPL_HACK
#include "dss/dss_isoiec9796-3-hack.hpp"

};
