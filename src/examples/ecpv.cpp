#include <iostream>
#include <sstream>
#include <string>

#include "generic/octet.hpp"
#include "generic/hash.hpp"
#include "generic/mgf.hpp"
#include "generic/kdf.hpp"
#include "generic/sym.hpp"

#include "ec/ZZ_p/affine/ec.hpp"
#include "ec/ZZ_p/affine/ec_compress.hpp"
#include "ec/ZZ_p/affine/ec_defaults.hpp"
#include "ec/ZZ_p/affine/utils.hpp"

#include "ec/ZZ_p/projective/ec.hpp"

using namespace NTL;
using namespace std;
using namespace ECZZ_p;
using namespace ECZZ_p::Affine;

#include "algorithm/comb.hpp"

#include <stdio.h>

class ECPVKDF : public KDF
{
    const size_t L_key;
    const MGF MGF2;
public:
    ECPVKDF(size_t L_key)
        : L_key(L_key),
          MGF2(MGF::MGF2, Hash::SHA1) {};

    inline ByteSeq operator() (const ByteSeq & data) const
        {
            return MGF2(data, L_key);
        }
};

const EC_Defaults::Sizes ECSize = EC_Defaults::EC161;

static const SymXor  Sym(18);
static const ECPVKDF KDF(18);
static const Hash    Hash(Hash::SHA1);
static const MGF     MGF1(MGF::MGF1, Hash::SHA1);

int main(int argc     __attribute__((unused)),
         char *argv[] __attribute__((unused)))
{
    EC EC = EC_Defaults::create(ECSize);

    cout << EC << endl;

    EC.enter_mod_context(EC::FIELD_CONTEXT);

    const size_t Ln = L(EC.getOrder());

    cout << "Current modulus: " << ZZ_p::modulus() << endl;

    const ZZ Xa = ZZ_str("00e6a080e0b2a7a850ba71d26c9606669a4b4a6c18");
    const ZZ k  = ZZ_str("00d8a0abc5b7a4029ac232cbcda16819e1b715f9f4");

    const EC_Point Y  = EC.getBasePoint() * Xa;
    const EC_Point PP = EC.getBasePoint() * k;

    cout << "Ln:          " << Ln << endl;
    cout << "Order:       " << I2OSP(EC.getOrder()) << endl;
    cout << "Generator:   " << EC.getBasePoint() << endl;
    cout << "Private key: " << I2OSP(Xa) << endl;
    cout << "Public key:  " << Y << endl;
    cout << "Session key: " << I2OSP(k) << endl;
    cout << "Randomizer:  " << PP << endl;

    const Octet SS = I2OSP(PP.getX());
    const Octet P  = KDF(SS);

    cout << "Π : " << P << endl;

    EC.enter_mod_context(EC::ORDER_CONTEXT);

    string M("Test User 1");

    const Octet DERPrintableString = I2OSP(0x13);
    const Octet DERSize = I2OSP(M.length());

    const Octet M_rec =
        DERPrintableString ||
        DERSize ||
        ByteSeq((const unsigned char *)
                M.c_str(),
                M.length());

    const size_t L_red = 5;

    unsigned char M_clr_data[] = {0xfa, 0x2b, 0x0c, 0xbe, 0x77};

    const Octet  M_clr = Octet((unsigned char *) M_clr_data,
                               (size_t) sizeof(M_clr_data));

    cout << "M_rec: "  << M_rec << endl;
    cout << "M_clr: "  << M_clr << endl;

    const Octet C_red_ = I2OSP(L_red);
    Octet C_red;

    for (unsigned int i = 0; i<L_red; i++)
        C_red = C_red || C_red_ ;

    const Octet d = C_red || M_rec;

    cout << "d: " << d << endl;

    const Octet r = d ^ P;
    const Octet u = Hash(r || M_clr);
    const ZZ_p  t = InMod(OS2IP(u));
    const ZZ_p  s = (InMod(k) - InMod(Xa)*t);

    cout << "u: " << u << endl;
    cout << "t: " << I2OSP(t) << endl;

    const Octet S = I2OSP(s);

    cout << "R: " << r << endl;
    cout << "S: " << s << endl;

    return 0;
}
