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

    const ZZ Xa = ZZ_str("24a3a993ab59b12ce7379a123487647e5ec9e0ce");

    const EC_Point Y = EC.getBasePoint() * Xa;

    cout << "Private key: " << I2OSP(Xa) << endl;
    cout << "Public key: " << Y << endl;

    const ZZ k =  ZZ_str("08a8bea9g2b40ce7400672261d5c05e5fd8ab326");
    const ZZ k1 = ZZ_str("09a8bea9f2b40ce7400672261d5c05e5fd8ab326");

    const ZZ E  = k1-k;

    const EC_Point kG = EC.getBasePoint() * k;
    const EC_Point k1G = EC.getBasePoint() * k1;

    cout << "Session key: " << I2OSP(k) << endl;
    cout << "Session radical: " << kG << endl;

    const ByteSeq Pi = EC2OSP(kG,EC::EC2OSP_COMPRESSED);
    const ByteSeq Pi1 = EC2OSP(k1G,EC::EC2OSP_COMPRESSED);

    cout << "Π : " << Pi << endl;

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

    cout << "Hash Input: " << Hash_Input << endl;

    ByteSeq Hash_Token = Truncate(Hash(Hash_Input), L_red);
    ByteSeq Hash_Token1 = Truncate(Hash(Hash_Input1), L_red);

    cout << "Hash_Token: " << Hash_Token << endl;

    ByteSeq D = Hash_Token || M_rec;
    ByteSeq D1 = Hash_Token1 || M_rec;

    cout << "D: " << D << endl;

    EC.enter_mod_context(EC::ORDER_CONTEXT);

    const ZZ_p d = InMod(OS2IP(D));
    const ZZ_p d1 = InMod(OS2IP(D1));

    const ZZ_p pi = InMod(OS2IP(Pi));
    const ZZ_p pi1 = InMod(OS2IP(Pi));

    cout << "d: " << d << endl;
    cout << "π: " << pi << endl;

    const ZZ_p r = (d + pi);
    const ZZ_p s = (InMod(k) - InMod(Xa)*r);

    const ZZ_p r1 = (d1 + pi1);
    const ZZ_p s1 = (InMod(k1) - InMod(Xa)*r1);

    cout << "r: " << r << endl;
    cout << "s: " << s << endl;

    const ByteSeq R = I2OSP(r,Ln);
    const ByteSeq S = I2OSP(s,Ln);

    cout << "R: " << R << endl;
    cout << "S: " << S << endl;

    cout << "Current modulus: " << ZZ_p::modulus() << endl;


    cout << "EPSILON: " << E << endl;
    cout << "r1 - r: " << (r1 - r) << endl;


    ZZ_p Xx = (s1 - s - InMod(E)) / (r - r1);

    cout << "Xa: " << Xa << endl;
    cout << "Xx: " << Xx << endl;

    EC.leave_mod_context();

    return 0;
}