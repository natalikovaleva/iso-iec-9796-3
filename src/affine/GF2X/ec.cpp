#include "affine/GF2X/ec.hpp"
#include "affine/GF2X/utils.hpp"

using namespace Affine::GF2X;

/* ---------------------- Points ------------------------ */

EC_Point::EC_Point(const EC_Point & Point)
    : X(Point.X),
      Y(Point.Y),
      __EC(Point.__EC),
      isZeroPoint(Point.isZeroPoint)
{}

EC_Point::EC_Point(const EC_Point & Point, bool isZero)
    : X(isZero ? GF2X() : Point.X),
      Y(isZero ? GF2X() : Point.Y),
      __EC(Point.__EC)
{}


EC_Point::EC_Point(const GF2X &X, const GF2X &Y, const EC & __EC)
    : X(X), Y(Y), __EC(__EC), isZeroPoint(false)
{
    if (! _IsOnCurve())
        throw;
}

EC_Point::EC_Point(const EC & __EC)
    : X(GF2X()),
      Y(GF2X()),
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
    
    GF2X Y2, X3;
    bool __result = false;
    
    SqrMod(Y2, Y, __EC.P_mod);

    std::cout << "Y2: " << Y2 << std::endl;
    std::cout << "__EC.P_mod: " << __EC.P_mod << std::endl;
    
    PowerMod(X3, X, 3, __EC.P_mod);

    GF2X L_Part;
    
    MulMod(L_Part, __EC.A, X, __EC.P_mod);

    L_Part += X3;
    L_Part += __EC.B;

    std::cout << "L_part: " << L_Part << std::endl;
    std::cout << "Y2: " << Y2 << std::endl;
    
    if (L_Part == Y2 )
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
    
    GF2X L;
    
    if ((this == &_Y) ||
        (X==YX) || (Y==YY))
    {
        L = (( 3 * sqr(X) + __EC.A ) / (2 * Y) ) % __EC.P_mod;
    }
    else
    {
        L = ((Y - YY) / (X - YX)) % __EC.P_mod;
    }
    
    GF2X X3 = sqr(L) - X - YX;

    Y = (L*(X-X3) - Y) % __EC.P_mod;
    X = X3             % __EC.P_mod;

    return;
}

void EC_Point::operator*= (const GF2X & Y)
{
    EC_Point S(*this);
    EC_Point R(__EC);

    if (IsZero(Y))
    {
        return;
    }

    for (long i = 0; i < NumBits(Y); i++)
    {
        if (IsOne(coeff(Y, i)))
        {
            R += S;
        }

        S += S;
    }
    
    *this = R;

    return;
}

EC_Point EC_Point::operator* (const GF2X & Y) const
{
    EC_Point __retval(*this);

    __retval*= Y;

    return __retval;
}

/* --------------------- Curves ------------------------- */

EC::EC(const GF2X & A,
       const GF2X & B,
       const GF2X & C,
       const GF2X & N,
       const GF2X & P,
       //--- TODO Something with it .. ---
       const GF2X & Gx,
       const GF2X & Gy,
       //---------------------------------
       const GF2X & Seed)
    : N(N), P(P),
      Seed(Seed), A(A), B(B), C(C),
      N_mod(N), P_mod(P), P_deg(deg(P)),
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

GF2X EC::generate_random() const
{
    return random_GF2X(deg(N));
}


std::ostream& Affine::GF2X::operator<<(std::ostream& s, const EC & _EC)
{
    s << "X^3 + " << _EC.getA() <<"X + " <<
        _EC.getB() << " (mod " << _EC.getModulus() << ")";

    return s;
}

std::ostream& Affine::GF2X::operator<<(std::ostream& s, const EC_Point & _EC_Point)
{
    s << "( x: " << I2OSP(_EC_Point.X) << "; y: " <<
        I2OSP(_EC_Point.Y) << ")";

    return s;
}

bool EC::isCorrectOrder() const
{
    return (G * (N + 1)) == G;
}
