#pragma once

#include "ec.hpp"

namespace ECGF2X
{
    namespace Affine
    {
        class EC_Defaults
        {

        public:
            enum Sizes
            {
                EC0   = 0,   /* Broken EC */
                EC128 = 128,
                EC163 = 163,
                EC160 = 160,
                EC192 = 192,
                EC283 = 283,
                EC571 = 571
            };

            enum Version
            {
                V1 = 0,
                V2,
                V3
            };

            static Sizes getSize(unsigned int id);
            static unsigned int getId(Sizes size);
            static inline EC_Defaults::Sizes toSizes(long X)
                { return (EC_Defaults::Sizes) X; }
            static inline long toByteSize(const EC_Defaults::Sizes Size)
                { return (long) (Size+7)/8; }

            static void restoreContext(void);

        private:
            static const Sizes __size_matrix[];

        public:

            static EC create(Sizes size, Version ver = V1);

            EC_Defaults();
            ~EC_Defaults();
        };
    }
}
