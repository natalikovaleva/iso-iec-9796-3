#include <iostream>
#include <sstream>
#include <string>

#include "generic/octet.hpp"
#include "generic/hash.hpp"
#include "generic/mgf.hpp"
#include "affine/GF2X/ec.hpp"
#include "affine/GF2X/ec_defaults.hpp"
#include "affine/GF2X/utils.hpp"

#include <stdio.h>

using namespace NTL;
using namespace std;

using namespace Affine::GF2X;

static const Hash Hash(Hash::SHA1);

int main(int argc     __attribute__((unused)),
         char *argv[] __attribute__((unused)))
{
    GF2X::HexOutput = 1;

    EC EC = EC_Defaults::create(EC_Defaults::EC163);

    cout << EC << endl;

    cout << "EC:N: " << I2OSP(EC.getOrder()) << endl;

    const size_t Ln = L(EC.getOrder());

    cout << "L(N): " << Ln << endl;

    cout << "L_b(N): " << Lb(EC.getOrder()) << endl;

    cout << "Order correctness: " << EC.isCorrectOrder() << endl;

    const ZZ Xa = ZZ_str("2ddd259e3d30a77abc31cdf299a0e6cff7d78f869");
    const ZZ k  = ZZ_str("397e49b664b13079fa8f2992e5bcdb38d6895a31b");

    const EC_Point R = EC.getBasePoint() * k;

    cout << "kG: " << R << endl;

    const ByteSeq OPoint = EC2OSP(R, EC2OSP_UNCOMPRESSED);

    cout << "Octet Point: " << OPoint << endl;

    const MGF MGF1(MGF::MGF1, Hash::SHA1);

    cout << "Π : " << MGF1(OPoint, Ln) << endl;
    return 0;
}
