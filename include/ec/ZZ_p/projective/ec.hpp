#pragma once

#include <NTL/GF2X.h>
#include <NTL/ZZ.h>
#include <NTL/ZZ_p.h>

#include <ostream>
#include <memory>

#include "ec/ZZ_p/affine/ec.hpp"
#include "algorithm/convertors.hpp"
#include "algorithm/precomputations.hpp"
#include "algorithm/multiplication.hpp"

namespace ECZZ_p
{
    namespace Projective
    {
        using NTL::ZZ_p;
        using NTL::ZZ;

        class EC;

        class EC_Point
        {

            ZZ_p X;
            ZZ_p Y;
            ZZ_p Z;

            const EC & __EC;

            bool __isZeroPoint;

            Algorithm::Precomputations<EC_Point,
                                       ZZ_p> __precomputations;
            const Algorithm::RLMul<EC_Point,
                                   ZZ_p> __generic_multiplication;

        public:

            EC_Point(const ZZ_p &X, const ZZ_p &Y, const ZZ_p &Z, const EC & __EC); // Projective
            EC_Point(const EC_Point & Point); // Same point in same field
            EC_Point(const EC & __EC); // Zero
            ~EC_Point();

            bool _IsOnCurve() const;

        public:
            bool precompute(const Algorithm::Precomputations_Method<EC_Point, ZZ_p> & method)
                { __precomputations = method(*this); return __precomputations.isReady(); }

            inline bool isPrecomputed() const
                { return __precomputations.isReady(); }

            inline bool isZero() const
                { return __isZeroPoint; }

            EC_Point & operator=  (const EC_Point & Y);
            EC_Point   operator+  (const EC_Point & Y) const;
            EC_Point   operator+  (const Affine::EC_Point & Y) const;
            EC_Point   operator*  (const ZZ_p & Y) const;
            EC_Point   operator*  (const long Y) const;

            void operator+= (const Affine::EC_Point & Y);
            void operator+= (const EC_Point & Y);
            void operator*= (const long Y);
            void operator*= (const ZZ_p & Y);

            inline bool operator== (const EC_Point &Y_)
                { return Y_.isZero() ? isZero() : (X == Y_.X) && (Y == Y_.Y); }

            inline const ZZ_p & getX() const
                { return X; }

            inline const ZZ_p & getY() const
                { return Y; }

            inline const ZZ_p & getZ() const
                { return Z; }

            inline const EC & getEC() const
                { return __EC; }

            inline bool isSameEC(const EC & __EC) const;

            friend class EC;
            friend std::ostream& operator<<(std::ostream& s,
                                            const EC_Point & _EC_Point);
            friend inline void Jacobian_Double(ZZ_p & X, ZZ_p & Y, ZZ_p & Z, const EC & __EC);
            friend inline void Jacobian_Addition(EC_Point & _X, const Affine::EC_Point & _Y);
            friend inline void Jacobian_Addition(EC_Point & _X, const EC_Point & _Y);
        };


        class EC : public Affine::EC
        {
            EC_Point G; // Base point in GF2X/Projective
                        // Can be precomputed
        public:
            inline const EC_Point & getBasePoint() const
                { return G; }
            inline const Affine::EC_Point & getAffineBasePoint() const
                { return Affine::EC::getBasePoint(); }

        public:
            /* This EC curve class exists only for precomputations.
             * Derive Projective::EC from Affine::EC */

            EC(const Affine::EC & EC);
            ~EC();

            EC_Point create() const;

            EC_Point create(const ZZ_p & x,
                            const ZZ_p & y,
                            const ZZ_p & z) const;
        };

        inline bool IsZero(const EC_Point & EC_Point)
        {
            return EC_Point.isZero();
        }
    }

    Projective::EC_Point toProjective(const Affine::EC_Point & Point,
                                      const Projective::EC & EC);

    Affine::EC_Point toAffine(const Projective::EC_Point & Point);
}

namespace Algorithm
{
    /* For templates/algorithms */
    template <>
    struct conv1 <ECZZ_p::Affine::EC_Point, ECZZ_p::Projective::EC_Point>
    {
        inline ECZZ_p::Affine::EC_Point operator()(const ECZZ_p::Projective::EC_Point & from)
            { return ECZZ_p::toAffine(from); }
    };

    template <>
    struct conv1 <ECZZ_p::Projective::EC_Point, ECZZ_p::Projective::EC_Point>
    {
        inline ECZZ_p::Projective::EC_Point operator()(const ECZZ_p::Projective::EC_Point & from)
            { return from; }
    };
}
