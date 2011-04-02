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

static const Hash    Hash(Hash::RIPEMD160);
static const MGF     MGF(MGF::MGF2, Hash::RIPEMD160);

int main(int argc     __attribute__((unused)),
         char *argv[] __attribute__((unused)))
{
    EC EC = EC_Defaults::create(EC_Defaults::EC160, EC_Defaults::V2);

    cout << EC << endl;

    EC.enter_mod_context(EC::FIELD_CONTEXT);

    const size_t Ln = L(EC.getOrder());

    cout << "Current modulus: " << ZZ_p::modulus() << endl;

    const ZZ Xa = ZZ_str("d648bcb2e4d5d151656c84774ed016ba292a5a38");
    const ZZ k  = ZZ_str("887ac5728a8390818b535fcbf04e827b0f8b543c");

    const ZZ e = InvMod(Xa, EC.getOrder());

    const EC_Point Y  = EC.getBasePoint() * e;
    const EC_Point PP = Y * k;

    const Octet CertData = I2OSP(Y.getX()) || I2OSP(Y.getY());
    const Octet Za = Truncate(CertData, Hash.getInputSize());

    cout << "Ln:          " << Ln << endl;
    cout << "Order:       " << I2OSP(EC.getOrder()) << endl;
    cout << "Generator:   " << EC.getBasePoint() << endl;
    cout << "Private key: " << I2OSP(Xa) << endl;
    cout << "Public key:  " << Y << endl;
    cout << "Session key: " << I2OSP(k) << endl;
    cout << "Randomizer:  " << PP << endl;
    cout << "Cert Data:   " << CertData << endl;
    cout << "Za:          " << Za << endl;

    const Octet P  = MGF(EC2OSP(PP, EC2OSP_COMPRESSED), Ln);

    cout << "Π : " << P << endl;

    EC.enter_mod_context(EC::ORDER_CONTEXT);

    string M("This is a test message!");

    const long L_rec = 10;
    const long L_red = 10;
    const long L_clr = M.length() - L_rec;

    cout << "Message: '" << M << "'" << endl;
    cout << "[ L_rec: " << L_rec << "; L_clr: "
         << L_clr << "; L_red: " << L_red << endl;

    const Octet   C_rec = I2OSP(L_rec, 8);
    const Octet   C_clr = I2OSP(L_clr, 8);

    cout << "C_rec: "  << C_rec << endl;
    cout << "C_clr: "  << C_clr << endl;

    const ByteSeq M_rec = ByteSeq((const unsigned char *)
                                  M.substr(0, L_rec).c_str(),
                                  L_rec);
    const ByteSeq M_clr = ByteSeq((const unsigned char *)
                                  M.substr(L_rec, L_clr).c_str(),
                                  L_clr);

    cout << "M_rec: "  << M_rec << endl;
    cout << "M_clr: "  << M_clr << endl;

    Octet Hash_Input = C_rec || C_clr || M_rec || M_clr || P;

    cout << "Hash Input: " << Hash_Input << endl;

    ByteSeq Hash_Token = Truncate(Hash(Hash_Input), L_red);

    cout << "Hash_Token: " << Hash_Token << endl;

    ByteSeq d = Hash_Token || M_rec;

    cout << "d: " << d << endl;

    const Octet m = MGF(Za || M_clr, Ln);

    cout << "MGF: " << m << endl;

    const Octet r = d ^ P ^ m;
    const ZZ_p  t = InMod(OS2IP(r));
    const ZZ_p  s = (InMod(k) - InMod(Xa)*t);

    cout << "t: " << I2OSP(t) << endl;

    const Octet S = I2OSP(s);

    cout << "R: " << r << endl;
    cout << "S: " << S << endl;

    return 0;
}
