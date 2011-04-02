#include <string>
#include <sstream>

#include "ec/GF2X/affine/ec.hpp"
#include "ec/GF2X/affine/ec_defaults.hpp"
#include "ec/GF2X/affine/utils.hpp"
#include "ec/ZZ_p/affine/utils.hpp"

using namespace std;
using namespace NTL;
using namespace ECGF2X::Affine;

using ECZZ_p::Affine::ZZ_str;
using NTL::ZZ;


const EC_Defaults::Sizes EC_Defaults::__size_matrix[] =
{
    EC0, EC163, EC283, EC571
};

EC_Defaults::EC_Defaults()
{}

EC_Defaults::~EC_Defaults()
{}


void EC_Defaults::restoreContext(void)
{
}


EC EC_Defaults::create(Sizes size, Version ver)
{
    GF2X A, B, C,
        P, GX, GY,
        SEED;
    ZZ N;



    switch (size)
    {
        case EC163:
            switch (ver)
            {
                case V1:
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

        case EC571:
            switch (ver)
            {
                case V1:

                    SetCoeff(P, 571);
                    SetCoeff(P, 10);
                    SetCoeff(P, 5);
                    SetCoeff(P, 2);
                    SetCoeff(P, 0);

                    {
                        istringstream N_("3864537523017258344695351890931987344298927329706434998657235251451519142289560424536143999389415773083133881121926944486246872462816813070234528288303332411393191105285703");

                        N_ >> N;
                    }


                    A  = GF2X_str("1");
                    B  = GF2X_str("2f40e7e2221f295de297117b7f3d62f5c6a97ffcb8ceff1cd6ba8ce4a9a18ad84ffabbd8efa59332be7ad6756a66e294afd185a78ff12aa520e4de739baca0c7ffeff7f2955727a");

                    GX = GF2X_str("303001d34b856296c16c0d40d3cd7750a93d1d2955fa80aa5f40fc8db7b2abdbde53950f4c0d293cdd711a35b67fb1499ae60038614f1394abfa3b4c850d927e1e7769c8eec2d19");
                    GY = GF2X_str("37bf27342da639b6dccfffeb73d69d78c6c27a6009cbbca1980f8533921e8a684423e43bab08a576291af8f461bb2a8b3531d2f0485c19b16e2f1516e23dd3c1a4827af1b8ac15b");

                    C  = GF2X_str("0x0");
                    SEED=GF2X_str("0x0");
                    break;
                default:
                    throw;
            }

        case EC283:
            switch (ver)
            {
                case V1:

                    SetCoeff(P, 283);
                    SetCoeff(P, 12);
                    SetCoeff(P, 7);
                    SetCoeff(P, 5);
                    SetCoeff(P, 0);

                    {
                        istringstream N_("7770675568902916283677847627294075626569625924376904889109196526770044277787378692871");
                        N_ >> N;
                    }

                    A  = GF2X_str("1");
                    B  = GF2X_str("27b680ac8b8596da5a4af8a19a0303fca97fd7645309fa2a581485af6263e313b79a2f5");

                    GX = GF2X_str("5f939258db7dd90e1934f8c70b0dfec2eed25b8557eac9c80e2e198f8cdbecd86b12053");
                    GY = GF2X_str("3676854fe24141cb98fe6d4b20d02b4516ff702350eddb0826779c813f0df45be8112f4");

                    C  = GF2X_str("0x0");
                    SEED=GF2X_str("0x0");

                    break;
                default:
                    throw;
            }

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

