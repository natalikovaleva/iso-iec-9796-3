#pragma once

#include "ec/ZZ_p/affine/ec.hpp"
#include "ec/ZZ_p/affine/ec_compress.hpp"
#include "ec/ZZ_p/affine/ec_defaults.hpp"
#include "ec/ZZ_p/affine/utils.hpp"
#include "ec/ZZ_p/projective/ec.hpp"

using namespace ECZZ_p;

struct DSS_ZZ_p
{
    typedef Affine::EC aEC;
    typedef Affine::EC_Point aECP;
    typedef Projective::EC pEC;
    typedef Projective::EC_Point pECP;
};
