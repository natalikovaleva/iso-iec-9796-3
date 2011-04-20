#ifdef ENABLE_ISOIEC_DSS_TPL_HACK
#define _DomainParameters   ISOIEC_DSS<EC_Dscr>::_DomainParameters    
#define _Curve              ISOIEC_DSS<EC_Dscr>::_Curve  
#define _PCurve             ISOIEC_DSS<EC_Dscr>::_PCurve
#define _privateKey         ISOIEC_DSS<EC_Dscr>::_privateKey
#define _publicKey          ISOIEC_DSS<EC_Dscr>::_publicKey
#define _isPrivateKeyLoaded ISOIEC_DSS<EC_Dscr>::_isPrivateKeyLoaded
#define _isPublicKeyLoaded  ISOIEC_DSS<EC_Dscr>::_isPublicKeyLoaded
#define _Ln                 ISOIEC_DSS<EC_Dscr>::_Ln
#define _Lcm                ISOIEC_DSS<EC_Dscr>::_Lcm
#define _BasePoint          ISOIEC_DSS<EC_Dscr>::_BasePoint
#define _PRNG               ISOIEC_DSS<EC_Dscr>::_PRNG
#else
#undef  _DomainParameters    
#undef  _Curve
#undef  _PCurve
#undef  _privateKey
#undef  _publicKey
#undef  _isPrivateKeyLoaded
#undef  _isPublicKeyLoaded
#undef  _Ln
#undef  _Lcm
#undef  _BasePoint
#undef  _PRNG
#endif
