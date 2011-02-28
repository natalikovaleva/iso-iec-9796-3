#pragma once

#include "algorithm/convertors.hpp"
#include "algorithm/precomputations.hpp"
#include "algorithm/comb_idx.hpp"

namespace Algorithm
{
    
    template <class Group1,
              class Group2,
              class PGroup1>
    
    class Precomputations_Comb : public Precomputations_Logic <Group1,
                                                               Group2>
    {
        const long __precomputations_window;
        const long __precomputations_elements;
            
        PGroup1 ** __precomputations;
            
        const long __precomputations_max_size;
        const long __precomputations_portion;

    private:
        Precomputations_Comb(Precomputations_Comb & Source);
        
    public:
        Precomputations_Comb()
            : __precomputations_window(0),
              __precomputations_elements(0),
              __precomputations(NULL),
              __precomputations_max_size(0),
              __precomputations_portion(0)
            {} /* Empty precomputations. Totaly useless */
        
        Precomputations_Comb(const Group1 & Source,
                                   size_t MaxMulSize)
            : __precomputations_window(comb_window),
              __precomputations_elements(1 << __precomputations_window),
              __precomputations(new PGroup1 * [ __precomputations_elements ]),
              __precomputations_max_size(((MaxMulSize +
                                           comb_window - 1) / comb_window) *
                                         comb_window),
              __precomputations_portion(__precomputations_max_size / __precomputations_window)
            {
                conv1<PGroup1, Group1> groupConvertor;
                
                PGroup1 sPoint(groupConvertor(Source)); // Do something here?

                /* Fill 0, 1 */
                /* -------------------------------------------------------- */
                __precomputations[0] = new PGroup1(sPoint * 0); // Zero
                __precomputations[1] = new PGroup1(sPoint);     // This point
                /* ----------L----------------------------------------- */

                PGroup1 * prev = __precomputations[1];

                for (long i = 1; i<__precomputations_window; i++)
                {
                    __precomputations[1<<i] =
                        new PGroup1(
                            wdouble(*prev,
                                    __precomputations_portion));
        
                    prev = __precomputations[1<<i];
                }
    
                for (long i = 0; i<__precomputations_elements - __precomputations_window - 1; i++)
                {
                    long t_idx = comb_idx[i*3];
                    long f_idx = comb_idx[i*3 + 1];
                    long s_idx = comb_idx[i*3 + 2];

                    __precomputations[t_idx] = new PGroup1(*__precomputations[f_idx] +
                                                           *__precomputations[s_idx]);
                }
            }

            
        ~Precomputations_Comb()
            {
                for (long i = 0; i< __precomputations_elements; i++)
                {
                    delete __precomputations[i];
        
                    __precomputations[i] = NULL;
                }
                
                delete [] __precomputations;
            }
        

    public:
        void Multiply(      Group1 & P,
                      const Group2 & Y)
            {
                P *= 0;
                
                for (long i = 0;
                     i < getMulPortions();
                     i++)
                {
                    P += P;
                    P += getPrecomputedForMul(Y, i);
                }
            }
        
        inline bool isReady(void) 
            { return true; }
            
            
    private:
        long getMulPortions(void) const
            { return __precomputations_portion; }
        
        const PGroup1 & getPrecomputedForMul(const Group2 & scalar,
                                             const long portion) const
            {
                if (NumBits(scalar) > __precomputations_max_size)
                {
                    throw;
                }

                return * __precomputations[ comb_build_index(scalar,
                                                             __precomputations_window,
                                                             __precomputations_portion,
                                                             __precomputations_portion - portion - 1) ];
            }

        /* ---- REALIZATION DETAILS ---- */
        
        static inline PGroup1 wdouble(const PGroup1 & input,
                                      long window)
            {
                PGroup1 r(input);
                
                for (long i=0; i<window; i++)
                    r += r;
                
                return r;
            }

        static inline long comb_build_index(const Group2 & Y,
                                            const long window,
                                            const long portions,
                                            const long iteration)
            {
                long index = 0x0;
                
                for (long i = 0; i<window; i++)
                {
                    index |=
                        bit(Y, i*portions + iteration) << i;
                }
                return index;
            }
    };

    template <class Group1,
              class Group2,
              class PGroup1>
    class Precomputations_Method_Comb : public Precomputations_Method<Group1,
                                                                      Group2>
    {
    public:
        Precomputations_Method_Comb(size_t MulSize)
            : Precomputations_Method<Group1, Group2>(MulSize) {};
        
    public:
        Precomputations<Group1, Group2>
        operator()(const Group2 & arg2 __attribute__ ((unused))) const
            __attribute__ ((noreturn))
            { throw; } // NOT IMPLEMENTED
        
        Precomputations<Group1, Group2>
        operator()(const Group1 & arg1 __attribute__ ((unused)),
                   const Group2 & arg2 __attribute__ ((unused))) const
            __attribute__ ((noreturn))
            { throw; } // NOT IMPLEMENTED
        
        Precomputations<Group1, Group2>
        operator()(const Group1 & arg1) const
            { Precomputations<Group1,
                              Group2>
                    ret(new Precomputations_Comb<Group1,
                                                 Group2,
                                                 PGroup1> (arg1, this->getMulSize()));
                return ret; }
    };
}
