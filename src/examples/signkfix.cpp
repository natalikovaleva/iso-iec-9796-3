#include <iostream>
#include <sstream>
#include <string>

#include "generic/octet.hpp"
#include "generic/hash.hpp"
#include "ec/ZZ_p/affine/ec.hpp"
#include "ec/ZZ_p/affine/ec_compress.hpp"
#include "ec/ZZ_p/affine/ec_defaults.hpp"
#include "ec/ZZ_p/affine/utils.hpp"

#include <stdio.h>

using namespace NTL;
using namespace std;

using namespace ECZZ_p::Affine;

static const ByteSeq C = I2OSP(1, 4);
static const Hash Hash(Hash::RIPEMD160);



int main(int argc     __attribute__((unused)),
         char *argv[] __attribute__((unused)))
{

    EC EC = EC_Defaults::create(EC_Defaults::EC160);

    cout << EC << endl;

    EC.enter_mod_context(EC::FIELD_CONTEXT);

    cout << "EC:N: " << I2OSP(EC.getOrder()) << endl;

    const size_t Ln = L(EC.getOrder());

    cout << "L_bits(N): " << Ln << endl;

    cout << "L(N): " << L(EC.getOrder()) << endl;

    cout << "Current modulus: " << ZZ_p::modulus() << endl;

    ZZ seed = ZZ() + time(NULL);
    SetSeed(seed);



    const ZZ Xa = RandomBnd(EC.getOrder());

    const EC_Point Y = EC.getBasePoint() * Xa;

    cout << "Private key: " << I2OSP(Xa) << endl;
    cout << "Public key: " << Y << endl;

    const ZZ k =  RandomBnd(EC.getOrder());
    const ZZ k1 = k + RandomBnd(EC.getOrder() / 2);

    const ZZ E  = k1-k;

    const EC_Point kG = EC.getBasePoint() * k;
    const EC_Point k1G = EC.getBasePoint() * k1;

    cout << "E: " << E << endl;

    cout << "Session key: " << I2OSP(k) << endl;
    cout << "Session key1: " << I2OSP(k1) << endl;

    cout << "kG: " << kG << endl;
    cout << "k1G"  << k1G << endl;

    const ByteSeq Pi = EC2OSP(kG,EC::EC2OSP_COMPRESSED);
    const ByteSeq Pi1 = EC2OSP(k1G,EC::EC2OSP_COMPRESSED);

    cout << "Π : " << Pi << endl;
    cout << "Π1 : " << Pi1 << endl;

    string M("This is a test message!");

    const long L_rec = 10;
    const long L_red = 9;
    const long L_clr = M.length() - L_rec;

    cout << "Message: '" << M << "'" << endl;
    cout << "[ L_rec: " << L_rec << "; L_clr: "
         << L_clr << "; L_red: " << L_red << endl;

    const Octet   C_rec = I2OSP(L_rec, 4);
    const Octet   C_clr = I2OSP(L_clr, 4);

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

    Octet Hash_Input = C_rec || C_clr || M_rec || M_clr || Pi || C;
    Octet Hash_Input1 = C_rec || C_clr || M_rec || M_clr || Pi1 || C;

    ByteSeq Hash_Token = Truncate(Hash(Hash_Input), L_red);
    ByteSeq Hash_Token1 = Truncate(Hash(Hash_Input1), L_red);

    cout << "Hash_Token: " << Hash_Token << endl;
    cout << "Hash_Token1: " << Hash_Token << endl;

    ByteSeq D = Hash_Token || M_rec;
    ByteSeq D1 = Hash_Token1 || M_rec;

    cout << "D: " << D << endl;
    cout << "D1: " << D1 << endl;

    EC.enter_mod_context(EC::ORDER_CONTEXT);

    const ZZ_p d = InMod(OS2IP(D));
    const ZZ_p d1 = InMod(OS2IP(D1));

    const ZZ_p pi = InMod(OS2IP(Pi));
    const ZZ_p pi1 = InMod(OS2IP(Pi));

    cout << "d: " << d << endl;
    cout << "π: " << pi << endl;
    cout << "d1: " << d1 << endl;
    cout << "π1: " << pi1 << endl;

    const ZZ_p r = (d + pi);
    const ZZ_p s = (InMod(k) - InMod(Xa)*r);

    const ZZ_p r1 = (d1 + pi1);
    const ZZ_p s1 = (InMod(k1) - InMod(Xa)*r1);

    cout << "r1: " << r1 << endl;
    cout << "s1: " << s1 << endl;
    cout << "r: " << r << endl;
    cout << "s: " << s << endl;

    const ByteSeq R = I2OSP(r,Ln);
    const ByteSeq S = I2OSP(s,Ln);


    cout << "EPSILON: " << E << endl;
    cout << "r1 - r: " << (r1 - r) << endl;

    ZZ_p Xx = (s1 - s - InMod(E)) / (r - r1);

    cout << "Xa: " << Xa << endl;
    cout << "Xx: " << Xx << endl;

    EC.leave_mod_context();

    return 0;
}
