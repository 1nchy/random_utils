#include "random_object.hpp"

#include <ctime>
#include <cstdlib>
#include <cstdarg>

namespace icy {

void _S_init_random_seed() {
    static bool _inited = false;
    if (!_inited) {
        srand((uint)time(nullptr));
        _inited = true;
    }
}

random_object_base::random_object_base() {
    _S_init_random_seed();
}

}