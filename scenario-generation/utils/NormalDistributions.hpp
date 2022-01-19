#ifndef UTILS_NORMAL_DISTRIBUTION_HPP
#define UTILS_NORMAL_DISTRIBUTION_HPP

#include <iostream>
#include <string>
#include <chrono>
#include <random>
#include <cmath>

#include "../include/date/date.h"

#include "DateUtils.hpp"
#include "RandomGenerator.hpp"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Normal

template<class T>
class Normal {
public:

    Normal(double mean=0.0, double stdev=1.0);

    double mean() const;
    double stdev() const;

    T sample();

    template<class T2>
    friend std::ostream& operator<<(std::ostream& oss, const Normal<T2>& n);

private:

    std::normal_distribution<double> distr;

};

template<class T>
Normal<T>::Normal(double mean, double stdev) : distr(mean, stdev) {}

template<class T>
double Normal<T>::mean() const { return distr.mean(); }

template<class T>
double Normal<T>::stdev() const { return distr.stddev(); }

template<class T>
T Normal<T>::sample() { return distr(generator()); }

template<class T>
std::ostream& operator<<(std::ostream& oss, const Normal<T>& n) {
    oss << "N(" << n.mean() << "," << n.stdev() << ")";
    return oss;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// NormalTime

class NormalTime {
public:

    NormalTime(const std::string& time="00:00:00", 
               const std::string& stdev="00:00:00");
    NormalTime(const Time& time, const std::string& stdev="00:00:00");
    NormalTime(const Time& time, const Time& stdev=Time{0});

    Time mean() const;
    Time stdev() const;

    Time sample();
    
    friend std::ostream& operator<<(std::ostream& oss, const NormalTime& n);

private:

    Time meanTime;
    Time stdevTime;
    Normal<double> timeStdev;

};

NormalTime::NormalTime(const std::string& time, const std::string& stdev)
: meanTime{time}, stdevTime{stdev}, timeStdev{0, (double)(stdevTime.count())} 
{}

NormalTime::NormalTime(const Time& time, const std::string& stdev)
: meanTime{time}, stdevTime{stdev}, timeStdev{0, (double)(stdevTime.count())} 
{}

NormalTime::NormalTime(const Time& time, const Time& stdev)
: meanTime{time}, stdevTime{stdev}, timeStdev{0, (double)(stdevTime.count())}
{}

Time NormalTime::mean() const { return meanTime; }
Time NormalTime::stdev() const { return stdevTime; }

Time NormalTime::sample() 
{ return Time{meanTime + std::chrono::seconds{(int) (timeStdev.sample())}}; }

std::ostream& operator<<(std::ostream& oss, const NormalTime& n) {
    oss << "N(" << n.mean() << "+/-" << n.stdev() << ")";
    return oss;
}

#endif // UTILS_NORMAL_DISTRIBUTION_HPP
