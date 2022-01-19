#ifndef UTILS_RANDOM_GENERATOR_HPP
#define UTILS_RANDOM_GENERATOR_HPP

#include <random>

std::default_random_engine& generator() {
    static std::random_device device;
    static std::default_random_engine engine{device()};
    return engine;
}

double getRand() {
    static std::uniform_real_distribution<double> distr(0.0, 1.0);
    return distr(generator());
}

int randInt(int max) {
    std::uniform_int_distribution<int> distr{0, max};
    return distr(generator());
}

#endif // UTILS_RANDOM_GENERATOR_HPP
