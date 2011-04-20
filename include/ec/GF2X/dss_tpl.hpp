#pragma once

#include "ec/GF2X/affine/ec.hpp"
#include "ec/GF2X/affine/ec_compress.hpp"
#include "ec/GF2X/affine/ec_defaults.hpp"
#include "ec/GF2X/affine/utils.hpp"
#include "ec/GF2X/projective/ec.hpp"

struct DSS_GF2X
{
    typedef ECGF2X::Affine::EC aEC;
    typedef ECGF2X::Affine::EC_Point aECP;
    typedef ECGF2X::Affine::EC_Defaults aECD;
    typedef ECGF2X::Projective::EC pEC;
    typedef ECGF2X::Projective::EC_Point pECP;
};

namespace ECGF2X
{
    typedef DSS_GF2X DSSEC;
}

