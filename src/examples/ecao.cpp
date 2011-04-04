#include <iostream>
#include <sstream>
#include <string>

#include "generic/octet.hpp"
#include "generic/hash.hpp"
#include "generic/mgf.hpp"

#include "dss/datain.hpp"

#include "ec/ZZ_p/affine/ec.hpp"
#include "ec/ZZ_p/affine/ec_compress.hpp"
#include "ec/ZZ_p/affine/ec_defaults.hpp"
#include "ec/ZZ_p/affine/utils.hpp"

#include "ec/ZZ_p/projective/ec.hpp"

using namespace NTL;
using namespace std;
using namespace ECZZ_p;
using namespace ECZZ_p::Affine;

#include "algorithm/comb.hpp"

#include <stdio.h>

static const MGF  MGF1(MGF::MGF1, Hash::SHA256);

int main(int argc     __attribute__((unused)),
         char *argv[] __attribute__((unused)))
{
    EC EC = EC_Defaults::create(EC_Defaults::EC192);

    cout << EC << endl;

    EC.enter_mod_context(EC::FIELD_CONTEXT);

    const size_t Ln = L(EC.getOrder());

    cout << "Current modulus: " << ZZ_p::modulus() << endl;

    const ZZ Xa = ZZ_str("a662ee3761adf2bbca1c16959b1de2a43d4cd1bf10937f21");
    const ZZ k  = ZZ_str("722c12adafa68860758d37b1f23f5dadc292bdccd6373358");

    const EC_Point Y  = EC.getBasePoint() * Xa;
    const EC_Point PP = EC.getBasePoint() * k;

    cout << "Generator:   " << EC.getBasePoint() << endl;
    cout << "Private key: " << I2OSP(Xa) << endl;
    cout << "Public key:  " << Y << endl;
    cout << "Session key: " << I2OSP(k) << endl;
    cout << "Randomizer:  " << PP << endl;

    const Octet P = EC2OSP(PP, EC2OSP_COMPRESSED);

    cout << "Π : " << P << endl;

    EC.enter_mod_context(EC::ORDER_CONTEXT);

    const size_t Lf = L(ZZ_p::modulus());

    const DataInputProvider ExampleStaticProvider(StaticDataInputPolicy(9, 12, Hash::SHA256, -1, Lf));
    const DataInput * ECAO_Data = ExampleStaticProvider.newDataInput(DataInputProvider::DATA_ECAO);

    string M("plaintext");

    DataInput::DSSDataInput SignData = ECAO_Data->createInput(M, P);

    const size_t K = Ln; // Security parameter

    const Octet r = SignData.d ^ P;
    const Octet u = MGF1(r || SignData.M_clr.getDataSize(), Ln + K);
    const ZZ_p  t = InMod(OS2IP(u));
    const ZZ_p  s = (InMod(k) - InMod(Xa)*t);

    cout << "u: " << u << endl;
    cout << "t: " << I2OSP(t) << endl;

    const Octet S = I2OSP(s);

    cout << "R: " << r << endl;
    cout << "S: " << S << endl;

    delete ECAO_Data;

    return 0;
}
