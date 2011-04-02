#include "ec/GF2X/affine/ec.hpp"
#include "ec/ZZ_p/affine/utils.hpp"
#include "ec/GF2X/affine/ec_defaults.hpp"
#include "ec/GF2X/affine/ec_compress.hpp"

#include <iostream>

using namespace ECGF2X::Affine;
using namespace std;

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


