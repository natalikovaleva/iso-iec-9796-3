#include "dss/dss_isoiec9796-3.hpp"
#include "generic/zz_utils.hpp"

#include "algorithm/comb.hpp"

using namespace NTL;
using namespace std;
using namespace ECZZ_p;
using namespace ECZZ_p::Affine;

int main(int argc     __attribute__((unused)),
         char *argv[] __attribute__((unused)))
{
    EC Curve = EC_Defaults::create(EC_Defaults::EC160, EC_Defaults::V2);

    long L_rec = 10;

    cout << "Order bits: " << Lb(Curve.getOrder()) << endl;

    fixedGenerator
        tG(I2OSP(ZZ_str("887ac5728a8390818b535fcbf04e827b0f8b543c")));

    Octet Xa(I2OSP(ZZ_str("d648bcb2e4d5d151656c84774ed016ba292a5a38")));
    Octet Ya(I2OSP(ZZ_str("e3daf39434a4b15c633a76ded8ada3de0a70bbe12e32d49c1b14fdc3efa071d7e864cf134b8d55af")));

    string M("This is a test message!");

    const Algorithm::Precomputations_Method_Comb<Projective::EC_Point,
                                                 ZZ,
                                                 Affine::EC_Point> Precomputation (NumBits(Curve.getModulus()));

    const ECDataInputPolicy<DSS_ZZ_p> DefaultInputPolicy(L_rec, Curve, Hash::RIPEMD160);

    const DSSDomainParameters<DSS_ZZ_p> DomainParameters(Curve, DefaultInputPolicy, Precomputation);

    ECKNR<DSS_ZZ_p> dss(DomainParameters,
                        DSSECKNRDomainParameters(Hash(Hash::RIPEMD160),
                                                 MGF(MGF::MGF2, Hash::RIPEMD160)),
                        tG);

    dss.setPrivateKey(Xa);
    dss.setPublicKey(Ya);
    dss.buildPrecomputationTables();

    cout << "L_rec: " << DefaultInputPolicy(M.length()).L_rec
         << "; L_red: " << DefaultInputPolicy(M.length()).L_red
         << "; L_max: " << DefaultInputPolicy(M.length()).L_max << endl;

    DigitalSignature sign =
        dss.sign(ByteSeq(M.c_str(), M.length()));

    cout << "R: " << sign.R << endl;
    cout << "S: " << sign.S << endl;

    VerificationVerdict v = dss.verify(sign);

    cout << "M: " << v.getMessage() << endl;
    cout << "V: " << v.isValid() << endl;

    return 0;
}
