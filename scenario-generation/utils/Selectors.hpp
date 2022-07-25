#ifndef UTILS_SELECTORS_HPP
#define UTILS_SELECTORS_HPP

#include <iostream>
#include <vector>
#include <random>
#include <algorithm>

#include "../include/rapidjson/document.h"

#include "Typedefs.hpp"
#include "IOUtils.hpp"
#include "RandomGenerator.hpp"

template <class T>
class RandomSelector {
public:
    
    // Constructors
    RandomSelector();
    explicit RandomSelector(const std::vector<T>& vec);
    RandomSelector(const std::vector<T>& vec, const std::vector<double>& prs);

    // Queries
    const std::vector<T>& getVec() const;
    const std::vector<double>& getPrs() const;

    T select();
    T& selectRef();
    std::vector<T> selectRandomN(int n, bool replace=false);
    std::vector<T> selectWeightedN(int n, bool replace=false);

    // I/O
    template <class T2>
    friend std::ostream& operator<<(
            std::ostream& oss, 
            const RandomSelector<T2>& rs);

private: 
    
    // The vector of values to select from
    std::vector<T> vec;

    // The associated probabilities with which to select values
    std::vector<double> prs;

};

// Default constructor
template <class T>
RandomSelector<T>::RandomSelector() {}

// Construct a random uniform selector with the values in vec
template <class T>
RandomSelector<T>::RandomSelector(const std::vector<T>& vec) 
    : vec{vec}, prs(vec.size()) {
    std::fill(prs.begin(), prs.end(), 1);
}

// Construct a selector with the values in vec, according to probabilities prs
template <class T>
RandomSelector<T>::RandomSelector(
        const std::vector<T>& vec, 
        const std::vector<double>& prs) 
: vec{vec}, prs{prs} 
{}

// Return the vector of values
template <class T>
const std::vector<T>& RandomSelector<T>::getVec() const { return vec; }

// Return the vector of probabilities
template <class T>
const std::vector<double>& RandomSelector<T>::getPrs() const { return prs; }


// Select a value, and return a copy of it
template <class T>
T RandomSelector<T>::select() {
    std::discrete_distribution<int> distr(prs.begin(), prs.end());
    return vec[distr(generator())];
}

// Select a value, and return a reference to it
template <class T>
T& RandomSelector<T>::selectRef() {
    std::discrete_distribution<int> distr(prs.begin(), prs.end());
    return vec[distr(generator())];
}

// Select N random values, with or without replacement, with uniform weights
template <class T>
std::vector<T> RandomSelector<T>::selectRandomN(int n, bool replace) {
    if (replace) {
        std::uniform_int_distribution<int> distr(0,vec.size()-1);
        std::vector<T> ret(n);
        for (int i = 0; i < n; ++i)
            ret[i] = vec[distr(generator())];
        return ret;
    } else {
        if (n > vec.size()) {
            std::cerr << "RandomSelector Error: " 
                      << vec.size() << "C" << n << std::endl;
            std::exit(1);
        }

        std::vector<int> idx(vec.size());
        for (int i = 0; i < vec.size(); ++i)
            idx[i] = i;
        std::shuffle(idx.begin(), idx.end(), generator());

        std::vector<T> ret(n);
        for (int i = 0; i < n; ++i)
            ret[i] = vec[idx[i]];
        return ret;
    }

}

// Select N random values, with or without replacement, with weights
template <class T>
std::vector<T> RandomSelector<T>::selectWeightedN(int n, bool replace) {
    if (replace) {
        std::discrete_distribution<int> distr(prs.begin(), prs.end());
        std::vector<T> ret(n);
        for (int i = 0; i < n; ++i)
            ret[i] = vec[distr(generator())];
        return ret;
    } else {
        if (n > vec.size()) {
            std::cerr << "RandomSelector Error: "
                      << vec.size() << "C" << n << std::endl;
            std::exit(1);
        }

        std::vector<T> ret(n);
        std::vector<T> v = vec;
        ProbabilityList p = prs;
        for (int i = 0; i < n; ++i) {
            std::discrete_distribution<int> distr(p.begin(), p.end());
            int x = distr(generator());
            ret[i] = v[x];
            v.erase(v.begin() + x);
            p.erase(p.begin() + x);
        }

        return ret;
    }
}

// Print the vector of values and corresponding probabilities
template <class T>
std::ostream& operator<<(std::ostream& oss, const RandomSelector<T>& rs) {
    oss << "RandomSelector(";
    if (!rs.vec.empty()) {
        typename std::vector<T>::const_iterator vecit = rs.vec.begin();
        std::vector<double>::const_iterator prsit = rs.prs.begin();
        oss << *vecit++ << "(" << *prsit++ << ")";
        for (; vecit != rs.vec.end(); ++vecit, ++prsit)
            oss << ", " << *vecit << "(" << *prsit << ")";
    }
    oss << ")";
    return oss;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// SpaceSelector

class SpaceSelector {
public:

    // Constructors
    SpaceSelector();
    SpaceSelector(const SpaceIDList& v, int n=1);
    SpaceSelector(const rj::Value& v, int n=1);

    // Queries
    SpaceIDList select();

    // I/O
    friend std::ostream& operator<<(std::ostream& oss, const SpaceSelector& s);

private:

    // The vector of space ids to select from
    RandomSelector<SpaceID> sel;

    // The number of space ids to select
    int n;

};

// Default Constructor
SpaceSelector::SpaceSelector() {}

// Construct a space selecto with the given list of space ids
SpaceSelector::SpaceSelector(const SpaceIDList& v, int n) : sel{v}, n{n} {}

// Read from the given rapidjson value to initialize the list of space ids
SpaceSelector::SpaceSelector(const rj::Value& v, int n) 
    : n{n} {
    SpaceIDList sids;
    for (const rj::Value& x : v.GetArray())
        sids.push_back(x.GetInt());
    sel = RandomSelector<SpaceID>{sids};
}

// Select n random spaces
SpaceIDList SpaceSelector::select() { return sel.selectRandomN(n); }

// Print the vector of spaces to select from, and how many to select
std::ostream& operator<<(std::ostream& oss, const SpaceSelector& s) {
    oss << "SpaceSelector(" << s.sel.getVec() << ", n=" << s.n << ")";
    return oss;
}

#endif // UTILS_SELECTORS_HPP
