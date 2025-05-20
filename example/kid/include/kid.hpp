#include "random_object.hpp"

#include <vector>

struct kid {
public:
    static constexpr unsigned MAX_ENERGY = 10;
    kid() = default;
    kid(const kid& _rhs);
    kid(kid&&);
    ~kid() = default;
    auto add(int _x) -> int;
    auto add(int _x, int _y) -> int;
    auto add(int _x, int _y, int _z) -> int;
    auto increment(int& x) -> void;
    auto rest() -> void;
    auto show() const -> void;
    auto prepare() -> void;
    auto check() const -> void;
private:
    auto bias(unsigned _x) const -> int;
    auto increase_energy(unsigned) -> void;
    auto decrease_energy(unsigned) -> void;
private:
    std::vector<int> _args;
    int _result;
    icy::random_object<bool> _roi;
    icy::random_object<unsigned> _rou;
    unsigned _energy = MAX_ENERGY;
};