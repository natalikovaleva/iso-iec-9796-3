#pragma once

namespace Algorithm
{
    template <class To, class From>
    struct conv1
    {
        inline To operator()(const From &);
    };
}
