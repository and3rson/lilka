#include <stdint.h>

// So why do we need this?
// - rand_r(seed) does not produce satisfactory results
// - std::mt19937 is too slow
// - std::minstd_rand0 does not seem random at all
// - LFSR depends on state and it's hard to make it idempotent
// So behold - Xoroshiro128+!
// https://en.wikipedia.org/wiki/Xoroshiro128%2B
// /AD

class Xoroshiro128Plus {
public:
    explicit Xoroshiro128Plus(uint64_t seed);
    uint64_t operator()();
    inline uint64_t max() const {
        return UINT64_MAX;
    }

private:
    static uint64_t rotl(const uint64_t x, int k);
    static uint64_t splitmix64(uint64_t x);
    // We don't really need the state at all, since we always generate one number at a time for any given seed.
    // Keeping it here in case we need to generate multiple numbers in the future.
    uint64_t state[2];
};
