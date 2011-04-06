#include "dss/dss_isoiec9796-3.hpp"

#include "generic/zz_utils.hpp"

using namespace NTL;
using namespace std;
using namespace ECZZ_p;
using namespace ECZZ_p::Affine;

int main(int argc     __attribute__((unused)),
         char *argv[] __attribute__((unused)))
{
    EC Curve = EC_Defaults::create(EC_Defaults::EC160);

    fixedGenerator
        tG(I2OSP(ZZ_str("08a8bea9f2b40ce7400672261d5c05e5fd8ab326")));

    Octet Xa(I2OSP(ZZ_str("24a3a993ab59b12ce7379a123487647e5ec9e0ce")));

    string M("This is a test message!");
    
    const StaticDataInputPolicy DefaultInputPolicy(10,
                                                   9, Hash::RIPEMD160);

    ECNR<ECZZ_p::Affine::EC,
         ECZZ_p::Affine::EC_Point> dss(Curve, tG,
                                       DefaultInputPolicy);
    
    dss.setPrivateKey(Xa);
    dss.generatePublicKey();

    cout << Curve << endl;
    
    DigitalSignature sign =
        dss.sign(ByteSeq(M.c_str(), M.length()));

    cout << "R: " << sign.R << endl;
    cout << "S: " << sign.S << endl;

    VerificationVerdict v = dss.verify(sign);

    cout << "M: " << v.getMessage() << endl;
    cout << "V: " << v.isValid() << endl;

    return 0;
}

