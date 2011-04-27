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

struct VerificationVerdict
{
    const Octet Message;
    const bool  isValid;

public:

    VerificationVerdict(const Octet & Message)
        : Message(Message),
          isValid(true)
        {}

    VerificationVerdict()
        : Message(Octet()),
          isValid(false)
        {}
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

class fixedGenerator : public generateRandomValueCallback
{
    const Octet _FixedValue;
public:
    fixedGenerator(const Octet & FixedValue)
        : _FixedValue(FixedValue)
        {}
    Octet getRandomValue()
        { return _FixedValue; }
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

    virtual bool setPrivateKey(const Octet & PrivateKey) = 0;
    virtual bool setPublicKey(const Octet & PublicKey) = 0;

    virtual Octet generatePublicKey() = 0;

    virtual void buildPrecomputationTables() = 0;
};

class DigitalSignatureWithAddition
{
protected:
    DigitalSignatureWithAddition() {}

public:
    virtual ~DigitalSignatureWithAddition() {}

    virtual DigitalSignature sign(const ByteSeq & data) = 0;
    virtual VerificationVerdict verify(const DigitalSignature & data) = 0;

    virtual bool setPrivateKey(const Octet & PrivateKey) = 0;
    virtual bool setPublicKey(const Octet & PublicKey) = 0;

    virtual Octet generatePublicKey() = 0;

    virtual void buildPrecomputationTables() = 0;
};
