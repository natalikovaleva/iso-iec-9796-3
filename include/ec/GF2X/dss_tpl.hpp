#pragma once

#include "ec/GF2X/affine/ec.hpp"
#include "ec/GF2X/affine/ec_compress.hpp"
#include "ec/GF2X/affine/ec_defaults.hpp"
#include "ec/GF2X/affine/utils.hpp"
#include "ec/GF2X/projective/ec.hpp"

using namespace ECGF2X;

struct DSS_GF2X
{
    typedef Affine::EC aEC;
    typedef Affine::EC_Point aECP;
    typedef Projective::EC pEC;
    typedef Projective::EC_Point pECP;
};
