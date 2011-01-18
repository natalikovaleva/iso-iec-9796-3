#include <string>
#include <sstream>

#include "affine/GF2X/ec.hpp"
#include "affine/GF2X/ec_defaults.hpp"
#include "affine/GF2X/utils.hpp"

using namespace std;
using namespace NTL;
using namespace Affine::GF2X;

const EC_Defaults::Sizes EC_Defaults::__size_matrix[] =
{
    EC0, EC185
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
    istringstream P;

    GF2X A, B, C,
        N, _P, GX, GY,
        SEED;
    
    
    switch (size)
    {
        case EC185:
            P.str("[1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1]");
            A  = GF2X_str("0x072546b5435234a422e0789675f432c89435de5242");
            B  = GF2X_str("0x00c9517d06d5240d3cff38c74b20b6cd4d6f9dd4d9");
            GX = GF2X_str("0x07af69989546103d79329fcc3d74880f33bbe803cb");
            GY = GF2X_str("0x01ec23211b5966adea1d3f87f7ea5848aef0b7ca9f");
            N  = GF2X_str("0x0400000000000000000001e60fc8821cc74daeafc1");
            C  = GF2X_str("0x0");
            SEED=GF2X_str("0x0");
            break;
            
        default:
            throw;
    }
    
    P >> _P;
    
    return EC(A, B, C,
              N, _P, GX, GY,
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

