#include "ec/ZZ_p/projective/ec.hpp"

#include "ec/ZZ_p/affine/ec.hpp"
#include "ec/ZZ_p/affine/utils.hpp"

#include <exception>

namespace ECZZ_p
{
    namespace Projective
    {
        inline void Jacobian_Double(ZZ_p & X, ZZ_p & Y, ZZ_p & Z, const EC & __EC)
        {
            const ZZ_p A = sqr(Y);
            const ZZ_p B = 4 * X * A;
            const ZZ_p C = 8 * sqr(A);
            const ZZ_p D = 3 * sqr(X) + __EC.getA() * power(Z, 4);
            const ZZ_p Y1 = Y;

            X = sqr(D) - 2*B;
            Y = D * (B - X) - C;
            Z *= (2 * Y1);
        }

        inline void Jacobian_Addition(EC_Point & _X, const Affine::EC_Point & _Y)
        {
            const ZZ_p A = sqr(_X.getZ());
            const ZZ_p B = _X.getZ() * A;
            const ZZ_p C = _Y.getX() * A;
            const ZZ_p D = _Y.getY() * B;
            const ZZ_p E = C - _X.getX();
            const ZZ_p F = D - _X.getY();
            const ZZ_p G = sqr(E);
            const ZZ_p H = G * E;
            const ZZ_p I = _X.getX() * G;

            const ZZ_p Y1 = _X.getY();

            _X.X = sqr(F) - (H + 2*I);
            _X.Y = F * (I - _X.X) - (Y1 * H);
            _X.Z *= E;
        }

        inline void Jacobian_Addition(EC_Point & _X, const EC_Point & _Y)
        {
            const ZZ_p A = _X.getX() * sqr(_Y.getZ());
            const ZZ_p B = _Y.getX() * sqr(_X.getZ());
            const ZZ_p C = _X.getY() * power(_Y.getZ(), 3);
            const ZZ_p D = _Y.getY() * power(_X.getZ(), 3);
            const ZZ_p E = B - A;
            const ZZ_p F = D - C;

            const ZZ_p E2 = sqr(E);
            const ZZ_p E3 = power(E, 3);

            _X.X = - E3 - 2*A*E2 + sqr(F);
            _X.Y = - C*E3 + F*(A*E2 - _X.X);
            _X.Z *= _Y.getZ() * E;
        }
    }

    Projective::EC_Point
    toProjective(const Affine::EC_Point & Point,
                 const Projective::EC & EC)
    {
        if (Point.isZero())
            return EC.create();
        else
            return EC.create(Point.getX(), Point.getY(), ZZ_p() + 1);
    }

    Affine::EC_Point
    toAffine(const Projective::EC_Point & Point)
    {
        if (Point.isZero())
            return Point.getEC().getAffineBasePoint()
            .getEC().create();
        else
            return Point.getEC().getAffineBasePoint()
                .getEC().create(Point.getX() / sqr(Point.getZ()),
                                Point.getY() / power(Point.getZ(), 3));
    }
}

using namespace ECZZ_p;
using namespace ECZZ_p::Projective;

EC_Point::EC_Point(const EC_Point & Point)
    : X(Point.X),
      Y(Point.Y),
      Z(Point.Z),
      __EC(Point.__EC),
      __isZeroPoint(Point.__isZeroPoint),
      __precomputations(Point.__precomputations),
      __generic_multiplication()
{}

EC_Point::EC_Point(const ZZ_p &X,
                   const ZZ_p &Y,
                   const ZZ_p &Z, const EC & __EC)
    : X(X), Y(Y), Z(Z), __EC(__EC),
      __isZeroPoint(false),
      __precomputations(),
      __generic_multiplication()
{
    if (! _IsOnCurve())
    {
        throw std::exception();
    }
}

EC_Point::EC_Point(const EC & __EC)
    : X(ZZ_p()),
      Y(ZZ_p()),
      Z(ZZ_p()),
      __EC(__EC),
      __isZeroPoint(true),
      __precomputations(),
      __generic_multiplication()
{}

EC_Point::~EC_Point()
{}

// ___ must be in proper context __
bool EC_Point::_IsOnCurve() const
{
    if (__isZeroPoint)
        return true;

    const ZZ_p L_Part = sqr(Y);
    const ZZ_p Z2 = sqr(Z);
    const ZZ_p Z4 = sqr(Z2);
    ZZ_p R_Part = power(X, 3);

    R_Part += __EC.getA() * X * Z4;
    R_Part += __EC.getB() * Z2 * Z4;

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

    if (Y.__isZeroPoint)
    {
        this->__isZeroPoint = true;
        this->__precomputations.drop();
    }

    else
    {
        this->X = Y.getX();
        this->Y = Y.getY();
        this->Z = Y.getZ();
        this->__isZeroPoint = false;
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
    if (_Y.isZero())
    {
        return;
    }

    if (isZero())
    {
        *this = _Y;
        return;
    }

    if ((this == &_Y) || ((Y == _Y.getY()) &&
                          (X == _Y.getX()) &&
                          (Z == _Y.getZ())))
    {
        Jacobian_Double(X, Y, Z, __EC);
    }
    else
    {
        Jacobian_Addition(*this, _Y);
    }

    __precomputations.drop();

    return;
}

EC_Point EC_Point::operator+  (const Affine::EC_Point & _Y) const
{
    EC_Point __retval(*this);

    __retval+= _Y;

    return __retval;
}

void EC_Point::operator+= (const Affine::EC_Point & _Y)
{
    if (_Y.isZero())
    {
        return;
    }

    if (isZero())
    {
        *this = toProjective(_Y, __EC);
        return;
    }

    Jacobian_Addition(*this, _Y);

    __precomputations.drop();

    return;
}

using ECZZ_p::Affine::NumBits;
using ECZZ_p::Affine::bit;

void EC_Point::operator*= (const ZZ & Y)
{
    if (IsZero(Y))
    {
        __isZeroPoint = true;
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
        __isZeroPoint = true;
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



/* ----------------------- EC ---------------------------------- */

EC::EC(const Affine::EC & EC)
    : Affine::EC(EC),
      G(toProjective(EC.getBasePoint(), *this))
{}

EC::~EC()
{}

EC_Point EC::create() const
{
    return EC_Point(*this);
}

EC_Point EC::create(const ZZ_p & x,
                    const ZZ_p & y,
                    const ZZ_p & z) const
{
    return EC_Point(x, y, z, *this);
}
