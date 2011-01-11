#include <iostream>
#include <sstream>
#include <string>

#include <NTL/ZZ.h>
#include <NTL/ZZ_p.h>

#include "hash.hpp"
#include "ec.hpp"
#include "ec_compress.hpp"
#include "ec_defaults.hpp"
#include "utils.hpp"

#include <stdio.h>

using namespace NTL;
using namespace std;

static const ByteSeq C = I2OSP(1, 4);
static const Hash_Seq Hash(Hash::RIPEMD160);



int main(int argc     __attribute__((unused)),
         char *argv[] __attribute__((unused)))
{

    EC EC = EC_Defaults::create(EC_Defaults::EC160);

    EC_Defaults::restoreContext();

    for (unsigned int i = 0; i<20000; i++)
    {
        
    
        EC.enter_mod_context(EC::FIELD_CONTEXT);
        
        //cout << "EC:N: " << I2OSP(EC.getOrder()) << endl;

        const size_t Ln = L(EC.getOrder());
    
        //cout << "L_bits(N): " << Ln << endl;
    
        //cout << "L(N): " << L(EC.getOrder()) << endl;

        //cout << "Current modulus: " << ZZ_p::modulus() << endl;
    
        const ZZ_p Xa = ZZ_p_str("209173461442612814547828376902112692552984027342");
    
        const EC_Point Y = EC.getBasePoint() * Xa;

        //cout << "Private key: " << I2OSP(Xa) << endl;
        //cout << "Public key: " << Y << endl;

        const ZZ_p k = ZZ_p_str("49435060524598955092027863610721442198924735270");
        const EC_Point kG = EC.getBasePoint() * k;

        //cout << "Session key: " << I2OSP(k) << endl;
        //cout << "Session radical: " << kG << endl;

        const ByteSeq Pi = EC2OSP(kG,EC2OSP_COMPRESSED);

        //cout << "Π : " << Pi << endl;

        string M("This is a test message!");

        const long L_rec = 10;
        const long L_red = 9;
        const long L_clr = M.length() - L_rec;
    
        //cout << "Message: '" << M << "'" << endl;
        //cout << "[ L_rec: " << L_rec << "; L_clr: " << L_clr << "; L_red: " << L_red << endl;

        const Octet   C_rec = I2OSP(L_rec, 4);
        const Octet   C_clr = I2OSP(L_clr, 4);

        //cout << "C_rec: "  << C_rec << endl;
        //cout << "C_clr: "  << C_clr << endl;

        const ByteSeq M_rec = ByteSeq((const unsigned char *)
                                      M.substr(0, L_rec).c_str(),
                                      L_rec);
        const ByteSeq M_clr = ByteSeq((const unsigned char *)
                                      M.substr(L_rec, L_clr).c_str(),
                                      L_clr);

        //cout << "M_rec: "  << M_rec << endl;
        //cout << "M_clr: "  << M_clr << endl;

        Octet Hash_Input = C_rec || C_clr || M_rec || M_clr || Pi || C;

        //cout << "Hash Input: " << Hash_Input << endl;

        ByteSeq Hash_Token = Truncate(Hash(Hash_Input), L_red);
    
        //cout << "Hash_Token: " << Hash_Token << endl;

        ByteSeq D = Hash_Token || M_rec;

        //cout << "D: " << D << endl;

        EC.enter_mod_context(EC::ORDER_CONTEXT);
    
        const ZZ_p d = InMod(OS2IP(D));
        const ZZ_p pi = InMod(OS2IP(Pi));

        //cout << "d: " << d << endl;
        //cout << "π: " << pi << endl;
    
        const ZZ_p r = (d + pi);
        const ZZ_p s = (k - Xa*r);

        //cout << "r: " << r << endl;
        //cout << "s: " << s << endl;
    
        const ByteSeq R = I2OSP(r,Ln);
        const ByteSeq S = I2OSP(s,Ln);

        // cout << "R: " << R << endl;
        // cout << "S: " << S << endl;

        //cout << "Current modulus: " << ZZ_p::modulus() << endl;

    }
    
    EC.leave_mod_context();
    
    return 0;
}
