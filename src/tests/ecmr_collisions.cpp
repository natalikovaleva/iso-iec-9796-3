#include "dss/dss_isoiec9796-3.hpp"
#include "generic/zz_utils.hpp"

#include "algorithm/comb.hpp"

using namespace NTL;
using namespace std;
using namespace ECGF2X;
using namespace ECGF2X::Affine;

class NTLPRNG : public generateRandomValueCallback
{
    const ZZ _N;
public:
    NTLPRNG(const ZZ & Modulus)
        : _N(Modulus)
        {
            ZZ seed;
            seed += time(NULL);
            SetSeed(seed);
        }
    ~NTLPRNG() {}
    Octet getRandomValue()
        {
            return I2OSP(RandomBnd(_N));
        }
};

int main(int argc     __attribute__((unused)),
         char *argv[] __attribute__((unused)))
{
    GF2X::HexOutput = 1;

    EC Curve = EC_Defaults::create(EC_Defaults::EC163);

    long L_rec = 18;

    cout << "Order bits: " << Lb(Curve.getOrder()) << endl;

    NTLPRNG PRNG(Curve.getOrder());

    Octet Xa(I2OSP(ZZ_str("2ddd259e3d30a77abc31cdf299a0e6cff7d78f869")));

    const string Message("123456789012345678901234567890");

    const Algorithm::Precomputations_Method_Comb<Projective::EC_Point,
                                                 ZZ,
                                                 Affine::EC_Point> Precomputation (NumBits(Curve.getModulus()));

    const ECMRDataInputPolicy<DSS_GF2X> DefaultInputPolicy(L_rec, Curve, Hash::SHA1);
    const DSSDomainParameters<DSS_GF2X> DomainParameters(Curve, DefaultInputPolicy, Precomputation);
    const DSSECMRDomainParameters SchemeParameters(Hash(Hash::SHA1), MGF(MGF::MGF1, Hash::SHA1));

    ECMR<DSS_GF2X> dss(DomainParameters,
                       SchemeParameters,
                       PRNG);

    dss.setPrivateKey(Xa);
    dss.generatePublicKey();
    dss.buildPrecomputationTables();

    cout << "L_rec: " << DefaultInputPolicy(Message.length()).L_rec
         << "; L_red: " << DefaultInputPolicy(Message.length()).L_red
         << "; L_max: " << DefaultInputPolicy(Message.length()).L_max << endl;

    Octet BaseMessage = ByteSeq(Message.c_str(), Message.length());

    DigitalSignature baseSign =
        dss.sign(BaseMessage);

    cout << "R: " << baseSign.R << endl;
    cout << "S: " << baseSign.S << endl;

    VerificationVerdict v = dss.verify(baseSign);

    cout << "M: " << v.Message << endl;
    cout << "V: " << v.isValid << endl;

    if (! v.isValid)
        abort();

    cout << "Searching new signature, changing first byte" << endl;

    ZZ fakeR = OS2IP(baseSign.R);
    ZZ addR = (ZZ() + 1) << ((L_rec/2)*8);

    unsigned long long i = 0;

    do
    {
        fakeR += addR;
        i ++ ;


        DigitalSignature fakeSign(I2OSP(fakeR, baseSign.R.getDataSize()), baseSign.S, baseSign.M_clr);
        VerificationVerdict verdict = dss.verify(fakeSign);

        if (verdict.isValid)
        {
            cout << "FAKE SIGN AT " << i << " PROBES" << endl;
            cout << "R: " << fakeSign.R << endl;
            cout << "S: " << fakeSign.S << endl;
            cout << "M_clr: " << fakeSign.M_clr << endl;
            cout << "M: " << verdict.Message << endl;

            cout << "BASE SIGN: " << endl;
            cout << "R: " << baseSign.R << endl;
            cout << "S: " << baseSign.S << endl;
            cout << "M_clr: " << baseSign.M_clr << endl;
            cout << "M: " << BaseMessage << endl;
        }
    } while (1);



    return 0;
}
