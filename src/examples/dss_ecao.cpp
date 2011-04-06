#include "dss/dss_isoiec9796-3.hpp"

#include "generic/zz_utils.hpp"

using namespace NTL;
using namespace std;
using namespace ECZZ_p;
using namespace ECZZ_p::Affine;

int main(int argc     __attribute__((unused)),
         char *argv[] __attribute__((unused)))
{
    EC Curve = EC_Defaults::create(EC_Defaults::EC192);

    fixedGenerator
        tG(I2OSP(ZZ_str("722c12adafa68860758d37b1f23f5dadc292bdccd6373358")));

    Octet Xa(I2OSP(ZZ_str("a662ee3761adf2bbca1c16959b1de2a43d4cd1bf10937f21")));

    const string Message("plaintext");

    const StaticDataInputPolicy DefaultInputPolicy(9,
                                                   12, Hash::SHA256, -1,
                                                   L(Curve.getOrder()));

    ECAO<EC,
         EC_Point> dss(Curve, tG,
                       DefaultInputPolicy,
                       Hash::SHA256,
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

