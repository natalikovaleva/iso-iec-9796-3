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
#include <sys/time.h>

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

    fixedGenerator
        tG(I2OSP(ZZ_str("08a8bea9f2b40ce7400672261d5c05e5fd8ab326")));

    decombType::Generator decomb(8,3,NumBits(EC.getModulus()), tG);

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

    for (int i = 0; i<100; i ++)
    {
        const ZZ k = OS2IP(decomb.getRandomValue());
        if (!(G1*k == G2*k))
            {
                std::cout << "Incorrect computations!" << std::endl;
                return 1;
            }
    }

    struct timeval tv1, tv2;

    gettimeofday(&tv1, NULL);

    for (int i = 0; i<100000; i++)
    {
        const ZZ k = OS2IP(decomb.getRandomValue());
        const EC_Point kG1 = G1*k;
    }

    gettimeofday(&tv2, NULL);

    std::cout << "deComb: " << tv2.tv_sec - tv1.tv_sec << std::endl;

    gettimeofday(&tv1, NULL);

    for (int i = 0; i<100000; i++)
    {
        const ZZ k = OS2IP(decomb.getRandomValue());
        const EC_Point kG2 = G2*k;
    }

    gettimeofday(&tv2, NULL);

    std::cout << "Comb: " << tv2.tv_sec - tv1.tv_sec << std::endl;

    return 0;
}
