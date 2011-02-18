#include "ec/GF2X/affine/ec.hpp"
#include "ec/GF2X/projective/ec.hpp"
#include "ec/ZZ_p/affine/utils.hpp"
#include "ec/GF2X/affine/ec_defaults.hpp"
#include "ec/GF2X/affine/ec_compress.hpp"
#include <iostream>

using namespace ECGF2X;
using namespace ECGF2X::Affine;
using namespace std;

using ECZZ_p::Affine::ZZ_str;

int main(int argc     __attribute__ ((unused)),
         char *argv[] __attribute__ ((unused)))
{
    GF2X::HexOutput = 1;

    EC EC = EC_Defaults::create(EC_Defaults::EC163);

    Projective::EC EC_p(EC);

    const EC_Point G = EC.getBasePoint();
    const Projective::EC_Point G_p = EC_p.getBasePoint();

    ZZ seed;
    seed += time(NULL);
    SetSeed(seed);

    
    ZZ k;
    
    EC.generate_random(k);

    cout << "Original: " << G << endl;
    cout << "k: " << k << endl;
    cout << "O*k: " << G * k << endl;
    cout << "P*k => O: " << toAffine( G_p * k, EC ) << endl;

    cout << "Try to precompute" << endl;

    Projective::EC_Point_Precomputations_Comb comb(G_p);

    Projective::EC_Point G_pp(G_p, & comb);
    
    cout << "PP*k => O:" << toAffine(G_pp * k, EC) << endl;

    return 0;
}
