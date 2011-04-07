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
    EC Curve = EC_Defaults::create(EC_Defaults::EC192);

    long L_rec = 9;

    cout << "Order bits: " << Lb(Curve.getOrder()) << endl;
    cout << "Modulus bits: " << Lb(Curve.getModulus()) << endl;

    fixedGenerator
        tG(I2OSP(ZZ_str("722c12adafa68860758d37b1f23f5dadc292bdccd6373358")));

    Octet Xa(I2OSP(ZZ_str("a662ee3761adf2bbca1c16959b1de2a43d4cd1bf10937f21")));

    const string Message("plaintext");


    const Algorithm::Precomputations_Method_Comb<Projective::EC_Point,
                                                 ZZ,
                                                 Affine::EC_Point> Precomputation (NumBits(Curve.getModulus()));
    const ECAODataInputPolicy<DSS_ZZ_p> DefaultInputPolicy(L_rec, Curve, Hash::SHA256);
    const DSSDomainParameters<DSS_ZZ_p> DomainParameters(Curve, DefaultInputPolicy, Precomputation);
    const DSSECAODomainParameters SchemeParameters(Hash(Hash::SHA256), MGF(MGF::MGF1, Hash::SHA256));

    ECAO<DSS_ZZ_p> dss(DomainParameters,
                       SchemeParameters,
                       tG);

    dss.setPrivateKey(Xa);
    dss.generatePublicKey();
    dss.buildPrecomputationTables();

    cout << "L_rec: " << DefaultInputPolicy(Message.length()).L_rec
         << "; L_red: " << DefaultInputPolicy(Message.length()).L_red
         << "; L_max: " << DefaultInputPolicy(Message.length()).L_max << endl;

    DigitalSignature sign =
        dss.sign(ByteSeq(Message.c_str(), Message.length()));

    cout << "R: " << sign.R << endl;
    cout << "S: " << sign.S << endl;

    VerificationVerdict v = dss.verify(sign);

    cout << "M: " << v.getMessage() << endl;
    cout << "V: " << v.isValid() << endl;

    return 0;
}
