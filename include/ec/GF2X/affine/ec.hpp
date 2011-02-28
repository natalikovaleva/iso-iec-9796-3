#pragma once

#include <NTL/GF2X.h>
#include <NTL/ZZ.h>
#include <NTL/ZZ_p.h>

#include <ostream>

namespace ECGF2X
{
    namespace Affine
    {
        
        using NTL::GF2X;
        using NTL::ZZ;
        using NTL::GF2XModulus;
        using NTL::random_GF2X;
        using NTL::ZZ_pContext;
        
        class EC;

        class EC_Point
        {
    
            GF2X X;
            GF2X Y;

            const EC & __EC;

            bool isZeroPoint;

        public:
            EC_Point(const GF2X &X, const GF2X &Y, const EC & __EC); // Generic
            EC_Point(const EC_Point & Point); // Same point in same field
            EC_Point(const EC_Point & Point, bool isZero); // Copy zero point from same field
            EC_Point(const EC & __EC); // Zero
            ~EC_Point();

            bool _IsOnCurve() const;
    
        public:

            inline bool isZero() const
                { return isZeroPoint; }
    
            EC_Point & operator=  (const EC_Point & Y);
            EC_Point   operator+  (const EC_Point & Y) const;
            EC_Point   operator*  (const ZZ & Y) const;
            EC_Point   operator*  (const long Y) const;

            void operator+= (const EC_Point & Y);
            void operator*= (const ZZ & Y);
            void operator*= (const long Y);

            inline bool operator== (const EC_Point &Y_)
                { return Y_.isZero() ? isZero() : (X == Y_.X) && (Y == Y_.Y); }

            inline const GF2X & getX() const
                { return X; }

            inline const GF2X & getY() const
                { return Y; }
        
            inline const EC & getEC() const
                { return __EC; }

            inline bool isSameEC(const EC & __EC) const;

            friend class ECGF2X::Affine::EC;
            friend std::ostream& ECGF2X::Affine::operator<<(std::ostream& s,
                                                          const EC_Point & _EC_Point);
        };

        class EC
        {
            ZZ_pContext __order; // Order Modulus context
            ZZ_pContext __global;
            bool __is_global_setted;

            const ZZ   N;     // Point Order
            const GF2X P;     // Modulus, get from creators context
            const GF2X Seed;  // Random generated seed
            const GF2X A;     // EC Params 
            const GF2X B;     // EC Params 
            const GF2X C;     // EC Params 
    
            const GF2XModulus P_mod;
            const long        P_deg;

            /* Depends on P_mod */
            
            const EC_Point G; // Base point

            /* Types */
        public:
            enum MOD_CONTEXT
            {
                ORDER_CONTEXT
            };
    
    
            /* Getters */
        public:
            inline const GF2XModulus & getModulus() const 
                { return P_mod; }

            inline const GF2X & getA() const
                { return A; }

            inline const GF2X & getB() const
                { return B; }

            inline const GF2X & getC() const
                { return C; }
             
            inline const EC_Point & getBasePoint() const
                { return G; }

            inline const ZZ & getOrder() const
                { return N; }
    
        public:

            EC(const GF2X & A,
               const GF2X & B,
               const GF2X & C,
               const ZZ   & N,
               const GF2X & P,
               //--- TODO Something with it .. ---
               const GF2X & Gx,
               const GF2X & Gy,
               //---------------------------------
               const GF2X & Seed);
    
            ~EC();

            EC_Point create() const;
    
            EC_Point create(const GF2X & x,
                            const GF2X & y) const;
    
            inline const EC_Point & get_base_point(void) const { return G; } ;
        
            bool generate_random(GF2X & d) const;
            bool generate_random(ZZ & d) const;
            GF2X generate_random(void) const;

            inline void enter_mod_context()
                { if (! __is_global_setted) { __global.save(); __is_global_setted = true; }
                    __order.restore(); }
    
            inline void leave_mod_context() const
                {  if (__is_global_setted) __global.restore(); }

            bool isCorrectOrder() const;
    
            friend class ECGF2X::Affine::EC_Point;
            friend std::ostream& ECGF2X::Affine::operator<<(std::ostream& s, const EC & _EC);
        };

        std::ostream& operator<<(std::ostream& s, const EC & _EC);
        std::ostream& operator<<(std::ostream& s, const EC_Point & _EC_Point);

    }
}


