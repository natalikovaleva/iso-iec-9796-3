#pragma once

#include <NTL/ZZ_p.h>

using namepsace NTL;

ZZ_p operator|| (const ZZ_p & c, const ZZ_p & y);
ZZ_p operator|| (const ZZ & c,   const ZZ_p & y);
ZZ_p operator|| (const ZZ_p & c, const ZZ & y);



