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

        class EC;
        class EC_Point;
        
        class EC_Point_Precomputations_Logic
        {
        public:
            EC_Point_Precomputations_Logic() {};
            virtual ~EC_Point_Precomputations_Logic() {};
            virtual bool isReady(void) = 0;
            virtual void Multiply(EC_Point & P,
                                  const ZZ & Y) const = 0;
        };
        
        class EC_Point_Precomputations
        {
        private:
            long * __ref_counter;
            const bool __can_be_used;

            EC_Point_Precomputations_Logic * __logic;
            
        protected:
            inline long * get(void) 
                { std::cout << "GET" << std::endl;
                    if   (! __ref_counter) return NULL;
                    else (* __ref_counter) ++ ; return __ref_counter;
                }
            
            inline long * drop (void)
                { std::cout << "DROP" << std::endl;
                    if (! __ref_counter) return NULL;
                    else (* __ref_counter) -- ; return __ref_counter; }

            inline bool isLastUser (void)
                { return (! __ref_counter) ? false :
                        ( ( * __ref_counter) == 1 ); }
            
        public:
            EC_Point_Precomputations * operator= (EC_Point_Precomputations * Source)
                {
                    std::cout << "operator= on precomputations" << std::endl;
                    Source->get(); return Source; }
            
        public:
            bool isReady(void) const
                { return __can_be_used; }

        private:
            EC_Point_Precomputations();

        public:
            EC_Point_Precomputations(EC_Point_Precomputations_Logic * Logic)
                : __ref_counter(new long(0)),
                  __can_be_used(true),
                  __logic(Logic) {}
            
            EC_Point_Precomputations(EC_Point_Precomputations & Source)
                : __ref_counter(Source.get()),
                  __can_be_used(true),
                  __logic(Source.__logic) {}
            
            ~EC_Point_Precomputations() { drop(); if (isLastUser()) delete __logic; }

        public:
            void Multiply(EC_Point & P,
                          const ZZ & Y) const
                {
                    if (__logic)
                        __logic->Multiply(P, Y);
                    else throw;
                }
            
        };
    }
}

#include "ec/GF2X/projective/ec_comb.hpp"
// #include "ec/GF2X/projective/ec_comb2.hpp"

