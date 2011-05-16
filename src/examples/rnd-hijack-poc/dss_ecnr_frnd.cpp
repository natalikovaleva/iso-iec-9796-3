#include "dss/dss_isoiec9796-3.hpp"
#include "generic/zz_utils.hpp"

#include "algorithm/comb.hpp"

#include <alloca.h>
#include <stdio.h>

using namespace NTL;
using namespace std;
using namespace ECZZ_p;
using namespace ECZZ_p::Affine;

class fileGenerator : public generateRandomValueCallback
{
    FILE * input;
    long   size;

public:
    fileGenerator(const char * filepath, long size)
        {
            input = fopen(filepath, "r");
            if (input == NULL)
            {
                throw std::exception();
            }


            this->size = size;
        }

    ~fileGenerator()
    	{
            fclose(input);
        }

    Octet getRandomValue()
        {
            unsigned char * buffer = (unsigned char *) alloca(size);
            if (fread(buffer, size, 1, input) != 1)
            {
                std::cout << "size: " << size << std::endl;
                throw std::exception();
            }

            Octet blah(buffer, size);
            std::cout << "OCTET: " << blah << std::endl;
            return blah;

        }
};


int main(int argc     __attribute__((unused)),
         char *argv[] __attribute__((unused)))
{
    EC Curve = EC_Defaults::create(EC_Defaults::EC160);

    const unsigned long L_rec = 10;

    cout << "Order bits: " << Lb(Curve.getOrder()) << endl;

    fileGenerator
        tG("./data.urandom2", L(Curve.getOrder()));

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

    const int signs_count = 1100;

    DigitalSignature sign1 =
        dss.sign(ManagedBlob(M.c_str(), M.length()));

    cout << "R: " << sign1.R << endl;
    cout << "S: " << sign1.S << endl;


    ZZ t = OS2IP(sign1.R);
    ZZ s = OS2IP(sign1.S);

    Curve.enter_mod_context(DSS_ZZ_p::aEC::FIELD_CONTEXT);

    EC_Point publicKey = Curve.create(ZZ_str("00e564acae27d2271c4af829cface6decc8cdce6"),
                                      ZZ_str("7bd48ce108ffd3cfa38177f683b5bcf4fd97a4a9"));

    EC_Point dPoint = Curve.getBasePoint() * Curve.getOrder();

    DSS_ZZ_p::aECP R1 = publicKey * t + Curve.getBasePoint() * s;
    Curve.leave_mod_context();


    // return 0;

    for (int i = 0; i < signs_count; i ++)
    {
        DigitalSignature sign =
            dss.sign(ManagedBlob(M.c_str(), M.length()));

        cout << "R: " << sign.R << endl;
        cout << "S: " << sign.S << endl;

        VerificationVerdict v = dss.verify(sign);

        cout << "M: " << v.Message << endl;
        cout << "V: " << v.isValid << endl;

        ZZ tn = OS2IP(sign.R);
        ZZ sn = OS2IP(sign.S);

        Curve.enter_mod_context(DSS_ZZ_p::aEC::FIELD_CONTEXT);
        DSS_ZZ_p::aECP Rn = publicKey * t + Curve.getBasePoint() * s;

        if ((Rn + dPoint) == R1)
        {
            std::cout << "OK!" << std::endl;
            return 1;
        }


        Curve.leave_mod_context();


    }

    return 0;
}
