#include "ec/ZZ_p/affine/ec.hpp"
#include "ec/ZZ_p/affine/utils.hpp"

using namespace ECZZ_p::Affine;

/* ---------------------- Points ------------------------ */

EC_Point::EC_Point(const EC_Point & Point)
    : X(Point.X),
      Y(Point.Y),
      __EC(Point.__EC),
      isZeroPoint(Point.isZeroPoint)
{}

EC_Point::EC_Point(const EC_Point & Point, bool isZero)
    : X(isZero ? ZZ_p() : Point.X),
      Y(isZero ? ZZ_p() : Point.Y),
      __EC(Point.__EC)
{}


EC_Point::EC_Point(const ZZ_p &X, const ZZ_p &Y, const EC & __EC)
    : X(X), Y(Y), __EC(__EC), isZeroPoint(false)
{
    if (! _IsOnCurve())
        throw;
}

EC_Point::EC_Point(const EC & __EC)
    : X(ZZ_p()),
      Y(ZZ_p()),
      __EC(__EC),
      isZeroPoint(true)
{}

EC_Point::~EC_Point()
{}

// ___ must be in proper context __
bool EC_Point::_IsOnCurve() const
{
    if (isZeroPoint)
        return true;
    
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

EC_Point & EC_Point::operator= (const EC_Point & Y)
{
    // __EC field must be the same
    if (! isSameEC(__EC))
        throw; // assert

    if (Y.isZeroPoint)
        this->isZeroPoint = true;
    else
    {
        this->X = Y.getX();
        this->Y = Y.getY();
        this->isZeroPoint = false;
    }
    
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
    {
        return;
    }
    
    
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

    Y = L*(X-X3) - Y;
    X = X3;

    return;
}

void EC_Point::operator*= (const ZZ_p & Y)
{
    const ZZ & _Y = rep(Y);
    
    EC_Point S(*this);
    EC_Point R(__EC);

    if (IsZero(Y))
    {
        return;
    }

    for (long i = 0; i < NumBits(_Y); i++)
    {
        if (bit(_Y, i))
        {
            R += S;
        }

        S += S;
    }
    
    *this = R;

    return;
}

EC_Point EC_Point::operator* (const ZZ_p & Y) const
{
    EC_Point __retval(*this);

    __retval*= Y;

    return __retval;
}

/* --------------------- Curves ------------------------- */

EC::EC(const ZZ & A,
       const ZZ & B,
       const ZZ & C,
       const ZZ & N,
       const ZZ & P,
       //--- TODO Something with it .. ---
       const ZZ & Gx,
       const ZZ & Gy,
       //---------------------------------
       const ZZ & Seed)
    : __mod(P),
      __order(N),
      __is_global_setted(true),
      P(setAndUseMod(P)), N(N), N_pp(InMod(N)), 
      Seed(InMod(Seed)),
      A(InMod(A)), B(InMod(B)), C(InMod(C)),
      G(EC_Point(InMod(Gx), InMod(Gy), *this))
{}


EC::EC(const ZZ_p & A,
       const ZZ_p & B,
       const ZZ_p & C,
       const ZZ   & N,
       //--- TODO Something with it .. ---
       const ZZ_p & Gx,
       const ZZ_p & Gy,
       //---------------------------------
       const ZZ_p & Seed)
    : __mod(ZZ_p::modulus()),
      __order(N),
      __is_global_setted(false),
      P(ZZ_p::modulus()), N(N), N_pp(InMod(N)),
      Seed(Seed), A(A), B(B), C(C),
      G(EC_Point(Gx, Gy, *this))
{}

EC::~EC()
{}

EC_Point EC::create() const
{
    return EC_Point(*this);
}


EC_Point EC::create(const ZZ_p & x,
                      const ZZ_p & y) const
{
    return EC_Point(x, y, *this);
}

EC_Point EC::create(const ZZ & x,
                      const ZZ & y) const
{
    ZZ_p x_ZZp, y_ZZp;

    conv(x_ZZp, x);
    conv(y_ZZp, y);
    
    return EC_Point(x_ZZp, y_ZZp, *this);
}


bool EC::generate_random(ZZ_p & d) const
{
    random(d);

    if ((N - 2) < rep(d))
        return true;
    return false;
    
}

ZZ_p EC::generate_random() const
{
    return random_ZZ_p();
}


std::ostream& ECZZ_p::Affine::operator<<(std::ostream& s, const EC & _EC)
{
    s << "X^3 + " << _EC.getA() <<"X + " <<
        _EC.getB() << " (mod " << _EC.getModulus() << ")";

    return s;
}

std::ostream& ECZZ_p::Affine::operator<<(std::ostream& s, const EC_Point & _EC_Point)
{
    s << "( x: " << I2OSP(_EC_Point.X) << "; y: " <<
        I2OSP(_EC_Point.Y) << ")";

    return s;
}

bool EC::isCorrectOrder() const
{
    return (G * (N_pp + 1)) == G;
}
