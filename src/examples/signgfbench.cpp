#include <iostream>
#include <sstream>
#include <string>

#include "generic/octet.hpp"
#include "generic/hash.hpp"
#include "generic/mgf.hpp"
#include "affine/GF2X/ec.hpp"
#include "affine/GF2X/ec_defaults.hpp"
#include "affine/GF2X/utils.hpp"

#include <stdio.h>

using namespace NTL;
using namespace std;

using namespace Affine::GF2X;

static const Hash Hash(Hash::SHA1);

int main(int argc     __attribute__((unused)),
         char *argv[] __attribute__((unused)))
{
    GF2X::HexOutput = 1;

    EC EC = EC_Defaults::create(EC_Defaults::EC163);
    const size_t Ln = L(EC.getOrder());

    int time1, time2 = time(NULL);

    Octet r_;
    Octet s_;

    ZZ seed;

    seed += time(NULL);
        
    SetSeed(seed);
    
    for (unsigned int i=0; i<30000; i++)
    {
        ZZ Xa;
        ZZ k;

        EC.generate_random(Xa);
        EC.generate_random(k);
    
        const EC_Point R = EC.getBasePoint() * k;
        const ByteSeq OPoint = EC2OSP(R, EC2OSP_UNCOMPRESSED);
        const MGF MGF1(MGF::MGF1, Hash::SHA1);
        const ByteSeq Pi = MGF1(OPoint, Ln);
        const string Message("TestVector");
        const long L_rec = Message.length();
        const long L_red = Ln - L_rec;
        const ByteSeq M(Message.c_str(), Message.length());
        const ByteSeq & M_rec = M;
        const Octet h = Truncate(Hash(Pi || M), L_red);
        const Octet d = h || M_rec;
        const Octet r = r_ = d ^ Pi;
        EC.enter_mod_context();

        const ZZ_p s = InMod(OS2IP(r) * k - OS2IP(r) - 1) / InMod(Xa + 1);

        EC.leave_mod_context();

        const Octet S = s_ = I2OSP(s, Ln);
    }

    time1 = time(NULL);
    
    cout << "LAST R: " << r_ << endl;
    cout << "LAST S: " << s_ << endl;
    cout << "time:   " << time1 - time2 << endl;
   

    return 0;
}
