#include <string>
#include <sstream>

#include "ec/ZZ_p/affine/ec.hpp"
#include "ec/ZZ_p/affine/ec_defaults.hpp"
#include "ec/ZZ_p/affine/utils.hpp"

#include <exception>

using namespace std;
using namespace NTL;
using namespace ECZZ_p::Affine;

const EC_Defaults::Sizes EC_Defaults::__size_matrix[] =
{
    EC0, EC160, EC161, EC192
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

EC EC_Defaults::create(Sizes size, Version ver)
{

    ZZ P,N;
    ZZ GX, GY;
    ZZ A, B, C, SEED;

    __backup_context.save();

    switch (size)
    {
        case EC160:
            switch (ver)
            {
                case V1:
                    P = ZZ_str("FFD5D55FA9934410D3EB8BC04648779F13174945");
                    A = ZZ_str("710062DCB53DC6E42F8227A4FBAC2240BD3504D4");
                    B = ZZ_str("4163E75BB92147D54E09B0F13822B076A0944359");
                    GX= ZZ_str("3C1E27D71F992260CF3C31C90D80B635E9FD0E68");
                    GY= ZZ_str("C436EFC0041BBF0947A304A005F8D43A36763031");
                    N = ZZ_str("2AA3A38FF1988B58235241EE59A73F4646443245");
                    break;

                case V2:
                    P = ZZ_str("FFFFFFFFFFFFFFFF0A13A2A0A085053B49A92B05");
                    A = ZZ_str("FFFFFFFFFFFFFFFF0A13A2A0A085053B49A92B02");
                    B = ZZ_str("809DC828D7EC47F1D1B2080062A9D350C3E7B230");
                    GX= ZZ_str("AE31677514F76709513C84424165D4400BB7D699");
                    GY= ZZ_str("71FA37B727CBD843C800D4741448267A8FDD047E");
                    N = ZZ_str("FFFFFFFFFFFFFFFF0A15341C63139E6C9E868967");
                    break;

                default:
                    throw std::exception();
            }
            break;

        case EC161:
            switch (ver)
            {
                case V1:
                    P = ZZ_str("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFAC73");
                    A = ZZ_str("0000000000000000000000000000000000000000");
                    B = ZZ_str("0000000000000000000000000000000000000007");
                    GX= ZZ_str("3B4C382CE37AA192A4019E763036F4F5DD4D7EBB");
                    GY= ZZ_str("938CF935318FDCED6BC28286531733C3F03C4FEE");
                    N = ZZ_str("0100000000000000000001B8FA16DFAB9ACA16B6B3");
                    break;
                default:
                    throw std::exception();
            }
            break;

        case EC192:
            switch (ver)
            {
                case V1:
                    P = ZZ_str("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFFFFFFFFFFFF");
                    A = ZZ_str("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFFFFFFFFFFFC");
                    B = ZZ_str("64210519E59C80E70FA7E9AB72243049FEB8DEECC146B9B1");
                    GX= ZZ_str("188DA80EB03090F67CBF20EB43A18800F4FF0AFD82FF1012");
                    GY= ZZ_str("07192B95FFC8DA78631011ED6B24CDD573F977A11E794811");
                    N = ZZ_str("FFFFFFFFFFFFFFFFFFFFFFFF99DEF836146BC9B1B4D22831");
                    break;
                default:
                    throw std::exception();
            }
            break;

        default:
            throw std::exception();
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
