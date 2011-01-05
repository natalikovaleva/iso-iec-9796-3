#include <iostream>
#include <NTL/ZZ.h>
#include <NTL/ZZ_p.h>

#include "hash.hpp"
#include "ec.hpp"
#include "ec_compress.hpp"
#include "ec_defaults.hpp"


using namespace NTL;


int main(int argc, char *argv[])
{
    // std::cout << "Hello" << std::endl;

    ZZ_pBak bak; // Module switching: .save/.restore

    Hash_ZZ_p H(Hash::SHA512);
    
    EC_Defaults defaults(EC_Defaults::EC192);

    EC sample = defaults.create();

    sample.enter_mod_context();

    std::cout << "Sample EC: " << sample << std::endl;

    const EC_Point & G = sample.get_base_point();

    std::cout << "Base point: " << G << std::endl;

    ZZ_p d = sample.generate_random();

    std::cout << "Random d: " << d << std::endl;

    ZZ_p d1 = d + 1;

    std::cout << "Random d+1: " << d1 << std::endl;

    ZZ_p dH = H(d);

    std::cout << "Random d (hashed): " << dH << std::endl;

    ZZ_p d1H = H(d1);

    std::cout << "Random d+1 (hashed): " << d1H << std::endl;

    EC_Point Q  = G * d;

    std::cout << "Q = G*d = " << Q << std::endl;

    EC_CPoint Qc (Q);

    std::cout << "Compress => Decompress Q = " << Qc.decompress(sample) << std::endl;
    
    std::cout << "Check order correctness: " << sample.isCorrectOrder() <<
        std::endl;
    
    std::cout << "Serialization corectness" << std::endl;

    unsigned char buffer[1024];

    Qc.serialize(buffer, sizeof(buffer), sample);
    
    EC_CPoint Qcc (buffer, sizeof(buffer));

    std::cout << "Decompress serialized Q = " << Qcc.decompress(sample) << std::endl;
    
    
    return 0;
}
