#pragma once

namespace Algorithm
{
    template <class Group1,
              class Group2>
    class Multiplication
    {
    protected:
        Multiplication() {};
    public:
        virtual ~Multiplication() {};
        virtual void Multiply(      Group1 &P,
                              const Group2 &Y) const = 0;
        
    };

    template <class Group1,
              class Group2>
    class RLMul : public Multiplication<Group1, Group2>
    {
    public:
        inline RLMul() {};
        inline ~RLMul() {};
        inline void Multiply(      Group1 &P,
                             const Group2 &Y) const
            {
                Group1 S(P);
                Group1 R(P * 0);

                if (Y == IsZero(Y))
                {
                    P *= 0;
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
    };
}
