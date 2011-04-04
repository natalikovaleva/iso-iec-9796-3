#include <iostream>
#include <sstream>
#include <string>

#include "generic/octet.hpp"
#include "generic/hash.hpp"

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

int main(int argc     __attribute__((unused)),
         char *argv[] __attribute__((unused)))
{

    EC EC = EC_Defaults::create(EC_Defaults::EC160);

    Projective::EC EC_p(EC);
    Projective::EC_Point G_p(EC_p.getBasePoint());

    cout << EC << endl;

    EC.enter_mod_context(EC::FIELD_CONTEXT);

    cout << "EC:N: " << I2OSP(EC.getOrder()) << endl;

    const size_t Ln = L(EC.getOrder());

    cout << "L_bits(N): " << Ln << endl;

    cout << "L(N): " << L(EC.getOrder()) << endl;

    cout << "Current modulus: " << ZZ_p::modulus() << endl;

    const ZZ Xa = ZZ_str("24a3a993ab59b12ce7379a123487647e5ec9e0ce");

    const EC_Point Y = EC.getBasePoint() * Xa;
    const EC_Point Y_a = toAffine(G_p * Xa);

    cout << "Private key: " << I2OSP(Xa) << endl;
    cout << "Public key (norm): " << Y << endl;
    cout << "Public key (proj): " << Y_a << endl;



    cout << "Generator: " << EC.getBasePoint() << endl;

    cout << "Try to precompute" << endl;

    EC_Point G_pp = EC.getBasePoint();

    Algorithm::Precomputations_Method_Comb<EC_Point,
                                           ZZ,
                                           EC_Point> Method (NumBits(EC.getModulus()));

    G_pp.precompute(Method);

    cout << "Is EC Correct order: " << EC.isCorrectOrder() << endl;

    const ZZ k = ZZ_str("08a8bea9f2b40ce7400672261d5c05e5fd8ab326");
    const EC_Point kG = G_pp * k;

    cout << "Session key: " << I2OSP(k) << endl;
    cout << "Session radical: " << kG << endl;

    const ByteSeq Pi = EC2OSP(kG,EC2OSP_COMPRESSED);

    cout << "Π : " << Pi << endl;

    const DataInputProvider ExampleStaticProvider(StaticDataInputPolitic(10, 9, Hash::RIPEMD160));
    const DataInput * ECNR_Data = ExampleStaticProvider.newDataInput(DataInputProvider::DATA_ECNR);

    string M("This is a test message!");

    DataInput::DSSDataInput SignData = ECNR_Data->createInput(M, Pi);

    EC.enter_mod_context(EC::ORDER_CONTEXT);

    const ZZ_p d = InMod(OS2IP(SignData.d));
    const ZZ_p pi = InMod(OS2IP(Pi));

    cout << "d: " << d << endl;
    cout << "π: " << pi << " ( "<< I2OSP(pi) << ")" <<endl;

    const ZZ_p r = (d + pi);
    const ZZ_p s = (InMod(k) - InMod(Xa)*r);

    cout << "r: " << r << endl;
    cout << "s: " << s << endl;

    const ByteSeq R = I2OSP(r,Ln);
    const ByteSeq S = I2OSP(s,Ln);

    cout << "R: " << R << endl;
    cout << "S: " << S << endl;

    cout << "Current modulus: " << ZZ_p::modulus() << endl;

    EC.leave_mod_context();

    delete ECNR_Data;

    return 0;
}
