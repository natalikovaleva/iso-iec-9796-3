#pragma once

#include <NTL/GF2X.h>
#include <NTL/ZZ.h>
#include <NTL/ZZ_p.h>

#include <ostream>
#include <memory>

#include "ec/GF2X/affine/ec.hpp"

namespace ECGF2X
{
    namespace Projective
    {
        using NTL::GF2X;
        using NTL::GF2XModulus;
        using NTL::ZZ;

        class EC_Point_Precomputations_Comb : public EC_Point_Precomputations_Logic
        {
            const long __precomputations_window;
            const long __precomputations_elements;
            
            Affine::EC_Point ** __precomputations;
            
            const long __precomputations_max_size;
            const long __precomputations_portion;

        private:
            EC_Point_Precomputations_Comb(EC_Point_Precomputations_Comb & Source);
            
        public:
            EC_Point_Precomputations_Comb(); /* Empty precomputations. Totaly useless */
            EC_Point_Precomputations_Comb(const EC_Point & Source);
            ~EC_Point_Precomputations_Comb();

        public:
            void Multiply(EC_Point & P,
                          const ZZ & Y) const;
            inline bool isReady(void) 
                { return true; }
            
            
        private:
            long getMulPortions(void) const;
            const Affine::EC_Point & getPrecomputedForMul(const ZZ & scalar,
                                                          const long portion) const;
        };
    }
}

