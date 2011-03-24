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
            // std::cout << "MIXED ADDITION" << std::endl;

            const GF2X & a = P1.getEC().getA();
            const GF2X & b = P1.getEC().getB();
            const GF2XModulus & P = P1.getEC().getModulus();
            const GF2X Z2 = SqrMod(P1.Z, P);
            const GF2X aZ2 = IsOne(a) ? Z2 : MulMod(a, Z2, P);

            /* ------------------------------------- */
            
            const GF2X & X_2 = P2.getX();
            const GF2X & Y_2 = P2.getY();
            const GF2X & Z_1 = P1.getZ();
            const GF2X & X_1 = P1.getX();
            const GF2X & Y_1 = P1.getY();
            
            /* ------------------------------------- */

            const GF2X A = MulMod(Y_2, Z2, P) + Y_1;
            const GF2X B = MulMod(X_2, Z_1, P) + X_1;

            if (IsZero(B))
            {
                P1.X = X_2;
                P1.Y = Y_2;
                P1.Z = GF2X(0, 1);
                
                Lopez_Dahab_Double(P1.X,
                                   P1.Y,
                                   P1.Z,
                                   a,
                                   b,
                                   P);

                return;
            }
            
            const GF2X C = MulMod(Z_1, B, P);
            const GF2X D = MulMod(SqrMod(B, P),
                                  (C + aZ2),
                                  P);
            const GF2X Z_3 = SqrMod(C, P);
            const GF2X E   = MulMod(A, C, P);
            const GF2X X_3 = SqrMod(A, P) + D + E;
            const GF2X F   = MulMod(X_2, Z_3, P) + X_3;
            const GF2X G   = MulMod(X_2 + Y_2,
                                    SqrMod(Z_3, P),
                                    P);
            const GF2X Y_3 = MulMod(E+Z_3, F, P) + G;

            P1.X = X_3;
            P1.Y = Y_3;
            P1.Z = Z_3;
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
    }

    Projective::EC_Point
    toProjective(const Affine::EC_Point & Point,
                 const Projective::EC & EC)
    {
        if (Point.isZero())
            return EC.create();
        else
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
      __precomputations(Point.__precomputations),
      __generic_multiplication()
{}

EC_Point::EC_Point(const GF2X &X,
                   const GF2X &Y,
                   const GF2X &Z, const EC & __EC)
    : X(X), Y(Y), Z(Z), __EC(__EC),
      __isZeroPoint(false),
      __precomputations(),
      __generic_multiplication()
{
    if (! _IsOnCurve())
    {
        throw;
    }
}

EC_Point::EC_Point(const EC & __EC)
    : X(GF2X()),
      Y(GF2X()),
      Z(GF2X()),
      __EC(__EC),
      __isZeroPoint(true),
      __precomputations(),
      __generic_multiplication()
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
    if ((IsOne(__EC.getA())) || (IsZero(__EC.getA())))
        Lopez_Dahab_Mixed_Addition(*this, _Y);
    else
    {
        /* FIXME ADD GENERIC ADDITIONS */
        abort();
    }
    
    __precomputations.drop();
    
    return;
}

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
      G(toProjective(EC.getBasePoint(), *this)),
      G_a(EC.getBasePoint())
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
