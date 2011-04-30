#include "dstu4145.hpp"

#pragma GCC visibility push(hidden)
#include "dss/dss_dstu4145.hpp"

#include "algorithm/comb.hpp"

#include <sstream>

using namespace NTL;
using namespace std;
using namespace ECZZ_p;
using namespace ECZZ_p::Affine;

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

struct DSTUContext
{
    Hash                            * _Hash;
    ECDSSDomainParameters<DSS_ZZ_p> * _Parameters;
    rcallPRNG                       * _PRNG;
    Algorithm::Precomputations_Method_Comb
    <Projective::EC_Point, ZZ, Affine::EC_Point>
                                    * _Precomputations;
    DSTU4145<DSS_ZZ_p>              * _DSTU4145;
};

static inline const char * filter_chr(char * output, const char * input)
{
    int i, j, len = strlen(input);

    for (i=0, j=0; i<len; i ++)
    {
        if (((input[i] >= '0') && (input[i] <= '9')) ||
            ((input[i] >= 'a') && (input[i] <= 'f')) ||
            ((input[i] >= 'A') && (input[i] <= 'F')))
            output[j ++] = input[i];
    }

    output[j] = 0;
    return output;
}


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

    EC * ec = NULL;

    try
    {
        ec = new EC(_A, _B, _C,
                    _N, _P, _GX, _GY,
                    _SEED);
    }
    catch(...)
    {
        return NULL;
    }

    return ec;

}

void dstu4145_freeEC_ZZ_p(void * _vEC)
{
    if (_vEC == NULL) return;

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

    DSTUContext * newContext = NULL;

    try
    {
        newContext = new DSTUContext;

        newContext->_Hash =
            new Hash(hash);
        newContext->_Precomputations =
            new Algorithm::Precomputations_Method_Comb
            <Projective::EC_Point, ZZ, Affine::EC_Point>
            (NumBits(Curve->getModulus()));
        newContext->_Parameters =
            new ECDSSDomainParameters<DSS_ZZ_p>(*Curve, *newContext->_Precomputations);
        newContext->_PRNG =
            new rcallPRNG(L(Curve->getOrder()), callback);
        newContext->_DSTU4145 =
            new DSTU4145<DSS_ZZ_p>(*newContext->_Parameters,
                                   *newContext->_Hash,
                                   *newContext->_PRNG);
    }
    catch(...)
    {
        return NULL;
    }


    return newContext;
}

void dstu4145_free_context(void * context)
{
    if (context == NULL) return;

    DSTUContext * _context = (DSTUContext *) context;
    delete _context->_DSTU4145;
    delete _context->_Parameters;
    delete _context->_Precomputations;
    delete _context->_Hash;
    delete _context->_PRNG;
    delete _context;
}

int dstu4145_set_private_key(const char * key, void * context)
{
    DSTUContext * _context = (DSTUContext *) context;

    int rval;

    char * buf = (char * ) alloca(strlen(key) + 1);

    try
    {
        const ZZ _Xa = ZZ_str(filter_chr(buf, key));
        rval = _context->_DSTU4145->setPrivateKey(Octet(I2OSP(_Xa))) == true? 0 : 1;
    }
    catch(...)
    {
        return 1;
    }

    return rval;
}

int dstu4145_set_public_key(const char * key, void * context)
{
    DSTUContext * _context = (DSTUContext *) context;

    int rval;

    char * buf = (char * ) alloca(strlen(key) + 1);
    filter_chr(buf, key);

    try
    {
        const Octet _Q = I2OSP(ZZ_str(buf), strlen(buf)/2);
        rval = _context->_DSTU4145->setPublicKey(_Q) == true ? 0 : 1;
    }
    catch(...)
    {
        return 1;
    }

    return rval;
}

char * dstu4145_create_public_key(char ** optr, void * context)
{
    DSTUContext * _context = (DSTUContext *) context;
    Octet _Q;

    try
    {
        _Q =  _context->_DSTU4145->generatePublicKey();
    }
    catch(...)
    {
        return NULL;
    }

    ostringstream output;

    output << _Q;

    char * buf = (char * ) alloca(strlen(output.str().c_str()) + 1);

    return optr == NULL ? NULL :
        ((*optr) = strdup(filter_chr(buf, output.str().c_str())));
}

int dstu4145_make_precoputations(void * context)
{
    DSTUContext * _context = (DSTUContext *) context;

    try
    {
        _context->_DSTU4145->buildPrecomputationTables();
    }
    catch(...)
    {
        return 1;
    }

    return 0;
}

struct SIGN * dstu4145_create_sign(const char * message, int message_size, void * context)
{
    DSTUContext * _context = (DSTUContext *) context;

    struct SIGN * _rv = (struct SIGN *) malloc(sizeof(struct SIGN));

    if ( _rv == NULL)
        return NULL;

    _rv->M = (char *) malloc(message_size);

    if ( _rv->M == NULL)
    {
        free(_rv);
        return NULL;
    }

    try
    {
        const ManagedBlob msg(message, message_size);
        DigitalSignature sign = _context->_DSTU4145->sign(msg);

        char * buf;
        ostringstream output;

        output << sign.R;
        buf = (char * ) alloca(strlen(output.str().c_str()) + 1);
        _rv->R = strdup(filter_chr(buf, output.str().c_str()));
        output.str(string(""));

        output << sign.S;
        buf = (char * ) alloca(strlen(output.str().c_str()) + 1);
        _rv->S = strdup(filter_chr(buf, output.str().c_str()));
        output.str(string(""));

    }
    catch(...)
    {
        free(_rv->M);
        free(_rv);
        return NULL;
    }

    memcpy(_rv->M, message, message_size);

    _rv->M_size = message_size;

    return _rv;
}

const char * dstu4145_verify_sign(const struct SIGN * sign, void * context)
{
    DSTUContext * _context = (DSTUContext *) context;

    int sR = strlen(sign->R);
    int sS = strlen(sign->S);

    char * bR = (char *) alloca(sR+1);
    char * bS = (char *) alloca(sS+1);

    bool rval;

    try
    {
        DigitalSignature _sign(I2OSP(ZZ_str(filter_chr(bR,sign->R))),
                               I2OSP(ZZ_str(filter_chr(bS,sign->S))),
                               ManagedBlob(sign->M, sign->M_size));

        rval =  (_context->_DSTU4145->verify(_sign)).isValid;
    }
    catch(...)
    {
        return NULL;
    }

    return rval ? sign->M : NULL;
}

void dstu4145_free_sign(struct SIGN * sign)
{
    if (sign == NULL) return;

    free(sign->M);
    free(sign->S);
    free(sign->R);
    free(sign);
}
