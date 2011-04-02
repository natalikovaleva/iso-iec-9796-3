#include <iostream>
#include <sstream>
#include <string>

#include "generic/octet.hpp"
#include "generic/hash.hpp"
#include "generic/mgf.hpp"

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

static const Hash Hash(Hash::SHA256);
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

    string M("plaintext");

    const size_t L_red = 12;
    const size_t L_max = Lf - L_red;
    const size_t L_rec = M.length();
    const size_t L_clr = M.length() - L_rec;

    const ByteSeq M_rec = ByteSeq((const unsigned char *)
                                  M.substr(0, L_rec).c_str(),
                                  L_rec);
    const ByteSeq M_clr = ByteSeq((const unsigned char *)
                                  M.substr(L_rec, L_clr).c_str(),
                                  L_clr);

    cout << "M_rec: "  << M_rec << endl;
    cout << "M_clr: "  << M_clr << endl;

    const Octet pad = I2OSP(1, L_max + 1 - L_rec);
    const Octet M_pad = pad || M_rec;

    cout << "M_pad: " << M_pad << endl;

    const Octet h = Truncate(Hash(M_pad), L_red);
    const Octet d = h || (Truncate(Hash(h), Lf + 1 - L_red) ^ M_pad);

    cout << "d: " << d << endl;

    const size_t K = Ln; // Security parameter

    const Octet r = d ^ P;
    const Octet u = MGF1(r || L_clr, Ln + K);
    const ZZ_p  t = InMod(OS2IP(u));
    const ZZ_p  s = (InMod(k) - InMod(Xa)*t);

    cout << "u: " << u << endl;
    cout << "t: " << I2OSP(t) << endl;

    const Octet S = I2OSP(s);

    cout << "R: " << r << endl;
    cout << "S: " << S << endl;

    return 0;
}
