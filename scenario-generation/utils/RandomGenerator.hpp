#ifndef UTILS_RANDOM_GENERATOR_HPP
#define UTILS_RANDOM_GENERATOR_HPP

#include <random>

// Returns the static default random engine for random number generation
std::default_random_engine& generator() {
    static std::random_device device;
    static std::default_random_engine engine{device()};
    return engine;
}

// Return a random double between 0.0 and 1.0
double getRand() {
    static std::uniform_real_distribution<double> distr(0.0, 1.0);
    return distr(generator());
}

// Return a random int between 0 and `max`
int randInt(int max) {
    std::uniform_int_distribution<int> distr{0, max};
    return distr(generator());
}

#endif // UTILS_RANDOM_GENERATOR_HPP
