#pragma once

#include <NTL/ZZ_p.h>

#include "ec.hpp"

using namepsace NTL;

ZZ_p operator|| (const ZZ_p & c, const ZZ_p & y);
ZZ_p operator|| (const ZZ & c,   const ZZ_p & y);
ZZ_p operator|| (const ZZ_p & c, const ZZ & y);

ZZ_p pi(const EC_Point & X);
ZZ_p pi(const EC_CPoint & X);





