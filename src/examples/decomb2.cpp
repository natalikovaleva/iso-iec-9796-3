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

    if (argc !=2)
    {
        printf("Usage: %s [1-2^31]\n", argv[0]);
        return 1;
    }

    typedef Algorithm::Precomputations_deComb<
        Projective::EC_Point,
        Affine::EC_Point> decombType;

    EC EC = EC_Defaults::create(EC_Defaults::EC160);
    Projective::EC ECP(EC);
    
    SetSeed(ZZ());

    fixedGenerator
        tG(I2OSP(ZZ_str("08a8bea9f2b40ce7400672261d5c05e5fd8ab326")));

    decombType::Generator decomb(8,
                                 NumBits(EC.getModulus())/8/2,
                                 NumBits(EC.getModulus()), tG);

    const Algorithm::Precomputations_Method_deComb<
        Projective::EC_Point,
        Affine::EC_Point> Metod (decomb);

    const Algorithm::Precomputations_Method_Comb<
        Projective::EC_Point,
        ZZ,
        Affine::EC_Point> MetodOrig (NumBits(EC.getModulus()));

    Projective::EC_Point G1 = ECP.getBasePoint();
    Projective::EC_Point G2 = ECP.getBasePoint();

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

    for (int i = 0; i<atoi(argv[1]); i++)
    {
        const ZZ k = OS2IP(decomb.getRandomValue());
        const EC_Point kG1 = Algorithm::toAffine(G1*k);
    }

    gettimeofday(&tv2, NULL);

    std::cout << "deComb: " << tv2.tv_sec - tv1.tv_sec << std::endl;

    gettimeofday(&tv1, NULL);

    for (int i = 0; i<atoi(argv[2]); i++)
    {
        const ZZ k = OS2IP(decomb.getRandomValue());
        const EC_Point kG2 = Algorithm::toAffine(G2*k);
    }

    gettimeofday(&tv2, NULL);

    std::cout << "Comb: " << tv2.tv_sec - tv1.tv_sec << std::endl;

    return 0;
}
