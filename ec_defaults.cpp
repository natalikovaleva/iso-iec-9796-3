#include "ec_defaults.hpp"

#include <string>
#include <sstream>

using namespace std;
using namespace NTL;

EC_Defaults::EC_Defaults(Sizes size)
    : __this_size(size)
{
    
}

EC_Defaults::~EC_Defaults()
{}

EC EC_Defaults::create() const
{
    istringstream A, B, C, P;
    istringstream SEED;
    istringstream N, GX, GY;
    
    switch (__this_size)
    {
        case EC192:
            P.str("6277101735386680763835789423207666416083908700390324961279");
            N.str("6277101735386680763835789423176059013767194773182842284081");
            A.str("-3");
            B.str("2455155546008943817740293915197451784769108058161191238065");
            C.str("1191689908718309326471930603292001425137626342642504031845");
            SEED.str("275585503993527016686210752207080241786546919125");
            GX.str("602046282375688656758213480587526111916698976636884684818");
            GY.str("174050332293622031404857552280219410364023488927386650641");

            break;
            
            
        default:
            throw;
    }
    
    ZZ   _P;
    P >> _P;

    ZZ_p::init(_P);
    
    ZZ_p _GX, _GY;

    GX >> _GX;
    GY >> _GY;

    ZZ_p _A, _B, _C, _N, _SEED;
    
    A    >> _A;
    B    >> _B;
    C    >> _C;
    N    >> _N;
    SEED >> _SEED;

    return EC(_A, _B, _C,
              _N, _GX, _GY,
              _SEED);
}


