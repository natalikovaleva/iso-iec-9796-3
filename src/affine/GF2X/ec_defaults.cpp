#include <string>
#include <sstream>

#include "affine/GF2X/ec.hpp"
#include "affine/GF2X/ec_defaults.hpp"
#include "affine/GF2X/utils.hpp"
#include "affine/ZZ_p/utils.hpp"

using namespace std;
using namespace NTL;
using namespace Affine::GF2X;

using Affine::ZZ_p::ZZ_str;
using NTL::ZZ;


const EC_Defaults::Sizes EC_Defaults::__size_matrix[] =
{
    EC0, EC163
};

EC_Defaults::EC_Defaults()
{}

EC_Defaults::~EC_Defaults()
{}


void EC_Defaults::restoreContext(void)
{
}


EC EC_Defaults::create(Sizes size)
{
    GF2X A, B, C,
        P, GX, GY,
        SEED;
    ZZ N;
    
    
    
    switch (size)
    {
        case EC163:
            P  = GF2X_str("800000000000000000000000000000000000000C9");
            N  = ZZ_str("40000000000000000000292fe77e70c12a4234c33");

            A  = GF2X_str("1");
            B  = GF2X_str("20a601907b8c953ca1481eb10512f78744a3205fd");
            GX = GF2X_str("3f0eba16286a2d57ea0991168d4994637e8343e36");
            GY = GF2X_str("0d51fbc6c71a0094fa2cdd545b11c5c0c797324f1");
            
            C  = GF2X_str("0x0");
            SEED=GF2X_str("0x0");
            break;
            
        default:
            throw;
    }
    
    return EC(A, B, C,
              N, P, GX, GY,
              SEED);
}


EC_Defaults::Sizes EC_Defaults::getSize(unsigned int id)
{
    if (id > sizeof(__size_matrix))
        return EC0;
    
    return __size_matrix[id];
}

unsigned int EC_Defaults::getId(Sizes size) 
{
    for (unsigned int i = 0; i<sizeof(__size_matrix); i++)
        if (__size_matrix[i] == size)
            return i;

    return getId(EC_Defaults::EC0); // throw ? 
}

