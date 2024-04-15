#include "rand.h"

Xoroshiro128Plus::Xoroshiro128Plus(uint64_t seed) {
    state[0] = splitmix64(seed);
    state[1] = splitmix64(state[0]);
};

uint64_t Xoroshiro128Plus::operator()() {
    const uint64_t s0 = state[0];
    uint64_t s1 = state[1];
    const uint64_t result = s0 + s1;

    s1 ^= s0;
    state[0] = rotl(s0, 55) ^ s1 ^ (s1 << 14);
    state[1] = rotl(s1, 36);

    return result;
}

uint64_t Xoroshiro128Plus::rotl(const uint64_t x, int k) {
    return (x << k) | (x >> (64 - k));
}

uint64_t Xoroshiro128Plus::splitmix64(uint64_t x) {
    x += 0x9e3779b97f4a7c15;
    x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9;
    x = (x ^ (x >> 27)) * 0x94d049bb133111eb;
    return x ^ (x >> 31);
}
