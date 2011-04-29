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
    EC Curve = EC_Defaults::create(EC_Defaults::EC161);

    fixedGenerator
        tG(I2OSP(ZZ_str("00d8a0abc5b7a4029ac232cbcda16819e1b715f9f4")));

    Octet Xa(I2OSP(ZZ_str("00e6a080e0b2a7a850ba71d26c9606669a4b4a6c18")));

    /* ----------------------------------------------------- */

    string M("Test User 1");

    unsigned char M_clr_data[] = {0xfa, 0x2b, 0x0c, 0xbe, 0x77};

    const Octet  M_clr_octet = Octet((unsigned char *) M_clr_data,
                                     (size_t) sizeof(M_clr_data));

    const ManagedBlob Message = Octet((unsigned char *) M.c_str(),
                                      M.length()) || M_clr_octet;

    /* ---------------------------------------------- */

    const Algorithm::Precomputations_Method_Comb<Projective::EC_Point,
                                                 ZZ,
                                                 Affine::EC_Point> Precomputation (NumBits(Curve.getModulus()));

    const StaticDataInputPolicy DefaultInputPolicy(13 - 2,
                                                   5  + 2,
                                                   13 + 5 + 10,
                                                   Hash::SHA1);

    const size_t KSize = 18; // Key size
    const SymXor Sym(KSize);

    const DSSDomainParameters<DSS_ZZ_p> DomainParameters(Curve, DefaultInputPolicy, Precomputation);
    const DSSECPVDomainParameters SchemeParameters(Hash(Hash::SHA1), Sym, KSize);

    ECPV<DSS_ZZ_p> dss(DomainParameters,
                       SchemeParameters,
                       tG);

    dss.setPrivateKey(Xa);
    dss.generatePublicKey();
    dss.buildPrecomputationTables();

    cout << "L_rec: " << DefaultInputPolicy(M.length()).L_rec
         << "; L_red: " << DefaultInputPolicy(M.length()).L_red
         << "; L_max: " << DefaultInputPolicy(M.length()).L_max << endl;

    DigitalSignature sign =
        dss.sign(Message);

    cout << "R: " << sign.R << endl;
    cout << "S: " << OS2IP(sign.S) << endl;

    VerificationVerdict v = dss.verify(sign);

    cout << "M: " << v.Message << endl;
    cout << "V: " << v.isValid << endl;

    return 0;
}
