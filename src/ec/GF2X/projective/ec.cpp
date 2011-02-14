#include "ec/GF2X/projective/ec.hpp"

#include "ec/GF2X/affine/ec.hpp"
#include "ec/GF2X/affine/utils.hpp"

using namespace ECGF2X;
using namespace ECGF2X::Projective;

/* ----------------------- convert ----------------------------- */

/* Generic doubling in Lopez-Dahab Projective coordinates
 * p.94 (115) */

namespace ECGF2X
{
    namespace Projective
    {
        inline void 
        Lopez_Dahab_Double(GF2X & X,
                           GF2X & Y,
                           GF2X & Z,
                           const GF2X & A,
                           const GF2X & B,
                           const GF2XModulus & P)
        {
            const GF2X X2 = SqrMod(X, P);
            const GF2X Y2 = SqrMod(Y, P);
            const GF2X Z2 = SqrMod(Z, P);
            const GF2X X4 = SqrMod(X2,P);
            const GF2X bZ4 = MulMod(B, SqrMod(Z2, P), P);

            MulMod(Z, X2, Z2, P); // Z now == Z_3
            add(X, X4, bZ4);      // X now == X_3
    
            const GF2X aZ_3 = MulMod(A, Z, P);
            const GF2X Braces = aZ_3 + Y2 + bZ4;

            add(Y, MulMod(bZ4, Z, P),
                MulMod(X, Braces, P));
        }

        /* Generic addition in Lopez-Dahab Projective coordinates
         * p.94 (115) */

        inline void 
        Lopez_Dahab_Mixed_Addition(EC_Point & P1,
                                   const Affine::EC_Point & P2)
        {
            const GF2XModulus & P = P1.getEC().getModulus();
            const GF2X & a = P1.getEC().getA();
            /* -------------------------------------- */
            const GF2X Z2  = SqrMod(P1.Z, P);
            const GF2X aZ2 = MulMod(a, Z2, P);
            /* -------------------------------------- */
            const GF2X A = MulMod(P2.getY(), Z2, P) + P1.Y;
            const GF2X B = MulMod(P2.getX(), P1.Z, P) + P1.X;
            const GF2X C = MulMod(P1.Z, B, P);
            const GF2X D = MulMod(SqrMod(B, P),
                                  C + aZ2,
                                  P);
            const GF2X E = MulMod(A, C, P);
    
            SqrMod(P1.Z, C, P);
            /* P1.Z == Z_3 */ 
            add(P1.X, SqrMod(A, P), D);
            P1.X += E;
            /* P1.X == X_3 */

            const GF2X F = MulMod(P2.getX(), P1.Z, P) + P1.X;
            const GF2X G = MulMod(SqrMod(P1.Z, P),
                                  (P2.getX() + P2.getY()),
                                  P);
    
            P1.Y = MulMod(E + P1.Z, F, P) + G;

            /* P1 == P3 */
        }

        /* LNCS. 2000 / 1977, 10.1.1.75.402 */
        inline void 
        Lopez_Dahab_Addition(EC_Point & P1,
                             const EC_Point & P2)
        {
            const GF2XModulus & P = P1.__EC.getModulus();
            /* -------------------------------------- */
            const GF2X A1 = MulMod(P1.X, P2.Z, P);
            const GF2X A2 = MulMod(P2.X, P1.Z, P);
            const GF2X C  = A1 + A2;
            const GF2X B1 = SqrMod(A1, P);
            const GF2X B2 = SqrMod(A2, P);
            const GF2X D  = B1 + B2;
            const GF2X E1 = MulMod(P1.Y, SqrMod(P2.Z, P), P);
            const GF2X E2 = MulMod(P2.Y, SqrMod(P1.Z, P), P);
            const GF2X F  = E1 + E2;
            const GF2X G  = MulMod(C, F, P);

            P1.Z = MulMod(MulMod(P1.Z, P2.Z, P),
                          D, P);
            P1.X = MulMod(A1, E2 + B2, P) +
                MulMod(A2, E1 + B1, P);
            P1.Y = MulMod(MulMod(A1, G, P) + MulMod(E1, D, P),
                          D, P) + MulMod(G + P1.Z, P1.X, P);
        }

        inline void
        Right_To_Left_Multiplication(EC_Point & P,
                                     const ZZ & Y)
        {
            EC_Point S(P);
            EC_Point R(P.getEC());

            if (IsZero(Y))
            {
                return;
            }

            for (long i = 0; i < NumBits(Y); i++)
            {
                if (bit(Y, i))
                {
                    R += S;
                }

                S += S;
            }
    
            P = R;
        }
    }

    Projective::EC_Point
    toProjective(const Affine::EC_Point & Point,
                 const Projective::EC & EC)
    {
        
        return EC.create(Point.getX(), Point.getY(),
                         GF2X(0, 1));
    }



    Affine::EC_Point 
    toAffine(const Projective::EC_Point & Point,
             const Affine::EC & EC)
    {
        const GF2X & P = EC.getModulus();
        const GF2X iZ = InvMod(Point.getZ(), P);

        return EC.create(MulMod(Point.getX(), iZ, P),
                         MulMod(Point.getY(), SqrMod(iZ, P), P));
    }

}


/* ---------------------- Points ------------------------ */

EC_Point::EC_Point(const EC_Point & Point)
    : X(Point.X),
      Y(Point.Y),
      Z(Point.Z),
      __EC(Point.__EC),
      __isZeroPoint(Point.__isZeroPoint),
      __isPrecomputed(false),
      __precomputations(NULL),
      __precomputations_window(0)
{}

EC_Point::EC_Point(const EC_Point & Point, bool isZero)
    : X(isZero ? GF2X() : Point.X),
      Y(isZero ? GF2X() : Point.Y),
      Z(isZero ? GF2X() : Point.Z),
      __EC(Point.__EC),
      __isZeroPoint(isZero),
      __isPrecomputed(false)

{}


EC_Point::EC_Point(const GF2X &X,
                   const GF2X &Y,
                   const GF2X &Z, const EC & __EC)
    : X(X), Y(Y), Z(Z), __EC(__EC),
      __isZeroPoint(false),
      __isPrecomputed(false)
{
    if (! _IsOnCurve())
        throw;
}

EC_Point::EC_Point(const EC & __EC)
    : X(GF2X()),
      Y(GF2X()),
      Z(GF2X()),
      __EC(__EC),
      __isZeroPoint(true),
      __isPrecomputed(false)
{}

EC_Point::~EC_Point()
{}

/* Lopez - Dahab */

// ___ must be in proper context __
bool EC_Point::_IsOnCurve() const
{
    if (__isZeroPoint)
        return true;

    const GF2XModulus & P = __EC.getModulus();

    // Y^2 + XYZ = ZX^3 + aX^2Z^2 + bZ^4

    const GF2X X2 = SqrMod(X,     P);
    const GF2X X3 = MulMod(X2, X, P);
    const GF2X Y2 = SqrMod(Y,     P);
    const GF2X Z2 = SqrMod(Z,     P);
    const GF2X Z4 = SqrMod(Z2,    P);
        
    const GF2X L_Part = Y2 + MulMod(MulMod(X, Y, P), Z, P);
    const GF2X R_Part =
        MulMod(X3, Z, P)
        + MulMod(MulMod(__EC.getA(), X2, P), Z2, P) // TODO: A={0,1}
        + MulMod(__EC.getB(), Z4, P);

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
        throw; // assert

    if (Y.__isZeroPoint)
        this->__isZeroPoint = true;
    else
    {
        this->X = Y.getX();
        this->Y = Y.getY();
        this->Z = Y.getZ();
        this->__isZeroPoint = false;
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
    
    if ((this == &_Y) || (Y== _Y.getY()))
    {
        Lopez_Dahab_Double(X, Y, Z,
                           __EC.getA(),
                           __EC.getB(),
                           __EC.getModulus());
    }
    else
    {
        Lopez_Dahab_Addition(*this, _Y);
    }
    
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

    /* Doubling couldn't be */
    Lopez_Dahab_Mixed_Addition(*this, _Y);
    
    return;
}

void EC_Point::operator*= (const ZZ & Y)
{
    if (isPrecomputed())
    {
    }
    else
        Right_To_Left_Multiplication(*this, Y);
    return;
}

EC_Point EC_Point::operator* (const ZZ & Y) const
{
    EC_Point __retval(*this);

    __retval*= Y;

    return __retval;
}

bool EC_Point::precompute(void)
{
    __precomputations_window = 1 * sizeof(char) * 8;
    
    __precomputations = new EC_Point *[1 << __precomputations_window];

    for (unsigned int i = 0; i < (1 << __precomputations_window); i++)
    {
        char bytes[4096]; // FIXME
    }
    
    
    return true;
}



/* ----------------------- EC ---------------------------------- */

EC::EC(const Affine::EC & EC)
    : Affine::EC(EC), G(toProjective(EC.getBasePoint(), *this))
{}

EC::~EC()
{}

EC_Point EC::create() const
{
    return EC_Point(*this);
}

EC_Point EC::create(const GF2X & x,
                    const GF2X & y,
                    const GF2X & z) const
{
    return EC_Point(x, y, z, *this);
}
