#pragma once

#include <NTL/ZZ_p.h>

#include <ostream>

using namespace NTL;

class EC;

class EC_Point
{
    
    ZZ_p X;
    ZZ_p Y;
    
    const EC & __EC;

public:
    EC_Point(const ZZ_p &X, const ZZ_p &Y, const EC & __EC); // Generic
    EC_Point(const EC & __EC); // Zero
    ~EC_Point();

    bool _IsOnCurve() const;
    
public:

    inline bool isZero() const;
   
    
    EC_Point & operator=  (const EC_Point & Y);
    EC_Point   operator+  (const EC_Point & Y) const;
    EC_Point   operator*  (const ZZ_p & Y) const;
    
    void operator+= (const EC_Point & Y);
    void operator*= (const ZZ_p & Y);

    inline const ZZ_p & getX() const;
    inline const ZZ_p & getY() const;

    inline bool isSameEC(const EC & __EC) const;

    friend class EC;
    friend std::ostream& operator<<(std::ostream& s, const EC_Point & _EC_Point);
};

class EC
{
    const ZZ_p N;     // Point Order
    const ZZ   P;     // Modulus, get from creators context
    const ZZ_p Seed;  // Random generated seed
    const ZZ_p A;     // EC Params 
    const ZZ_p B;     // EC Params 
    const ZZ_p C;     // EC Params 
    
    const EC_Point G; // Base point

    ZZ_pContext __mod; // Modulus context

    
public:

    EC(const ZZ_p & A,
       const ZZ_p & B,
       const ZZ_p & C,
       const ZZ_p & N,
       //--- TODO Something with it .. ---
       const ZZ_p & Gx,
       const ZZ_p & Gy,
       //---------------------------------
       const ZZ_p & Seed);
    
    ~EC();
    
    EC_Point create(const ZZ_p & x,
                    const ZZ_p & y) const;
    
    inline const EC_Point & get_base_point(void) const { return G; } ;

        
    bool generate_random(ZZ_p & d) const;
    ZZ_p generate_random(void) const;
    
    

    inline void enter_mod_context(void) { __mod.save(); };
    inline void leave_mod_context(void) const { __mod.restore(); } ;


    bool isCorrectOrder() const;
    
    friend class EC_Point;
    friend std::ostream& operator<<(std::ostream& s, const EC & _EC);
};

std::ostream& operator<<(std::ostream& s, const EC & _EC);
std::ostream& operator<<(std::ostream& s, const EC_Point & _EC_Point);

