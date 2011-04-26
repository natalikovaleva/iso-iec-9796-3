#pragma once

#include <exception>

#include "dss/dss_generic_additions.hpp"

/* -------------------------- */

#include "generic/zz_utils.hpp"

/* ---------------- DSTU 4145  ------------------ */

template <class EC_Dscr>
class DSTU4145 : public GENERIC_DSS<EC_Dscr>
{
public:
    DSTU4145(const ECDSSDomainParameters<EC_Dscr> Parameters,
                const Hash H,
                generateRandomValueCallback & PRNG)
        : GENERIC_DSS<EC_Dscr>(Parameters, H, PRNG)
        {}

    ~DSTU4145()
        {}

    Octet generatePublicKey()
        {
            if (! this->_isPrivateKeyLoaded)
                throw std::exception(); // Operation unaviable

            const ZZ e = this->_Curve.getOrder() - this->_privateKey;

            this->_Curve.enter_mod_context(EC_Dscr::aEC::FIELD_CONTEXT);
            this->_publicKey = this->_BasePoint * e;
            this->_Curve.leave_mod_context();


            this->_isPublicKeyLoaded = true;

            this->setPublicKeyHook();

            this->_Curve.enter_mod_context(EC_Dscr::aEC::FIELD_CONTEXT);
            typename EC_Dscr::aECP AffinePublicKey
                = toAffine(this->_publicKey);
            this->_Curve.leave_mod_context();

            return FE2OSP(AffinePublicKey.getX(), this->_Lcm) ||
                FE2OSP(AffinePublicKey.getY(), this->_Lcm);
        }

    DigitalSignature sign(const ByteSeq & data)
        {
            if (! this->_isPrivateKeyLoaded)
                throw std::exception();

            const ZZ & N = this->_Curve.getOrder();

            const ZZ k = OS2IP(this->_PRNG()) % N;

            this->_Curve.enter_mod_context(EC_Dscr::aEC::FIELD_CONTEXT);
            const typename EC_Dscr::aECP PP = toAffine(this->_BasePoint * k);
            this->_Curve.leave_mod_context();

            const ZZ P = FE2IP(PP.getX());
            const ZZ d = OS2IP(this->_Hash(data));

            const ZZ r = MulMod(d, P, N);
            const ZZ s = (k + this->_privateKey*r) % N;

            return DigitalSignature(I2OSP(r), I2OSP(s),
                                    data);
        }

    VerificationVerdict verify(const DigitalSignature & data)
    {
        if ( ! this->_isPublicKeyLoaded)
            throw std::exception();

        const ZZ & N = this->_Curve.getOrder();

        const ZZ r = OS2IP(data.R);
        const ZZ s = OS2IP(data.S);

        if (IsZero(r) || IsZero(s) ||
            r > N || s > N)
            return VerificationVerdict();


        this->_Curve.enter_mod_context(EC_Dscr::aEC::FIELD_CONTEXT);
        typename EC_Dscr::aECP R
            = toAffine(this->_publicKey * r + this->_BasePoint * s);
        this->_Curve.leave_mod_context();

        const ZZ P = FE2IP(R.getX());
        const ZZ d = OS2IP(this->_Hash(data.M_clr));

        const ZZ rr = MulMod(d, P, N);

        return rr == r ? VerificationVerdict(data.M_clr) : VerificationVerdict();
    }

private:
    void setPrivateKeyHook()
        {
            this->_privateKey %= this->_Curve.getOrder();
        }

};
