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
    
    RandomSelector();
    explicit RandomSelector(const std::vector<T>& vec);
    RandomSelector(const std::vector<T>& vec, const std::vector<double>& prs);

    T select();
    T& selectRef();
    std::vector<T> selectRandomN(int n, bool replace=false);
    std::vector<T> selectWeightedN(int n, bool replace=false);

    const std::vector<T>& getVec() const;
    const std::vector<double>& getPrs() const;

    template <class T2>
    friend std::ostream& operator<<(
            std::ostream& oss, 
            const RandomSelector<T2>& rs);

private: 
    
    std::vector<T> vec;
    std::vector<double> prs;

};

template <class T>
RandomSelector<T>::RandomSelector() {}

template <class T>
RandomSelector<T>::RandomSelector(const std::vector<T>& vec) 
    : vec{vec}, prs(vec.size()) {
    std::fill(prs.begin(), prs.end(), 1);
}

template <class T>
RandomSelector<T>::RandomSelector(
        const std::vector<T>& vec, 
        const std::vector<double>& prs) 
: vec{vec}, prs{prs} {}

template <class T>
T RandomSelector<T>::select() {
    std::discrete_distribution<int> distr(prs.begin(), prs.end());
    return vec[distr(generator())];
}

template <class T>
T& RandomSelector<T>::selectRef() {
    std::discrete_distribution<int> distr(prs.begin(), prs.end());
    return vec[distr(generator())];
}

template <class T>
const std::vector<T>& RandomSelector<T>::getVec() const { return vec; }

template <class T>
const std::vector<double>& RandomSelector<T>::getPrs() const { return prs; }

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

    SpaceSelector();
    SpaceSelector(const SpaceIDList& v, int n=1);
    SpaceSelector(const rj::Value& v, int n=1);

    SpaceIDList select();

    friend std::ostream& operator<<(std::ostream& oss, const SpaceSelector& s);

private:

    RandomSelector<SpaceID> sel;
    int n;

};

SpaceSelector::SpaceSelector() {}

SpaceSelector::SpaceSelector(const SpaceIDList& v, int n) : sel{v}, n{n} {}

SpaceSelector::SpaceSelector(const rj::Value& v, int n) 
    : n{n} {
    SpaceIDList sids;
    for (const rj::Value& x : v.GetArray())
        sids.push_back(x.GetInt());
    sel = RandomSelector<SpaceID>{sids};
}

SpaceIDList SpaceSelector::select() { return sel.selectRandomN(n); }

std::ostream& operator<<(std::ostream& oss, const SpaceSelector& s) {
    oss << "SpaceSelector(" << s.sel.getVec() << ", n=" << s.n << ")";
    return oss;
}

#endif // UTILS_SELECTORS_HPP
