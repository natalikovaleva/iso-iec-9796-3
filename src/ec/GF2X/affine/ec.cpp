#include "ec/GF2X/affine/ec.hpp"
#include "ec/GF2X/affine/utils.hpp"

#include <exception>

using namespace ECGF2X::Affine;

/* ---------------------- Points ------------------------ */

EC_Point::EC_Point(const EC_Point & Point)
    : X(Point.X),
      Y(Point.Y),
      __EC(Point.__EC),
      isZeroPoint(Point.isZeroPoint),
      __precomputations(Point.__precomputations),
      __generic_multiplication()
{}

EC_Point::EC_Point(const GF2X &X, const GF2X &Y, const EC & __EC)
    : X(X), Y(Y), __EC(__EC), isZeroPoint(false),
      __precomputations(),
      __generic_multiplication()
{
    if (! _IsOnCurve())
        throw std::exception();
}

EC_Point::EC_Point(const EC & __EC)
    : X(GF2X()),
      Y(GF2X()),
      __EC(__EC),
      isZeroPoint(true),
      __precomputations(),
      __generic_multiplication()
{}

EC_Point::~EC_Point()
{}

// ___ must be in proper context __
bool EC_Point::_IsOnCurve() const
{
    if (isZeroPoint)
        return true;

    // y^2 + xy = x^3 + ax^2 + b

    const GF2X Y2 = SqrMod(Y, __EC.P);
    const GF2X X2 = SqrMod(X, __EC.P);
    const GF2X X3 = PowerMod(X, 3, __EC.P);


    const GF2X L_Part = Y2 + MulMod(X, Y, __EC.P);
    const GF2X R_Part = X3 + MulMod(__EC.A, X2, __EC.P) + __EC.B;

    return ( L_Part == R_Part );
}

bool EC_Point::isSameEC(const EC & __EC) const
{
    return &(this->__EC) == &(__EC);
}

EC_Point & EC_Point::operator= (const EC_Point & Y)
{
    // __EC field must be the same
    if (! isSameEC(__EC))
        throw std::exception(); // assert

    if (Y.isZeroPoint)
    {
        this->isZeroPoint = true;
        this->__precomputations.drop();
    }
    else
    {
        this->X = Y.getX();
        this->Y = Y.getY();
        this->isZeroPoint = false;
        this->__precomputations = Y.__precomputations;
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
    const GF2X & YX = _Y.getX();
    const GF2X & YY = _Y.getY();

    if (_Y.isZero())
    {
        return;
    }

    if (isZero())
    {
        *this = _Y;
        return;
    }

    if ((this == &_Y) || (Y==YY))
    {
        const GF2X r = X + MulMod(Y, InvMod(X, __EC.P_mod),
                                               __EC.P_mod);
        const GF2X r2 = SqrMod(r, __EC.P_mod);
        const GF2X x2 = SqrMod(X, __EC.P_mod);

        const GF2X X3 = r2 + r +__EC.A;
        const GF2X Y3 = x2 + MulMod( r + 1, X3, __EC.P_mod);

        X = X3;
        Y = Y3;
    }
    else
    {
        const GF2X r = MulMod(Y + YY, InvMod( X + YX, __EC.P_mod),
                                                      __EC.P_mod);
        const GF2X r2 = SqrMod(r, __EC.P_mod);

        const GF2X X3 = r2 + r + X + YX + __EC.A;
        const GF2X Y3 = MulMod( X + X3, r, __EC.P_mod) + X3 + Y;

        X = X3;
        Y = Y3;
    }

    __precomputations.drop();

    return;
}

void EC_Point::operator*= (const ZZ & Y)
{
    if (IsZero(Y))
    {
        isZeroPoint = true;
        return;
    }

    if (isPrecomputed())
    {
        __precomputations.Multiply(*this, Y);
    }
    else
    {
        __generic_multiplication.Multiply(*this, Y);
    }

    __precomputations.drop();

    return;
}

void EC_Point::operator*= (const long Y)
{
    if (Y == 0)
    {
        isZeroPoint = true;
        __precomputations.drop();
        return;
    }
    else
        operator*=(ZZ() + Y);
}


EC_Point EC_Point::operator* (const ZZ & Y) const
{
    EC_Point __retval(*this);

    __retval*= Y;

    return __retval;
}

EC_Point EC_Point::operator* (const long Y) const
{
    EC_Point __retval(*this);

    __retval*= Y;

    return __retval;
}



/* --------------------- Curves ------------------------- */

EC::EC(const GF2X & A,
       const GF2X & B,
       const GF2X & C,
       const ZZ   & N,
       const GF2X & P,
       //--- TODO Something with it .. ---
       const GF2X & Gx,
       const GF2X & Gy,
       //---------------------------------
       const GF2X & Seed)
    : __order(N), __global(), __is_global_setted(false),
      N(N), P(P),
      Seed(Seed), A(A), B(B), C(C),
      P_mod(P), P_deg(deg(P)),
      G(EC_Point(Gx, Gy, *this))
{}

EC::~EC()
{}

EC_Point EC::create() const
{
    return EC_Point(*this);
}

EC_Point EC::create(const GF2X & x,
                    const GF2X & y) const
{
    return EC_Point(x, y, *this);
}


bool EC::generate_random(GF2X & d) const
{
    random(d, P_deg);

    /* FIXME: Check, that d < N-1 */
    return true;

}

bool EC::generate_random(ZZ & d) const
{
    RandomBnd(d, N);

    if (d < (N - 1))
        return true;

    return false;
}


GF2X EC::generate_random() const
{
    return random_GF2X(NumBits(N));
}


std::ostream& ECGF2X::Affine::operator<<(std::ostream& s, const EC & _EC)
{
    s << "X^3 + " << _EC.getA() <<"X + " <<
        _EC.getB() << " (mod " << _EC.getModulus() << ")";

    return s;
}

std::ostream& ECGF2X::Affine::operator<<(std::ostream& s, const EC_Point & _EC_Point)
{
    s << "( x: " << FE2OSP(_EC_Point.X) << "; y: " <<
        FE2OSP(_EC_Point.Y) << ")";

    return s;
}

bool EC::isCorrectOrder() const
{
    return (G * (N + 1)) == G;
}
