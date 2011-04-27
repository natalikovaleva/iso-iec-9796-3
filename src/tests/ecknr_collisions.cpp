#include "dss/dss_isoiec9796-3.hpp"
#include "generic/zz_utils.hpp"

#include "algorithm/comb.hpp"

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
    EC Curve = EC_Defaults::create(EC_Defaults::EC160, EC_Defaults::V2);

    long L_rec = 18;

    cout << "Order bits: " << Lb(Curve.getOrder()) << endl;

    NTLPRNG PRNG(Curve.getModulus());

    Octet Xa(I2OSP(ZZ_str("d648bcb2e4d5d151656c84774ed016ba292a5a38")));
    Octet Ya(I2OSP(ZZ_str("e3daf39434a4b15c633a76ded8ada3de0a70bbe12e32d49c1b14fdc3efa071d7e864cf134b8d55af")));

    string M("123456789012345678901234567890");

    const Algorithm::Precomputations_Method_Comb<Projective::EC_Point,
                                                 ZZ,
                                                 Affine::EC_Point> Precomputation (NumBits(Curve.getModulus()));

    const ECDataInputPolicy<DSS_ZZ_p> DefaultInputPolicy(L_rec, Curve, Hash::RIPEMD160);

    const DSSDomainParameters<DSS_ZZ_p> DomainParameters(Curve, DefaultInputPolicy, Precomputation);

    ECKNR<DSS_ZZ_p> dss(DomainParameters,
                        DSSECKNRDomainParameters(Hash(Hash::RIPEMD160),
                                                 MGF(MGF::MGF2, Hash::RIPEMD160)),
                        PRNG);

    dss.setPrivateKey(Xa);
    dss.setPublicKey(Ya);
    dss.buildPrecomputationTables();

    cout << "L_rec: " << DefaultInputPolicy(M.length()).L_rec
         << "; L_red: " << DefaultInputPolicy(M.length()).L_red
         << "; L_max: " << DefaultInputPolicy(M.length()).L_max << endl;

    Octet BaseMessage = ByteSeq(M.c_str(), M.length());

    DigitalSignature baseSign =
        dss.sign(BaseMessage);

    cout << "R: " << baseSign.R << endl;
    cout << "S: " << baseSign.S << endl;

    VerificationVerdict v = dss.verify(baseSign);

    cout << "M: " << v.Message << endl;
    cout << "V: " << v.isValid << endl;

    if (! dss.verify(baseSign).isValid)
        abort();

    cout << "Searching new signature:" << endl;

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
