#pragma once

#include "algorithm/convertors.hpp"
#include "algorithm/precomputations.hpp"
#include "algorithm/comb_idx.hpp"

#include "dss/dss.hpp"
#include "generic/zz_utils.hpp"

using namespace NTL;

/* Non thread-safe */

namespace Algorithm
{

    template <class Group1,
              class PGroup1>

    class Precomputations_deComb : public Precomputations_Logic <Group1,
                                                               ZZ>
    {
    public:
                /* This algorithm requires ZZ / Special random generator usage */

        class Generator : public generateRandomValueCallback
        {
            const unsigned int _window;
            const unsigned int _items;
            const unsigned int _max;
            const ZZ           _mask;
            const ZZ           _imask;

            unsigned int _decomb_idx;
            ZZ           _prev;
            Group1      * _cache;

        private:
            static ZZ makeRandomMask(unsigned int window,
                                     unsigned int items,
                                     unsigned int max
                                     )
                {
                    ZZ mask;
                    unsigned int d = max/window;

                    for (unsigned int i=0; i<window; i++)
                    {
                        for (unsigned int j=0; j<items; j++)
                        {
                            SetBit(mask, i*d+(d - j - 1));
                        }
                    }

                    return mask;
                }

            static ZZ makeInvRandomMask(unsigned int window,
                                        unsigned int items,
                                        unsigned int max)
                {
                    ZZ mask;
                    unsigned int d = max/window;

                    for (unsigned int i=0; i<window; i++)
                    {
                        for (unsigned int j=items; j<window; j++)
                        {
                            SetBit(mask, i*d+(d - j - 1));
                        }
                    }

                    return mask;
                }

        public:
            Generator(unsigned int window,
                      unsigned int items,
                      unsigned int max)
                : _window(window),
                  _items(items),
                  _max(max),
                  _mask(makeRandomMask(window, items, max)),
                  _imask(makeInvRandomMask(window, items, max)),
                  _decomb_idx(0),
                  _prev(),
                  _cache(NULL)
                {}

            PGroup1 * cache()
                { return _cache;  }

            void cache(PGroup1 * cache)
                { if (_cache) delete _cache; _cache = cache; _decomb_idx ++; }

            bool cached() const
                {
                    //std::cout << "_decomb_idx == " << _decomb_idx << //std::endl;
                    return ( _decomb_idx % 2 == 1 );
                }

            unsigned int items() const
                { return _items; }

            ZZ getMask()
                { return _mask; }

            ZZ getIMask()
                { return _imask; }

            Octet getRandomValue()
                {
                    //std::cout << "GET RANDOM VALUE!" << //std::endl;
                    /*             [ description ]
                     * |   *****|********|********|********| real
                     * |++++++++|++++++++|++++++++|++++++++| cache
                     * |!!!.....|!!!.....|!!!.....|!!!.....| mask
                     * |!!!*****|!!!*****|!!!*****|!!!*****| output
                     *  ^___ max                          0
                     * |<block->|
                     * |<--------------- max ------------->|
                     * i.e: blocks: 4
                     *      window: 4
                     *      items:  3
                     *                                             */

                    /* Example:
                     * cache:    0011|0011|1100|1101|0111
                     * mask:     1000|1000|1000|1000|1000
                     * &1:       0000|0000|1000|1000|0000
                     *
                     * new:      0100|1001|1110|0101|0001
                     *~mask:     0001|0001|0001|0001|0001
                     * &2:       0000|0000|0000|0000|0000
                     *
                     * &1 || &2: 0101|1001|1000|1000|0000
                     *                                             */


                    if (! cached())
                    {
                        RandomBits(_prev, _max);
                        return I2OSP(_prev);
                    }
                    else
                    {
                        ZZ base = RandomBits_ZZ(_max);
                        return I2OSP((_prev & _mask) | (base & _imask));
                    }
                }

            unsigned int getWindow() const
                {
                    return _window;
                }
            unsigned int getItems() const
                {
                    return _items;
                }
            unsigned int getMax() const
                {
                    return _max;
                }


        };

        /* ----- PRECOMPUTATION REALIZATION ----- */

    private:

        const long __precomputations_window;
        const long __precomputations_elements;

        PGroup1 ** __precomputations;

        const long __precomputations_max_size;
        const long __precomputations_portion;

        Generator & __decombContext;

    private:
        Precomputations_deComb(Precomputations_deComb & Source);

    public:
        Precomputations_deComb()
            : __precomputations_window(0),
              __precomputations_elements(0),
              __precomputations(NULL),
              __precomputations_max_size(0),
              __precomputations_portion(0),
              __decombContext(0, 0)
            {} /* Empty precomputations. Totaly useless */

        Precomputations_deComb(const Group1 & Source,
                               Generator & generator)
            : __precomputations_window(generator.getWindow()),
              __precomputations_elements(1 << __precomputations_window),
              __precomputations(new PGroup1 * [ __precomputations_elements ]),
              __precomputations_max_size(((generator.getMax() +
                                           generator.getWindow() - 1) / generator.getWindow()) *
                                         generator.getWindow()),
              __precomputations_portion(__precomputations_max_size / __precomputations_window),
              __decombContext(generator)
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


        ~Precomputations_deComb()
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
                            const ZZ & Y)
            {
                if (__decombContext.cached())
                {
                    //std::cout << "Cached? " << __decombContext.cached() << //std::endl;
                    _Multiply_Cached(P, Y);
                }

                else
                    _Multiply_Noncached(P, Y);
            }

        inline bool isReady(void)
            { return true; }

    private:
        void _Multiply_Noncached(      Group1 & P,
                            const ZZ & Y)
            {
                P *= 0;
                long i = 0;

                //std::cout << "FULL, by " << Y << //std::endl;

                for (;
                     i < __decombContext.items();
                     i++)
                {
                    P += P;
                    P += getPrecomputedForMul(Y, i);
                }

                P += P;
                __decombContext.cache(new Group1(P));
                P += getPrecomputedForMul(Y, i);
                i ++;

                for (;
                     i < getMulPortions();
                     i++)
                {
                    P += P;
                    P += getPrecomputedForMul(Y, i);
                }

                //std::cout << "Decomb: " << i << "iterations" << //std::endl;
            }

        void _Multiply_Cached(      Group1 & P,
                                    const ZZ & Y)
            {

                P = *__decombContext.cache();

                //std::cout << "CACHED: " << P << //std::endl;
                //std::cout << "by " << Y << //std::endl;

                long i = __decombContext.items();

                P += getPrecomputedForMul(Y, i);
                i ++;

                for (;
                     i < getMulPortions();
                     i++)
                {
                    P += P;
                    P += getPrecomputedForMul(Y, i);
                }
                //std::cout << "Decomb: " << i << "iterations" << //std::endl;
            }

        long getMulPortions(void) const
            { return __precomputations_portion; }

        const PGroup1 & getPrecomputedForMul(const ZZ & scalar,
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

        static inline long comb_build_index(const ZZ & Y,
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
              class PGroup1>
    class Precomputations_Method_deComb : public Precomputations_Method<Group1,ZZ>
    {
        typedef typename Precomputations_deComb<Group1, PGroup1>::Generator dcGenerator;
        dcGenerator & __decombContext;

    public:
        Precomputations_Method_deComb(dcGenerator & decombContext)
            : Precomputations_Method<Group1, ZZ>(decombContext.getMax()),
              __decombContext(decombContext) {};

    public:
        Precomputations<Group1, ZZ>
        operator()(const ZZ & arg2 __attribute__ ((unused))) const
            __attribute__ ((noreturn))
            { throw; } // NOT IMPLEMENTED

        Precomputations<Group1, ZZ>
        operator()(const Group1 & arg1 __attribute__ ((unused)),
                   const ZZ & arg2 __attribute__ ((unused))) const
            __attribute__ ((noreturn))
            { throw; } // NOT IMPLEMENTED

        Precomputations<Group1, ZZ>
        operator()(const Group1 & arg1) const
            { Precomputations<Group1,
                              ZZ>
                    ret(new Precomputations_deComb<Group1,
                        PGroup1> (arg1, __decombContext));
                return ret; }
    };

}
