#pragma once

#include "generic/octet.hpp"
#include "dss/datain.hpp"

struct DigitalSignature
{
public:

    const Octet R;
    const Octet S;
    const Octet M_clr;


    DigitalSignature(const Octet & R,
                     const Octet & S,
                     const Octet & M_clr)
        : R(R),
          S(S),
          M_clr(M_clr)
        {}
};

class VerificationVerdict
{
    const Octet _Message;
    const bool  _isValid;

public:
    VerificationVerdict(const Octet & Message)
        : _Message(Message),
          _isValid(true)
        {}

    VerificationVerdict()
        : _Message(Octet()),
          _isValid(false)
        {}

    inline bool isValid()
        { return _isValid; }
    inline Octet getMessage()
        { return _Message; }
};

class generateRandomValueCallback
{
protected:
    generateRandomValueCallback() {}

public:
    virtual ~generateRandomValueCallback() {}
    virtual Octet getRandomValue() = 0;

    inline Octet operator() ()
        { return getRandomValue(); }
};

class DigitalSignatureWithRecovery
{
protected:
    DigitalSignatureWithRecovery() {}

public:
    virtual ~DigitalSignatureWithRecovery() {}

    virtual DigitalSignature sign(const ByteSeq & data,
                                  const DataInputPolicy * dip = NULL) = 0;
    virtual VerificationVerdict verify(const DigitalSignature & data,
                                       const DataInputPolicy * dip = NULL) = 0;

    virtual void setPrivateKey(const Octet & PrivateKey) = 0;
    virtual void setPublicKey(const Octet & PublicKey) = 0;

    virtual Octet generatePublicKey() = 0;

    virtual void buildPrecomputationTables() = 0;
};

