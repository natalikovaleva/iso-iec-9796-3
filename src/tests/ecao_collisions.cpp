#include "dss/dss_isoiec9796-3.hpp"
#include "generic/zz_utils.hpp"

#include "algorithm/comb.hpp"

#include <signal.h>

using namespace NTL;
using namespace std;
using namespace ECZZ_p;
using namespace ECZZ_p::Affine;

static int bench = 0;

void alarm_SIGNAL(int sig __attribute__((unused)))
{
	cout << bench << " verifications / 10" << endl;
	bench = 0;
	alarm(10);
}

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
    EC Curve = EC_Defaults::create(EC_Defaults::EC192);

    long L_rec = 22;
    long L_pad = 1;

    cout << "Order bits: " << Lb(Curve.getOrder()) << endl;
    cout << "Modulus bits: " << Lb(Curve.getModulus()) << endl;

    NTLPRNG PRNG(Curve.getModulus());

    fixedGenerator
        tG(I2OSP(ZZ_str("722c12adafa68860758d37b1f23f5dadc292bdccd6373358")));

    Octet Xa(I2OSP(ZZ_str("a662ee3761adf2bbca1c16959b1de2a43d4cd1bf10937f21")));

    const string Message("123456789012345678901234567890");


    const Algorithm::Precomputations_Method_Comb<Projective::EC_Point,
                                                 ZZ,
                                                 Affine::EC_Point> Precomputation (NumBits(Curve.getModulus()));
    const ECAODataInputPolicy<DSS_ZZ_p> DefaultInputPolicy(L_rec, L_pad, Curve, Hash::SHA256);
    const DSSDomainParameters<DSS_ZZ_p> DomainParameters(Curve, DefaultInputPolicy, Precomputation);
    const DSSECAODomainParameters SchemeParameters(Hash(Hash::SHA256), MGF(MGF::MGF1, Hash::SHA256));

    ECAO<DSS_ZZ_p> dss(DomainParameters,
                       SchemeParameters,
                       PRNG);

    dss.setPrivateKey(Xa);
    dss.generatePublicKey();
    dss.buildPrecomputationTables();

    cout << "L_rec: " << DefaultInputPolicy(Message.length()).L_rec
         << "; L_red: " << DefaultInputPolicy(Message.length()).L_red
         << "; L_max: " << DefaultInputPolicy(Message.length()).L_max << endl;

    ByteSeq BaseMessage(ByteSeq(Message.c_str(), Message.length()));


    DigitalSignature sign =
        dss.sign(BaseMessage);

    cout << "R: " << sign.R << endl;
    cout << "S: " << sign.S << endl;

    VerificationVerdict v = dss.verify(sign);

    cout << "M: " << v.getMessage() << endl;
    cout << "V: " << v.isValid() << endl;

         if (!v.isValid())
             abort();

    cout << "Searching new signature" << endl;

    ZZ fakeR = OS2IP(sign.R);
    ZZ addR = (ZZ() + 1) << ((L_rec/2)*8);

    unsigned long long i = 0;

    struct sigaction act;
    act.sa_handler = alarm_SIGNAL;
    act.sa_flags=0x0;
    sigaction(SIGALRM, &act, NULL);
    alarm(10);

    do
    {
        fakeR += addR;
        i ++ ;
        bench ++;


        DigitalSignature fakeSign(I2OSP(fakeR, sign.R.getDataSize()), sign.S, sign.M_clr);
        VerificationVerdict verdict = dss.verify(fakeSign);

        if (verdict.isValid())
        {
            cout << "FAKE ECAO SIGN AT " << i << " PROBES. L_rec" << L_rec << "; Field: " << Lb(Curve.getModulus()) << endl;
            cout << "R: " << fakeSign.R << endl;
            cout << "S: " << fakeSign.S << endl;
            cout << "M_clr: " << fakeSign.M_clr << endl;
            cout << "M: " << verdict.getMessage() << endl;

            cout << "BASE SIGN: " << endl;
            cout << "R: " << sign.R << endl;
            cout << "S: " << sign.S << endl;
            cout << "M_clr: " << sign.M_clr << endl;
            cout << "M: " << BaseMessage << endl;

            return 1;
        }
    } while (1);


    return 0;
}
