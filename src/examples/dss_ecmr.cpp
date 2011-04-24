#include "dss/dss_isoiec9796-3.hpp"
#include "generic/zz_utils.hpp"

#include "algorithm/comb.hpp"

using namespace NTL;
using namespace std;
using namespace ECGF2X;
using namespace ECGF2X::Affine;

int main(int argc     __attribute__((unused)),
         char *argv[] __attribute__((unused)))
{
    GF2X::HexOutput = 1;

    EC Curve = EC_Defaults::create(EC_Defaults::EC163);

    unsigned long L_rec = 10;

    cout << "Order bits: " << Lb(Curve.getOrder()) << endl;

    fixedGenerator
        tG(I2OSP(ZZ_str("397e49b664b13079fa8f2992e5bcdb38d6895a31b")));

    Octet Xa(I2OSP(ZZ_str("2ddd259e3d30a77abc31cdf299a0e6cff7d78f869")));

    const string Message("TestVector");

    const Algorithm::Precomputations_Method_Comb<Projective::EC_Point,
                                                 ZZ,
                                                 Affine::EC_Point> Precomputation (NumBits(Curve.getModulus()));

    const ECMRDataInputPolicy<DSS_GF2X> DefaultInputPolicy(L_rec, Curve, Hash::SHA1);
    const DSSDomainParameters<DSS_GF2X> DomainParameters(Curve, DefaultInputPolicy, Precomputation);
    const DSSECMRDomainParameters SchemeParameters(Hash(Hash::SHA1), MGF(MGF::MGF1, Hash::SHA1));

    ECMR<DSS_GF2X> dss(DomainParameters,
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

    cout << "M: " << v.Message << endl;
    cout << "V: " << v.isValid << endl;

    return 0;
}
