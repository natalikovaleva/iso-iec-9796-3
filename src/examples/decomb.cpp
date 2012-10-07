using namespace std;

#include "ec/ZZ_p/affine/ec.hpp"
#include "ec/ZZ_p/affine/ec_compress.hpp"
#include "ec/ZZ_p/affine/ec_defaults.hpp"
#include "ec/ZZ_p/affine/utils.hpp"

#include "ec/ZZ_p/projective/ec.hpp"

#include "generic/zz_utils.hpp"
#include "algorithm/decomb.hpp"
#include "algorithm/comb.hpp"

#include <iostream>

using namespace NTL;
using namespace std;
using namespace ECZZ_p;
using namespace ECZZ_p::Affine;

int main(int argc     __attribute__((unused)),
         char *argv[] __attribute__((unused)))
{

    typedef Algorithm::Precomputations_deComb<
        Affine::EC_Point,
        Affine::EC_Point> decombType;

    EC EC = EC_Defaults::create(EC_Defaults::EC160);

    SetSeed(ZZ());

    decombType::Generator decomb(8,3,NumBits(EC.getModulus()));

    const Algorithm::Precomputations_Method_deComb<
        Affine::EC_Point,
        Affine::EC_Point> Metod (decomb);

    const Algorithm::Precomputations_Method_Comb<
        Affine::EC_Point,
        ZZ,
        Affine::EC_Point> MetodOrig (NumBits(EC.getModulus()));

    EC_Point G1 = EC.getBasePoint();
    EC_Point G2 = EC.getBasePoint();

    G1.precompute(Metod);
    G2.precompute(MetodOrig);

    for (int i = 0; i<20000; i++)
    {
        const ZZ k = OS2IP(decomb.getRandomValue());
        EC_Point kG1 = G1*k;
        EC_Point kG2 = G2*k;

        if (! (kG1 == kG2))
        {
            std::cout << "FAIL" << std::endl;
            std::cout << kG1 << std::endl;
            std::cout << kG2 << std::endl;
            return 1;
        }
    }



    return 0;
}
