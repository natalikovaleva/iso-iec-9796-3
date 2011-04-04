#include <iostream>
#include <sstream>
#include <string>

#include "generic/octet.hpp"
#include "generic/hash.hpp"
#include "ec/GF2X/affine/ec.hpp"
#include "ec/GF2X/affine/ec_defaults.hpp"
#include "ec/GF2X/affine/utils.hpp"

#include <stdio.h>

using namespace NTL;
using namespace std;

using namespace ECGF2X::Affine;

static const ByteSeq C = I2OSP(1, 4);
static const Hash Hash(Hash::RIPEMD160);

int main(int argc     __attribute__((unused)),
         char *argv[] __attribute__((unused)))
{
    GF2X::HexOutput = 1;
    
    EC EC = EC_Defaults::create(EC_Defaults::EC163);

    const size_t Ln = L(EC.getOrder());
    
    cout << EC << endl;

    int time1, time2 = time(NULL);

    Octet r_;
    Octet s_;

    ZZ seed;

    seed += time(NULL);
        
    SetSeed(seed);
    
    for (unsigned int i=0; i<10000; i++)
    {
        ZZ Xa; EC.generate_random(Xa);
        ZZ k;  EC.generate_random(k);
    
        const EC_Point kG = EC.getBasePoint() * k;
        const ByteSeq Pi = EC2OSP(kG,EC2OSP_COMPRESSED);
        string M("This is a test message!");
        const long L_rec = 10;
        const long L_red = 9;
        const long L_clr = M.length() - L_rec;
        const Octet   C_rec = I2OSP(L_rec, 4);
        const Octet   C_clr = I2OSP(L_clr, 4);
        const ByteSeq M_rec = ByteSeq((const unsigned char *)
                                      M.substr(0, L_rec).c_str(),
                                      L_rec);
        const ByteSeq M_clr = ByteSeq((const unsigned char *)
                                      M.substr(L_rec, L_clr).c_str(),
                                      L_clr);
        Octet Hash_Input = C_rec || C_clr || M_rec || M_clr || Pi || C;
        ByteSeq Hash_Token = Truncate(Hash(Hash_Input), L_red);
        ByteSeq D = Hash_Token || M_rec;
    
        EC.enter_mod_context(EC::ORDER_CONTEXT);
    
        const ZZ d  = OS2IP(D);
        const ZZ pi = OS2IP(Pi);
        const ZZ_p r  = InMod(d + pi);
        const ZZ_p s = (InMod(k) - InMod(Xa)*r);
        const ByteSeq R = r_ = I2OSP(r,Ln);
        const ByteSeq S = s_ = I2OSP(s,Ln);

        EC.leave_mod_context();
    }

    time1 = time(NULL);
    
    cout << "LAST R: " << r_ << endl;
    cout << "LAST S: " << s_ << endl;
    cout << "time:   " << time1 - time2 << endl;
   
    return 0;
}
