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

        int R_size;
        int S_size;
        int M_size;
    };

    void * dstu4145_createEC_ZZ_p(const char * P,
                                  const char * A,
                                  const char * B,
                                  const char * GX,
                                  const char * GY,
                                  const char * N);

    void dstu4145_freeEC_ZZ_p(void * _vEC);
    void * dstu4145_create_context(void * EC_ZZ_p, DSTU4145_HASHES id, fPRNG callback);
    void dstu4145_free_context(void * context);
    struct SIGN * dstu4145_create_sign(const char * message, int message_size, void * context);
    const char * dstu4145_verify_sign(const struct SIGN * sign, void * context);
    void dstu4145_free_sign(struct SIGN * sign);




#ifdef __cplusplus
}
#endif
