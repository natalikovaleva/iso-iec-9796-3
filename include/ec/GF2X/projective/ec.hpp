#pragma once

#include <NTL/GF2X.h>
#include <NTL/ZZ.h>
#include <NTL/ZZ_p.h>

#include <ostream>
#include <memory>

#include "ec/GF2X/affine/ec.hpp"
#include "ec/GF2X/projective/ec_precomputations.hpp"


namespace ECGF2X
{
    namespace Projective
    {
        using NTL::GF2X;
        using NTL::GF2XModulus;
        using NTL::ZZ;

        class EC;
        
        class EC_Point
        {
    
            GF2X X;
            GF2X Y;
            GF2X Z;

            const EC & __EC;
            
            bool __isZeroPoint;
            
            EC_Point_Precomputations * __precomputations;
            
        public:

            /* Create point from point and it's precomputations */
            EC_Point(const EC_Point & Point,
                     EC_Point_Precomputations_Logic * Precomputations);
            
            EC_Point(const GF2X &X, const GF2X &Y, const GF2X &Z, const EC & __EC); // Projective
            EC_Point(const EC_Point & Point); // Same point in same field
            EC_Point(const EC_Point & Point, bool isZero); // Copy zero point from same field
            EC_Point(const EC & __EC); // Zero
            ~EC_Point();

            bool _IsOnCurve() const;
    
        public:
            bool precompute(void);
            
            inline bool isPrecomputed() const
                { if (__precomputations)
                        return __precomputations->isReady();
                    else
                        return false; }
            
            inline bool isZero() const
                { return __isZeroPoint; }

            inline void setZero() 
                { __isZeroPoint = true; }
    
            EC_Point & operator=  (const EC_Point & Y);
            EC_Point   operator+  (const EC_Point & Y) const;
            EC_Point   operator+  (const Affine::EC_Point & Y) const;
            EC_Point   operator*  (const ZZ & Y) const;
    
            void operator+= (const Affine::EC_Point & Y);
            void operator+= (const EC_Point & Y);
            void operator*= (const ZZ & Y);

            inline bool operator== (const EC_Point &Y_)
                { return Y_.isZero() ? isZero() : (X == Y_.X) && (Y == Y_.Y); }

            inline const GF2X & getX() const
                { return X; }

            inline const GF2X & getY() const
                { return Y; }
        
            inline const GF2X & getZ() const
                { return Z; }
        
            inline const EC & getEC() const
                { return __EC; }

            inline bool isSameEC(const EC & __EC) const;

            friend class EC;
            friend std::ostream& operator<<(std::ostream& s,
                                            const EC_Point & _EC_Point);
            friend inline void 
            Lopez_Dahab_Addition(EC_Point & P1,
                                 const EC_Point & P2);

            friend inline void 
            Lopez_Dahab_Mixed_Addition(EC_Point & P1,
                                       const Affine::EC_Point & P2);
            
            friend inline void 
            Lopez_Dahab_Double(GF2X & X,
                               GF2X & Y,
                               GF2X & Z,
                               const GF2X & A,
                               const GF2X & B,
                               const GF2XModulus & P);
        };


        class EC : public Affine::EC
        {
            EC_Point G; // Base point in GF2X/Projective
                        // Can be precomputed
            
            const Affine::EC_Point G_a;
            
        public:
            inline const EC_Point & getBasePoint() const
                { return G; }
            inline const Affine::EC_Point & getAffineBasePoint() const
                { return G_a; }

        public:
            /* This EC curve class exists only for precomputations.
             * Derive Projective::EC from Affine::EC */

            EC(const Affine::EC & EC);
            ~EC();
            
            EC_Point create() const;
    
            EC_Point create(const GF2X & x,
                            const GF2X & y,
                            const GF2X & z) const;

        };
    }

    Projective::EC_Point toProjective(const Affine::EC_Point & Point,
                                      const Projective::EC & EC);
    
    Affine::EC_Point toAffine(const Projective::EC_Point & Point,
                              const Affine::EC & EC);
}

