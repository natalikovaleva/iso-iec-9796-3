#include "ec/GF2X/affine/ec.hpp"
#include "ec/GF2X/projective/ec.hpp"
#include "ec/ZZ_p/affine/utils.hpp"
#include "ec/GF2X/affine/ec_defaults.hpp"
#include "ec/GF2X/affine/ec_compress.hpp"

#include "algorithm/comb.hpp"

#include <iostream>

using namespace ECGF2X;
using namespace ECGF2X::Affine;
using namespace std;

int main(int argc     __attribute__ ((unused)),
         char *argv[] __attribute__ ((unused)))
{
    GF2X::HexOutput = 1;

    EC EC = EC_Defaults::create(EC_Defaults::EC163);

    Projective::EC EC_p(EC);

    const EC_Point G = EC.getBasePoint();
    const Projective::EC_Point G_p  = EC_p.getBasePoint();
          Projective::EC_Point G_pp = EC_p.getBasePoint();

    ZZ seed;
    seed += time(NULL);
    SetSeed(seed);


    ZZ k;

    EC.generate_random(k);

    cout << "Original: " << G << endl;
    cout << "k: " << k << endl;
    cout << "O*k: " << G * k << endl;
    cout << "P*k => O: " << toAffine( G_p * k) << endl;

    cout << "Try to precompute" << endl;

    Algorithm::Precomputations_Method_Comb<Projective::EC_Point,
                                           ZZ,
                                           Affine::EC_Point> Method (NumBits(EC_p.getModulus()));

    G_pp.precompute(Method);

    cout << "PP*k => O:" << toAffine(G_pp * k) << endl;

    return 0;
}
