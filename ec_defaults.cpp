#include "ec_defaults.hpp"

#include <string>
#include <sstream>

using namespace std;
using namespace NTL;

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
    istringstream A, B, C, P;
    istringstream SEED;
    istringstream N, GX, GY;
    
    switch (size)
    {
        case EC160:
            P.str("1460561292756498603293233702061390149339739277637");
            A.str("645124569210146932690244706073525923393765180628");
            B.str("373312328028029709155915040744237848078440481625");
            GX.str("343211939187247703360278081073954992441786175080");
            GY.str("1120187316498206318592251033297336489377990586417");
            N.str("243426882126083100548872686522511570103009817157");
            C.str("0");
            SEED.str("0");
            break;
            
        
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

    __backup_context.save();
        
    ZZ_p::init(_P);
    
    ZZ_p _GX, _GY;

    GX >> _GX;
    GY >> _GY;

    ZZ_p _A, _B, _C, _SEED;
    ZZ _N;
    
    
    A    >> _A;
    B    >> _B;
    C    >> _C;
    N    >> _N;
    SEED >> _SEED;

    return EC(_A, _B, _C,
              _N, _GX, _GY,
              _SEED);
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

