#pragma once

#include <ostream>
#include <tr1/memory>

namespace Algorithm
{
    using std::tr1::shared_ptr;

    /* Algoritm definition */
    template <class Group1,
              class Group2>
    class Precomputations_Logic
    {
    public:
        Precomputations_Logic() {};
        virtual ~Precomputations_Logic() {};
        virtual bool isReady(void) = 0;
        virtual void Multiply(      Group1 & P,
                              const Group2 & Y) = 0;
    };

    /* This must be route class, now just do nothing */
    template <class Group1,
              class Group2>
    class Precomputations
    {
    private:
        shared_ptr<Precomputations_Logic <Group1,
                                          Group2> > __logic;
        bool __isActual;
        
    public:
        bool isReady(void) const
            {   if (__isActual == false) return false;
                if (__logic == NULL) return false;
                else return __logic->isReady(); }

    public:
        Precomputations()
            : __isActual(false) {}
        
        Precomputations(Precomputations_Logic<Group1, Group2> * Logic)
            : __logic(Logic), __isActual(true) {}
            
        Precomputations(const Precomputations<Group1, Group2> & Source)
            : __logic(Source.__logic), __isActual(true) {}
            
        ~Precomputations() {}
        
        void operator= (const Precomputations<Group1, Group2> & Source)
            { __logic = Source.__logic; }

        void drop()
            { __isActual = false; };
                
    public:
        void Multiply(      Group1 & P,
                      const Group2 & Y)
            {
                if (__logic != NULL)
                    __logic->Multiply(P, Y);
                else throw;
                // else return;
            }
    };

    template <class Group1,
              class Group2>
    class Precomputations_Method
    {
    protected:
        const size_t __mulSize;
        const size_t __addSize;
        
        inline size_t getMulSize() const { return __mulSize; }
        inline size_t getAddSize() const { return __addSize; }
        
    public:
        /* Pass parameters to it */
        Precomputations_Method(size_t mulSize = 0,
                               size_t addSize = 0)
            : __mulSize(mulSize),
              __addSize(addSize)
            {}
        virtual ~Precomputations_Method()
            {}
        
        virtual Precomputations<Group1, Group2>
        operator()(const Group1 & arg1,
                   const Group2 & arg2) const = 0;
        
        virtual Precomputations<Group1, Group2>
        operator()(const Group1 & arg1) const = 0;
        
        virtual Precomputations<Group1, Group2>
        operator()(const Group2 & arg1) const = 0;
    };

}

            
        
            
