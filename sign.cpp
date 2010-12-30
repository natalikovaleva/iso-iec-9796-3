#include <iostream>
#include <NTL/ZZ.h>
#include <NTL/ZZ_p.h>

#include "ec.hpp"
#include "ec_defaults.hpp"


using namespace NTL;


int main(int argc, char *argv[])
{
    // std::cout << "Hello" << std::endl;

    ZZ_pBak bak; // Module switching: .save/.restore

    EC_Defaults defaults(EC_Defaults::EC192);

    EC sample = defaults.create();

    sample.enter_mod_context();

    // std::cout << "Sample EC: " << sample << std::endl;

    const EC_Point & G = sample.get_base_point();

    // std::cout << "Base point: " << G << std::endl;

    ZZ_p d = sample.generate_random();

    // std::cout << "Random d: " << d << std::endl;
    
    EC_Point Q  = G * d;

    // std::cout << "Q = G*d = " << Q << std::endl;

    std::cout << "Check order correctness: " << sample.isCorrectOrder() <<
        std::endl;
    
    
    return 0;
}
