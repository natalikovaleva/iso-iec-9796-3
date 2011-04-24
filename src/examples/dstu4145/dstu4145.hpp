#pragma once

#ifdef __cplusplus
extern "C"
{
#endif
    typedef void (* fPRNG)(unsigned char * buffer, int count);

    enum DSTU4145_HASHES
    {
        SHA512,
        SHA256,
        SHA1
    };

    struct SIGN
    {
        char * R;
        char * S;

        char * M;
        int M_size;
    };

    void * dstu4145_createEC_ZZ_p(const char * P,
                                  const char * A,
                                  const char * B,
                                  const char * GX,
                                  const char * GY,
                                  const char * N);

    void dstu4145_freeEC_ZZ_p(void * _vEC);
    void * dstu4145_create_context(void * EC_ZZ_p, enum DSTU4145_HASHES id, fPRNG callback);
    void dstu4145_free_context(void * context);
    struct SIGN * dstu4145_create_sign(const char * message, int message_size, void * context);
    const char * dstu4145_verify_sign(const struct SIGN * sign, void * context);

    int dstu4145_set_private_key(const char * key, void * context);
    int dstu4145_set_public_key(const char * key, void * context);
    char * dstu4145_create_public_key(char ** output, void * context);
    int dstu4145_make_precoputations(void * context);

    void dstu4145_free_sign(struct SIGN * sign);




#ifdef __cplusplus
}
#endif
