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
    EC Curve = EC_Defaults::create(EC_Defaults::EC160);

    const long L_rec = 10;

    cout << "Order bits: " << Lb(Curve.getOrder()) << endl;

    fixedGenerator
        tG(I2OSP(ZZ_str("08a8bea9f2b40ce7400672261d5c05e5fd8ab326")));

    Octet Xa(I2OSP(ZZ_str("24a3a993ab59b12ce7379a123487647e5ec9e0ce")));

    string M("This is a test message!");

    const Algorithm::Precomputations_Method_Comb<Projective::EC_Point,
                                                 ZZ,
                                                 Affine::EC_Point> Precomputation (NumBits(Curve.getModulus()));

    const ECDataInputPolicy<DSS_ZZ_p> DefaultInputPolicy(L_rec, Curve, Hash::RIPEMD160);
    const DSSDomainParameters<DSS_ZZ_p> DomainParameters(Curve, DefaultInputPolicy, Precomputation);

    ECNR<DSS_ZZ_p> dss(DomainParameters, tG);

    dss.setPrivateKey(Xa);
    dss.generatePublicKey();
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
