#include <iostream>
#include <sstream>
#include <string>

#include "generic/octet.hpp"
#include "generic/hash.hpp"
#include "generic/mgf.hpp"
#include "ec/GF2X/affine/ec.hpp"
#include "ec/GF2X/affine/ec_defaults.hpp"
#include "ec/GF2X/affine/utils.hpp"

#include <stdio.h>

using namespace NTL;
using namespace std;

using namespace ECGF2X::Affine;

static const Hash Hash(Hash::SHA1);

int main(int argc     __attribute__((unused)),
         char *argv[] __attribute__((unused)))
{
    GF2X::HexOutput = 1;

    EC EC = EC_Defaults::create(EC_Defaults::EC571);
    
    EC_Point * randoms[300];

    for (unsigned int i = 0; i<300; i++)
    {
        ZZ random; EC.generate_random(random);
        
        randoms[i] = new EC_Point(EC.getBasePoint() * random);
        cout << i << ": " << *(randoms[i]) << endl;
    }
    
    int time2, time1 = time(NULL);

    Octet OPoint;
    
    for (unsigned int i = 0; i<1000000; i++)
    {
        OPoint = Hash(EC2OSP(* (randoms [i % 300]), EC::EC2OSP_UNCOMPRESSED));
    }
    
    time2 = time(NULL);

    cout << "TIME Hash: " << ( time2 - time1 ) << endl;
    cout << "LAST: " << OPoint << endl;

    time1 = time(NULL);

    for (unsigned int i = 0; i<1000000; i++)
    {
        OPoint = EC2OSP(* (randoms [i % 300]), EC::EC2OSP_COMPRESSED);
    }
    
    time2 = time(NULL);

    cout << "TIME COMP: " << ( time2 - time1 ) << endl;
    cout << "LAST: " << OPoint << endl;

    time1 = time(NULL);
    
    for (unsigned int i = 0; i<1000000; i++)
    {
        OPoint = EC2OSP(*(randoms [i % 300]), EC::EC2OSP_UNCOMPRESSED);
    }
    
    time2 = time(NULL);

    cout  << "TIME UNCOMP: " << ( time2 - time1 )  << endl;
    cout  << "LAST: " << OPoint << endl;
    
    

    return 0;
}
