#include "Mathematics.hpp"
#include "Lang.hpp"

#include <stdlib.h>

namespace smk
{
    // random number between 0.0 and 1.0
     float uniformRandom() {
         const float max_float = static_cast<float>(RAND_MAX);
         const float rand_float = static_cast<float>(rand());
         return rand_float / max_float;
     }

    float uniformRandomRange(float minv, float maxv) {
        return minv + (maxv - minv) * uniformRandom();
    }
}
