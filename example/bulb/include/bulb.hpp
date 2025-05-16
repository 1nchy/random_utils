#include "random_object.hpp"

struct bulb {
public:
    bulb();
    bulb(const bulb&) = default;
    bulb& operator=(const bulb&) = default;
    ~bulb() = default;
public:
    void light_on();
    void light_out();
    bool light() const;
    void fix(size_t _l);
    size_t remains() const;
    void sweep();
    void check() const;
private:
    icy::random_object<bool> _rob;
    constexpr static size_t _life = 3;
    bool _light = false;
    size_t _count = 0;
};