#include "ec.hpp"

/* ---------------------- Points ------------------------ */

EC_Point::EC_Point(const ZZ_p &X, const ZZ_p &Y, const EC & __EC)
    : X(X), Y(Y), __EC(__EC)
{
    if (! _IsOnCurve())
        throw;
}

EC_Point::EC_Point(const EC & __EC)
    : X(ZZ_p()), Y(ZZ_p()), __EC(__EC)
{}

EC_Point::~EC_Point()
{}

// ___ must be in proper context __
bool EC_Point::_IsOnCurve() const
{
    ZZ_p Y2, X3;
    bool __result = false;

    sqr(Y2, Y);
    power(X3, X, 3);

    if (__EC.A *X  +  X3 + __EC.B == Y2 )
        __result = true;

    return __result;
}

bool EC_Point::isSameEC(const EC & __EC) const
{
    return &(this->__EC) == &(__EC);
}


const ZZ_p & EC_Point::getX() const
{
    return X;
}

const ZZ_p & EC_Point::getY() const
{
    return Y;
}



EC_Point & EC_Point::operator= (const EC_Point & Y)
{
    // __EC field must be the same
    if (! isSameEC(__EC))
        throw; // assert
    
    this->X = Y.getX();
    this->Y = Y.getY();
    
    return *this;
}

EC_Point EC_Point::operator+  (const EC_Point & _Y) const
{
    EC_Point __retval(*this);

    __retval+= _Y;
    
    return __retval;
}



void EC_Point::operator+= (const EC_Point & _Y)
{
    const ZZ_p & YX = _Y.getX();
    const ZZ_p & YY = _Y.getY();

    if (_Y.isZero())
        return;
    
    if (isZero())
    {
        *this = _Y;
        return;
    }

    
    ZZ_p L;
    
    
    if ((this == &_Y) ||
        (X==YX) || (Y==YY))
    {
        L = ( 3 * sqr(X) + __EC.A ) / (2 * Y);
    }
    else
    {
        L = (Y - YY) / (X - YX);
    }
    
    ZZ_p X3 = sqr(L) - X - YX;
    ZZ_p Y3 = L*(X-X3) - Y;

    X = X3;
    Y = Y3;


    return;
}

void EC_Point::operator*= (const ZZ_p & Y)
{
    const ZZ & _Y = rep(Y);
    
    EC_Point tmp(*this);

    if (IsZero(Y))
        this->Y = Y;
    
    
    for (long i=0; i < NumBits(_Y); i++)
    {
        if (bit(_Y, i))
        {
            *this += tmp;
        }

        tmp += tmp;
    }

    return;
}

EC_Point EC_Point::operator* (const ZZ_p & Y) const
{
    EC_Point __retval(*this);

    __retval*= Y;

    return __retval;
}



bool EC_Point::isZero() const
{
    return IsZero(Y);
}


/* --------------------- Curves ------------------------- */

EC::EC(const ZZ_p & A,
       const ZZ_p & B,
       const ZZ_p & C,
       const ZZ_p & P,
       //--- TODO Something with it .. ---
       const ZZ_p & Gx,
       const ZZ_p & Gy,
       //---------------------------------
       const ZZ_p & Seed)
    : N(N), P(ZZ_p::modulus()), Seed(Seed), A(A), B(B), C(C),
      G(EC_Point(Gx, Gy, *this)),
      __mod(ZZ_p::modulus())
{}

EC::~EC()
{}


EC_Point EC::create(const ZZ_p & x,
                      const ZZ_p & y) const
{
    return EC_Point(x, y, *this);
}

bool EC::generate_random(ZZ_p & d) const
{
    random(d);

    if ((rep(N) - 2) < rep(d))
        return true;
    return false;
    
}

ZZ_p EC::generate_random() const
{
    return random_ZZ_p();
}


std::ostream& operator<<(std::ostream& s, const EC & _EC)
{
    s << "X^3 + " << _EC.A <<"X + " <<
        _EC.B << " (mod " << _EC.P << ")";

    return s;
}

std::ostream& operator<<(std::ostream& s, const EC_Point & _EC_Point)
{
    s << "(" << _EC_Point.X << ";" <<
        _EC_Point.Y << ")";

    return s;
}

#include <iostream>

bool EC::isCorrectOrder() const
{
    std::cout << (G * N) << std::endl;
    
    return (G * N).isZero();
}
