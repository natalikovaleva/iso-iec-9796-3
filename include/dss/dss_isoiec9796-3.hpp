#pragma once

/* Field convertor helper */

#include "generic/octet.hpp"

#define OS2FEP_TPL
template <class ECP>
struct tOS2FEP
{
    inline typename ECP::FE operator()(const ByteSeq & From);
};

#include "dss/dss_ecknr.hpp"
#include "dss/dss_ecnr.hpp"
#include "dss/dss_ecmr.hpp"
#include "dss/dss_ecao.hpp"
#include "dss/dss_ecpv.hpp"

class fixedGenerator : public generateRandomValueCallback
{
    const Octet _FixedValue;
public:
    fixedGenerator(const Octet & FixedValue)
        : _FixedValue(FixedValue)
        {}
    Octet getRandomValue()
        { return _FixedValue; }
};

