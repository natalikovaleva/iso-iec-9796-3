#include "dss/dss_isoiec9796-3.hpp"

#include "generic/zz_utils.hpp"

using namespace NTL;
using namespace std;
using namespace ECGF2X;
using namespace ECGF2X::Affine;

int main(int argc     __attribute__((unused)),
         char *argv[] __attribute__((unused)))
{
    GF2X::HexOutput = 1;
    
    EC Curve = EC_Defaults::create(EC_Defaults::EC163);

    fixedGenerator
        tG(I2OSP(ZZ_str("397e49b664b13079fa8f2992e5bcdb38d6895a31b")));

    Octet Xa(I2OSP(ZZ_str("2ddd259e3d30a77abc31cdf299a0e6cff7d78f869")));

    const string Message("TestVector");

    const StaticDataInputPolicy DefaultInputPolicy(10,
                                                   11, Hash::SHA1);

    ECMR<EC,
         EC_Point> dss(Curve, tG,
                       DefaultInputPolicy,
                       Hash::SHA1,
                       MGF::MGF1);
    
    dss.setPrivateKey(Xa);
    dss.generatePublicKey();

    cout << Curve << endl;
    
    DigitalSignature sign =
        dss.sign(ByteSeq(Message.c_str(), Message.length()));

    cout << "R: " << sign.R << endl;
    cout << "S: " << sign.S << endl;

    VerificationVerdict v = dss.verify(sign);

    cout << "M: " << v.getMessage() << endl;
    cout << "V: " << v.isValid() << endl;

    return 0;
}
