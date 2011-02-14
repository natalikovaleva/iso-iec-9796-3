#include <string>
#include <sstream>

#include "affine/ZZ_p/ec.hpp"
#include "affine/ZZ_p/ec_defaults.hpp"
#include "affine/ZZ_p/utils.hpp"

using namespace std;
using namespace NTL;
using namespace Affine::ZZ_p;

const EC_Defaults::Sizes EC_Defaults::__size_matrix[] =
{
    EC0, EC128, EC160, EC192, EC256, EC512
};

EC_Defaults::EC_Defaults()
{}

EC_Defaults::~EC_Defaults()
{}

static ZZ_pBak __backup_context;

void EC_Defaults::restoreContext(void)
{
    __backup_context.restore();
    
}


EC EC_Defaults::create(Sizes size)
{
    
    ZZ P,N;
    ZZ GX, GY;
    ZZ A, B, C, SEED;
    
    __backup_context.save();
        
    switch (size)
    {
        case EC160:
            P = ZZ_str("FFD5D55FA9934410D3EB8BC04648779F13174945");
            A = ZZ_str("710062DCB53DC6E42F8227A4FBAC2240BD3504D4");
            B = ZZ_str("4163E75BB92147D54E09B0F13822B076A0944359");
            GX= ZZ_str("3C1E27D71F992260CF3C31C90D80B635E9FD0E68");
            GY= ZZ_str("C436EFC0041BBF0947A304A005F8D43A36763031");
            N = ZZ_str  ("2AA3A38FF1988B58235241EE59A73F4646443245");
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

