#include "dstu4145.hpp"

#pragma GCC visibility push(hidden)
#include "dss/dss_dstu4145.hpp"

#include "algorithm/comb.hpp"

using namespace NTL;
using namespace std;
using namespace ECZZ_p;
using namespace ECZZ_p::Affine;

struct DSTUContext
{
    Hash                            * _Hash;
    ECDSSDomainParameters<DSS_ZZ_p> * _Parameters;
    DSTU4145<DSS_ZZ_p>              * _DSTU4145;
};

class rcallPRNG : public generateRandomValueCallback
{
    unsigned char buffer[2048];
    const int _Ln;
    fPRNG _fprng;
        
public:
    rcallPRNG(int Ln, fPRNG fprng)
        : _Ln(Ln),
          _fprng(fprng)
        {}
    Octet getRandomValue()
        {
            _fprng(buffer, _Ln > sizeof(buffer) ?
                   sizeof(buffer) : _Ln);
            
            return Octet(buffer, _Ln);
        }
};
#pragma GCC visibility pop


void * dstu4145_createEC_ZZ_p(const char * P,
                              const char * A,
                              const char * B,
                              const char * GX,
                              const char * GY,
                              const char * N)
{
    const ZZ _P  = ZZ_str(P);
    const ZZ _N  = ZZ_str(N);
    const ZZ _A  = ZZ_str(A);
    const ZZ _B  = ZZ_str(B);
    const ZZ _GX = ZZ_str(GX);
    const ZZ _GY = ZZ_str(GY);
    const ZZ _SEED, _C;
    
    return new EC(_A, _B, _C,
                  _N, _P, _GX, _GY,
                  _SEED);
}

void dstu4145_freeEC_ZZ_p(void * _vEC)
{
    EC * vEC = (EC *) _vEC;
    delete vEC;
}



void * dstu4145_create_context(void * EC_ZZ_p, DSTU4145_HASHES id, fPRNG callback)
{
    Hash::Hash_Type hash = Hash::SHA512;

    EC * Curve = (EC *) EC_ZZ_p;
        
    switch(id)
    {
        case SHA512: hash = Hash::SHA512; break;
        case SHA256: hash = Hash::SHA256; break;
        case SHA1:   hash = Hash::SHA1; break;
        default:     hash = Hash::SHA512; break;
    }
    
    const Algorithm::Precomputations_Method_Comb
        <Projective::EC_Point, ZZ, Affine::EC_Point>
        Precomputation (NumBits(Curve->getModulus()));

    
    DSTUContext * newContext = new DSTUContext;
    rcallPRNG rG(L(Curve->getOrder()), callback);    

    newContext->_Hash =
        new Hash(hash);
    newContext->_Parameters =
        new ECDSSDomainParameters<DSS_ZZ_p>(*Curve, Precomputation);
    newContext->_DSTU4145 =
        new DSTU4145<DSS_ZZ_p>(*newContext->_Parameters, *newContext->_Hash, rG);
    
    return newContext;
}

void dstu4145_free_context(void * context)
{
    DSTUContext * _context = (DSTUContext *) context;
    delete _context->_DSTU4145;
    delete _context->_Parameters;
    delete _context->_Hash;
    delete _context;
}

struct SIGN * dstu4145_create_sign(const char * message, int message_size, void * context)
{
    DSTUContext * _context = (DSTUContext *) context;

    DigitalSignature sign =
        _context->_DSTU4145->sign(ByteSeq(message, message_size));

    struct SIGN * _rv = (struct SIGN *) malloc(sizeof(struct SIGN));

    if (_rv == NULL)
        return NULL;

    _rv->R = (char *) malloc(sign.R.getDataSize());

    if (_rv->R == NULL)
    {
        free(_rv);
        return NULL;
    }

    _rv->R_size = sign.R.getDataSize();
    
    memcpy(_rv->R, sign.R.getData(), _rv->R_size);

    _rv->S =  (char *) malloc(sign.S.getDataSize());

    if (_rv->S == NULL)
    {
        free(_rv->R);
        free(_rv);
        return NULL;
    }

    _rv->S_size = sign.S.getDataSize();
    
    memcpy(_rv->S, sign.S.getData(), _rv->S_size);
    
    _rv->M =  (char *) malloc(sign.M_clr.getDataSize());

    if (_rv->M == NULL)
    {
        free(_rv->R);
        free(_rv->S);
        free(_rv);
        return NULL;
    }

    _rv->M_size = sign.M_clr.getDataSize();
    
    memcpy(_rv->M, sign.M_clr.getData(), _rv->M_size);

    return _rv;
}

const char * dstu4145_verify_sign(const struct SIGN * sign, void * context)
{
    DSTUContext * _context = (DSTUContext *) context;

    DigitalSignature _sign(ByteSeq(sign->R, sign->R_size),
                           ByteSeq(sign->S, sign->S_size),
                           ByteSeq(sign->M, sign->M_size));
    
    return (_context->_DSTU4145->verify(_sign)).isValid ?
        sign->M : NULL;
}

void dstu4145_free_sign(struct SIGN * sign)
{
    free(sign->M);
    free(sign->S);
    free(sign->R);
}
