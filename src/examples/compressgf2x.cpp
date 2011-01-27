#include "affine/GF2X/ec.hpp"
#include "affine/ZZ_p/utils.hpp"
#include "affine/GF2X/ec_defaults.hpp"
#include "affine/GF2X/ec_compress.hpp"

#include <iostream>

using namespace Affine::GF2X;
using namespace std;

using Affine::ZZ_p::ZZ_str;

int main(int argc     __attribute__ ((unused)),
         char *argv[] __attribute__ ((unused)))
{
    GF2X::HexOutput = 1;

    EC EC = EC_Defaults::create(EC_Defaults::EC163);

    const EC_Point G = EC.getBasePoint();
       
    cout << G << endl;

    EC_CPoint Gc(G);

    EC_Point sample = Gc.decompress(EC);

    cout << sample << endl;

    ZZ n = ZZ_str("abcdef");

    cout << "Is decompressed point on curve: " << sample._IsOnCurve() << endl;

    if ( ( sample * n ) == ( G * n) )
    {
        cout << "Decompression: OK" << endl;
        return 0;
    }
    else
    {
        cout << "Decompression: FAIL" << endl;
        return 1;
    }
    
}


