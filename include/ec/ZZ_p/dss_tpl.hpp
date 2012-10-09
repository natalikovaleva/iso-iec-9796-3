#pragma once

#include "ec/ZZ_p/affine/ec.hpp"
#include "ec/ZZ_p/affine/ec_compress.hpp"
#include "ec/ZZ_p/affine/ec_defaults.hpp"
#include "ec/ZZ_p/affine/utils.hpp"
#include "ec/ZZ_p/projective/ec.hpp"

struct DSS_ZZ_p
{
    typedef ECZZ_p::Affine::EC aEC;
    typedef ECZZ_p::Affine::EC_Point aECP;
    typedef ECZZ_p::Affine::EC_Defaults aECD;
    typedef ECZZ_p::Projective::EC pEC;
    typedef ECZZ_p::Projective::EC_Point pECP;
};

namespace ECZZ_p
{
    typedef DSS_ZZ_p DSSEC;
}
