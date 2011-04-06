#include "dss/dss_isoiec9796-3.hpp"

#include "generic/zz_utils.hpp"

using namespace NTL;
using namespace std;
using namespace ECZZ_p;
using namespace ECZZ_p::Affine;

int main(int argc     __attribute__((unused)),
         char *argv[] __attribute__((unused)))
{
    EC Curve = EC_Defaults::create(EC_Defaults::EC160, EC_Defaults::V2);

    fixedGenerator
        tG(I2OSP(ZZ_str("887ac5728a8390818b535fcbf04e827b0f8b543c")));

    Octet Xa(I2OSP(ZZ_str("d648bcb2e4d5d151656c84774ed016ba292a5a38")));
    Octet Ya(I2OSP(ZZ_str("e3daf39434a4b15c633a76ded8ada3de0a70bbe12e32d49c1b14fdc3efa071d7e864cf134b8d55af")));
    

    string M("This is a test message!");
    
    const StaticDataInputPolicy DefaultInputPolicy(10,
                                                   10, Hash::RIPEMD160);

    const StaticDataInputPolicy SampleInputPolicy(9,
                                                  10, Hash::RIPEMD160);

    ECKNR<ECZZ_p::Affine::EC,
          ECZZ_p::Affine::EC_Point> dss(Curve, tG,
                                        DefaultInputPolicy,
                                        Hash::RIPEMD160,
                                        MGF::MGF2);

    dss.setPrivateKey(Xa);
    dss.setPublicKey(Ya);
    
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
