#include "dss/dss_dstu4145.hpp"

#include "algorithm/comb.hpp"

using namespace NTL;
using namespace std;
using namespace ECZZ_p;
using namespace ECZZ_p::Affine;

int main()
{
    EC Curve = EC_Defaults::create(EC_Defaults::EC160);

    fixedGenerator
        tG(I2OSP(ZZ_str("87ac5728a8390818b535fcbf04e827b0f8b543c")));

    Hash Sha512(Hash::SHA512);
    Octet Xa(I2OSP(ZZ_str("648bcb2e4d5d151656c84774ed016ba292a5a38")));

    string M("This is a test message!");

    const Algorithm::Precomputations_Method_Comb
        <Projective::EC_Point, ZZ, Affine::EC_Point>
        Precomputation (NumBits(Curve.getModulus()));

    const ECDSSDomainParameters<DSS_ZZ_p> DomainParameters
        (Curve, Precomputation);

    DSTU4145<DSS_ZZ_p> dss(DomainParameters, Sha512, tG);

    dss.setPrivateKey(Xa);
    dss.generatePublicKey();

    dss.buildPrecomputationTables();

    DigitalSignature sign =
        dss.sign(ByteSeq(M.c_str(), M.length()));

    cout << "R: " << sign.R << endl;
    cout << "S: " << sign.S << endl;

    VerificationVerdict v = dss.verify(sign);

    cout << "M: " << v.Message << endl;
    cout << "V: " << v.isValid << endl;


    return 0;
}

