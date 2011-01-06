#include <iostream>
#include <NTL/ZZ.h>
#include <NTL/ZZ_p.h>

#include "hash.hpp"
#include "ec.hpp"
#include "ec_compress.hpp"
#include "ec_defaults.hpp"
#include "utils.hpp"

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

    std::cout << "Check order correctness: " << sample.isCorrectOrder() <<
        std::endl;

    std::cout << "Find point, with tY = 1" << std::endl;

    EC_Point Y = Q * sample.generate_random();
    
    while (EC_CPoint::compress_tY(Y) == 0)
        Y *= sample.generate_random();

    std::cout << "Y = " << Y << std::endl;
    
    EC_CPoint Yc (Y);

    std::cout << "Compress => Decompress Y = " << Yc.decompress(sample) << std::endl;

    unsigned char buffer[1024];

    Yc.serialize(buffer, sizeof(buffer));
    
    EC_CPoint Ycc(buffer, Yc.serializeSize());

    std::cout << "Decompress serialized Y = " << Ycc.decompress(sample) << std::endl;

    while (EC_CPoint::compress_tY(Y) == 1)
        Y *= sample.generate_random();

    std::cout << "Y' = " << Y << std::endl;

    EC_CPoint Yd (Y);

    std::cout << "Compress => Decompress Y' = " << Yd.decompress(sample) << std::endl;

    Yd.serialize(buffer, sizeof(buffer));
    
    EC_CPoint Ydd(buffer, Yd.serializeSize());

    std::cout << "Decompress serialized Y' = " << Ydd.decompress(sample) << std::endl;
    
    return 0;
}
