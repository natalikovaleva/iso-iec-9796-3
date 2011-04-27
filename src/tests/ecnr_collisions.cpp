#include "dss/dss_isoiec9796-3.hpp"
#include "generic/zz_utils.hpp"

#include "algorithm/comb.hpp"

#include <vector>

using namespace NTL;
using namespace std;
using namespace ECZZ_p;
using namespace ECZZ_p::Affine;

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
    EC Curve = EC_Defaults::create(EC_Defaults::EC160);

    const long L_rec = 18;

    cout << "Order bits: " << Lb(Curve.getOrder()) << endl;

    NTLPRNG PRNG(Curve.getModulus());

    Octet Xa(I2OSP(ZZ_str("24a3a993ab59b12ce7379a123487647e5ec9e0ce")));

    string M("123456789ABCDEF012");

    const Algorithm::Precomputations_Method_Comb<Projective::EC_Point,
                                                 ZZ,
                                                 Affine::EC_Point> Precomputation (NumBits(Curve.getModulus()));

    const ECDataInputPolicy<DSS_ZZ_p> DefaultInputPolicy(L_rec, Curve, Hash::RIPEMD160);
    const DSSDomainParameters<DSS_ZZ_p> DomainParameters(Curve, DefaultInputPolicy, Precomputation);

    ECNR<DSS_ZZ_p> dss(DomainParameters, PRNG);

    dss.setPrivateKey(Xa);
    dss.generatePublicKey();
    dss.buildPrecomputationTables();

    Octet BaseMessage = ByteSeq(M.c_str(), M.length());

    DigitalSignature baseSign = dss.sign(BaseMessage);
    if (! dss.verify(baseSign).isValid)
        abort();

    cout << "L_rec: " << DefaultInputPolicy(M.length()).L_rec
         << "; L_red: " << DefaultInputPolicy(M.length()).L_red
         << "; L_max: " << DefaultInputPolicy(M.length()).L_max << endl;

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
            cout << "M: " << verdictMessage << endl;

            cout << "BASE SIGN: " << endl;
            cout << "R: " << baseSign.R << endl;
            cout << "S: " << baseSign.S << endl;
            cout << "M_clr: " << baseSign.M_clr << endl;
            cout << "M: " << BaseMessage << endl;
        }
    } while (1);


    return 0;
}
