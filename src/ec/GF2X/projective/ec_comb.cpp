#include "ec/GF2X/projective/ec.hpp"
#include "ec/GF2X/projective/ec_comb_index.hpp"

#include "ec/GF2X/affine/ec.hpp"
#include "ec/GF2X/affine/utils.hpp"

using namespace ECGF2X;
using namespace ECGF2X::Projective;

static inline Affine::EC_Point wdouble(const Affine::EC_Point & input,
                                       long window)
{
    Affine::EC_Point r(input);
    // std::cout << "wdouble(" << window << ")" << std::endl;
    
    for (long i=0; i<window; i++)
    {
        r += r;
    }

    return r;
}

EC_Point_Precomputations_Comb::EC_Point_Precomputations_Comb()
    : __precomputations_window(0),
      __precomputations_elements(0),
      __precomputations(NULL),
      __precomputations_max_size(0),
      __precomputations_portion(0)
{}

EC_Point_Precomputations_Comb::EC_Point_Precomputations_Comb(const EC_Point & Source)
    : __precomputations_window(comb_window),
      __precomputations_elements(1 << __precomputations_window),
      __precomputations(new Affine::EC_Point * [ __precomputations_elements ]),
      __precomputations_max_size(((NumBits(Source.getEC().getModulus()) +
                                   comb_window - 1) / comb_window) *
                                 comb_window),
      __precomputations_portion(__precomputations_max_size / __precomputations_window)
{
    Affine::EC_Point asAffine(toAffine(Source, Source.getEC()));
        
    /* Fill 0, 1, 2 */
    /* -------------------------------------------------------- */
    __precomputations[0] = new Affine::EC_Point(asAffine.getEC()); // Zero
    __precomputations[1] = new Affine::EC_Point(asAffine);         // This point
    /* ----------L----------------------------------------- */

    Affine::EC_Point * prev = __precomputations[1];
    
    for (long i = 1; i<__precomputations_window; i++)
    {
        __precomputations[1<<i] =
            new Affine::EC_Point(
                wdouble(*prev,
                        __precomputations_portion));
        
        prev = __precomputations[1<<i];
    }
    
   for (long i = 0; i<__precomputations_elements - __precomputations_window - 1; i++)
    {
        long t_idx = comb_idx[i*3];
        long f_idx = comb_idx[i*3 + 1];
        long s_idx = comb_idx[i*3 + 2];
        // std::cout << "B: " << t_idx << " = " << f_idx << " + " << s_idx << std::endl;

        __precomputations[t_idx] = new Affine::EC_Point(*__precomputations[f_idx] +
                                                        *__precomputations[s_idx]);
        
        // std::cout << "P: " << t_idx << " :: " << *__precomputations_own[t_idx] << std::endl;
        
    }
}

EC_Point_Precomputations_Comb::~EC_Point_Precomputations_Comb()
{

    // std::cout << "DELETE PRECOMPUTATIONS" << std::endl;
    
    for (long i = 0; i< __precomputations_elements; i++)
    {
        delete __precomputations[i];
        
        __precomputations[i] = NULL;
    }

    delete [] __precomputations;
}

long EC_Point_Precomputations_Comb::getMulPortions(void) const
{
    return __precomputations_portion;
}


/* Buffer -- NTL -- Big endian */
static inline long comb_build_index(const ZZ & Y,
                                    const long window,
                                    const long portions,
                                    const long iteration)
{
    long index = 0x0;

    for (long i = 0; i<window; i++)
    {
        index |=
            bit(Y, i*portions + iteration) << i;
        // std::cout << "Get bit: " << ( i*portions + iteration ) << std::endl;
    }
            
    // std::cout << "idx: " << index << std::endl;
    return index;
}


const Affine::EC_Point &
EC_Point_Precomputations_Comb::getPrecomputedForMul(const ZZ & scalar,
                                               const long portion) const
{
    if (NumBits(scalar) > __precomputations_max_size)
    {
        // std::cout << "Couldn't use precomputations!" << std::endl;

        throw;
    }

    return * __precomputations[ comb_build_index(scalar,
                                                 __precomputations_window,
                                                 __precomputations_portion,
                                                 __precomputations_portion - portion - 1) ];
}


/* TODO: Check, that Multiply compatible with point ? */
void
EC_Point_Precomputations_Comb::Multiply(EC_Point & P,
                                   const ZZ & Y) const
{
    // std::cout << "USING MIXED COMBO" << std::endl;
            
    P.setZero();
            
    for (long i = 0;
         i < getMulPortions();
         i++)
    {
        P += P;
        P += getPrecomputedForMul(Y, i);
    }
            
    // std::cout << "Is P On curve? " << P._IsOnCurve() << std::endl;
}

