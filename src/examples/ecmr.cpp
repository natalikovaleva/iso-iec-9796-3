#include <iostream>
#include <sstream>
#include <string>

#include "generic/octet.hpp"
#include "generic/hash.hpp"
#include "generic/mgf.hpp"

#include "dss/datain_isoiec9796-3.hpp"

#include "ec/GF2X/affine/ec.hpp"
#include "ec/GF2X/affine/ec_defaults.hpp"
#include "ec/GF2X/affine/utils.hpp"

#include <stdio.h>

using namespace NTL;
using namespace std;

using namespace ECGF2X::Affine;

static const Hash Hash(Hash::SHA1);

int main(int argc     __attribute__((unused)),
         char *argv[] __attribute__((unused)))
{
    GF2X::HexOutput = 1;

    EC EC = EC_Defaults::create(EC_Defaults::EC163);

    const StaticDataInputPolicy InputPolicy(10, 11, L(EC.getOrder()), Hash::SHA1);

    cout << EC << endl;

    cout << "EC:N: " << I2OSP(EC.getOrder()) << endl;

    const size_t Ln = L(EC.getOrder());

    cout << "L(N): " << Ln << endl;

    cout << "L_b(N): " << Lb(EC.getOrder()) << endl;

    cout << "Order correctness: " << EC.isCorrectOrder() << endl;

    const ZZ Xa = ZZ_str("2ddd259e3d30a77abc31cdf299a0e6cff7d78f869");
    const ZZ k  = ZZ_str("397e49b664b13079fa8f2992e5bcdb38d6895a31b");

    cout << "k: " << I2OSP(k) << endl;

    const EC_Point R = EC.getBasePoint() * k;

    cout << "kG: " << R << endl;

    const ByteSeq OPoint = EC2OSP(R, EC::EC2OSP_UNCOMPRESSED);

    cout << "Octet Point: " << OPoint << endl;

    const MGF MGF1(MGF::MGF1, Hash::SHA1);

    const ByteSeq Pi = MGF1(OPoint, Ln);

    cout << "Π : " << Pi << endl;

    const TDataInput<ECMR_Input> ECMR_Data(InputPolicy);

    const string Message("TestVector");

    DSSDataInput SignData = ECMR_Data.createInput(Message, Pi);

    const Octet r = SignData.d ^ Pi;

    cout << "R: " << r << endl;

    EC.enter_mod_context(EC::ORDER_CONTEXT);

    const ZZ_p s = InMod(OS2IP(r) * k - OS2IP(r) - 1) / InMod(Xa + 1);

    EC.leave_mod_context();

    const Octet S = I2OSP(s, Ln);

    cout << "S: " << S << endl;

    return 0;
}

