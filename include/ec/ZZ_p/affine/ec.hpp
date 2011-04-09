#pragma once

#include <NTL/ZZ_p.h>
#include <ostream>

#include "algorithm/convertors.hpp"
#include "algorithm/precomputations.hpp"
#include "algorithm/multiplication.hpp"

namespace ECZZ_p
{
    namespace Affine
    {

        using NTL::ZZ_p;
        using NTL::ZZ;
        using NTL::ZZ_pContext;
        using NTL::random_ZZ_p;


        class EC;

        class EC_Point
        {
            ZZ_p X;
            ZZ_p Y;

            const EC & __EC;

            bool isZeroPoint;

            Algorithm::Precomputations<EC_Point,
                                       ZZ> __precomputations;
            const Algorithm::RLMul<EC_Point,
                                   ZZ> __generic_multiplication;

        public:
            typedef ZZ_p FE;

            EC_Point(const ZZ_p &X, const ZZ_p &Y, const EC & __EC); // Generic
            EC_Point(const EC_Point & Point); // Same point in same field
            EC_Point(const EC & __EC); // Zero
            ~EC_Point();

            bool _IsOnCurve() const;

        public:
            bool precompute(const Algorithm::Precomputations_Method<EC_Point, ZZ> & method)
                { __precomputations = method(*this); return __precomputations.isReady(); }

            inline bool isPrecomputed() const
                { return __precomputations.isReady(); }

            inline bool isZero() const
                { return isZeroPoint; }

            EC_Point & operator=  (const EC_Point & Y);
            EC_Point   operator+  (const EC_Point & Y) const;
            EC_Point   operator*  (const ZZ   & Y) const;
            EC_Point   operator*  (const long Y) const;

            void operator+= (const EC_Point & Y);
            void operator*= (const ZZ & Y);
            void operator*= (const long Y);

            inline bool operator== (const EC_Point &Y_)
                { return Y_.isZero() ? isZero() : (X == Y_.X) && (Y == Y_.Y); }

            inline const ZZ_p & getX() const
                { return X; }

            inline const ZZ_p & getY() const
                { return Y; }

            inline const EC & getEC() const
                { return __EC; }

            inline bool isSameEC(const EC & __EC) const;

            friend class ECZZ_p::Affine::EC;
            friend std::ostream& ECZZ_p::Affine::operator<<(std::ostream& s,
                                                            const EC_Point & _EC_Point);
        };

        class EC
        {
            ZZ_pContext __mod;   // Field Modulus context
            ZZ_pContext __order; // Order Modulus context
            ZZ_pContext __global;
            bool __is_global_setted;

            const ZZ   P;     // Modulus, get from creators context
            const ZZ   N;     // Point Order
            const ZZ_p Seed;  // Random generated seed
            const ZZ_p A;     // EC Params
            const ZZ_p B;     // EC Params
            const ZZ_p C;     // EC Params

            const EC_Point G; // Base point


            const ZZ & setAndUseMod(const ZZ & x)
                { __global.save(); ZZ_p::init(x); return x; }


            /* Types */
        public:
            enum MOD_CONTEXT
            {
                FIELD_CONTEXT,
                ORDER_CONTEXT
            };

            enum EC2OSP_COMPRESS_MODE
            {
                EC2OSP_UNCOMPRESSED,
                EC2OSP_COMPRESSED,
                EC2OSP_HYBRID
            };

            /* Getters */
        public:
            inline const ZZ & getModulus() const
                { return P; }

            inline const ZZ_p & getA() const
                { return A; }

            inline const ZZ_p & getB() const
                { return B; }

            inline const ZZ_p & getC() const
                { return C; }

            inline const EC_Point & getBasePoint() const
                { return G; }

            inline const ZZ & getOrder() const
                { return N; }

        public:

            EC(const ZZ_p & A,
               const ZZ_p & B,
               const ZZ_p & C,
               const ZZ   & N,
               //--- TODO Something with it .. ---
               const ZZ_p & Gx,
               const ZZ_p & Gy,
               //---------------------------------
               const ZZ_p & Seed);

            EC(const ZZ & A,
               const ZZ & B,
               const ZZ & C,
               const ZZ & N,
               const ZZ & P,
               //--- TODO Something with it .. ---
               const ZZ & Gx,
               const ZZ & Gy,
               //---------------------------------
               const ZZ & Seed);

            ~EC();

            EC_Point create() const;

            EC_Point create(const ZZ_p & x,
                            const ZZ_p & y) const;

            EC_Point create(const ZZ & x,
                            const ZZ & y) const;


            inline const EC_Point & get_base_point(void) const { return G; } ;


            bool generate_random(ZZ_p & d) const;
            ZZ_p generate_random(void) const;



            inline void enter_mod_context(enum MOD_CONTEXT context)
                {
                    if (! __is_global_setted)
                    {
                        __global.save();
                        __is_global_setted = true;
                    }

                    switch(context)
                    {
                        case FIELD_CONTEXT: __mod.restore();   break;
                        case ORDER_CONTEXT: __order.restore(); break;
                        default: throw;
                    }
                }

            inline void leave_mod_context()
                { __global.restore(); __is_global_setted = false; }


            bool isCorrectOrder() const;

            friend class ECZZ_p::Affine::EC_Point;
            friend std::ostream& ECZZ_p::Affine::operator<<(std::ostream& s, const EC & _EC);
        };

        std::ostream& operator<<(std::ostream& s, const EC & _EC);
        std::ostream& operator<<(std::ostream& s, const EC_Point & _EC_Point);

        inline long int NumBits(const NTL::ZZ_p & x) { return NumBits(rep(x)); }
        inline long int bit(const NTL::ZZ_p & x, long int i) { return bit(rep(x), i); }
    }
}

namespace Algorithm
{
    /* For templates/algorithms */
    template <>
    struct conv1 <ECZZ_p::Affine::EC_Point, ECZZ_p::Affine::EC_Point>
    {
        inline ECZZ_p::Affine::EC_Point operator()(const ECZZ_p::Affine::EC_Point & from)
            { return from; }
    };
}
