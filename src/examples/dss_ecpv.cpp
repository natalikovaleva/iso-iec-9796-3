#include "dss/dss_isoiec9796-3.hpp"

#include "generic/zz_utils.hpp"

using namespace NTL;
using namespace std;
using namespace ECZZ_p;
using namespace ECZZ_p::Affine;

int main(int argc     __attribute__((unused)),
         char *argv[] __attribute__((unused)))
{
    EC Curve = EC_Defaults::create(EC_Defaults::EC161);

    fixedGenerator
        tG(I2OSP(ZZ_str("00d8a0abc5b7a4029ac232cbcda16819e1b715f9f4")));

    Octet Xa(I2OSP(ZZ_str("00e6a080e0b2a7a850ba71d26c9606669a4b4a6c18")));

    /* ----------------------------------------------------- */

    string M("Test User 1");

    unsigned char M_clr_data[] = {0xfa, 0x2b, 0x0c, 0xbe, 0x77};

    const Octet  M_clr_octet = Octet((unsigned char *) M_clr_data,
                                     (size_t) sizeof(M_clr_data));

    const Octet Message = Octet((unsigned char *) M.c_str(),
                                M.length()) || M_clr_octet;

    /* ---------------------------------------------- */

    const StaticDataInputPolicy DefaultInputPolicy(13 - 2,
                                                   5  + 2,
                                                   Hash::SHA1);
    const long KSize = 18;
    
    const SymXor Sym(KSize);
    
    ECPV<EC,
         EC_Point> dss(Curve, tG,
                       DefaultInputPolicy,
                       Sym,
                       KSize,
                       Hash::SHA1);
    
    dss.setPrivateKey(Xa);
    dss.generatePublicKey();

    cout << Curve << endl;
    
    DigitalSignature sign =
        dss.sign(Message);

    cout << "R: " << sign.R << endl;
    cout << "S: " << OS2IP(sign.S) << endl;

    VerificationVerdict v = dss.verify(sign);

    cout << "M: " << v.getMessage() << endl;
    cout << "V: " << v.isValid() << endl;

    return 0;
}

