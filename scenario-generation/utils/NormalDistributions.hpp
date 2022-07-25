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

    // Constructor
    Normal(double mean=0.0, double stdev=1.0);

    // Queries
    double mean() const;
    double stdev() const;

    T sample();

    // I/O
    template<class T2>
    friend std::ostream& operator<<(std::ostream& oss, const Normal<T2>& n);

private:

    // The normal distribution 
    std::normal_distribution<double> distr;

};

// Constructor
template<class T>
Normal<T>::Normal(double mean, double stdev) : distr(mean, stdev) {}

// Return the mean of the normal distribution
template<class T>
double Normal<T>::mean() const { return distr.mean(); }

// Return the standard deviation of the normal distribution
template<class T>
double Normal<T>::stdev() const { return distr.stddev(); }

// Returns a value obtained by sampling the distribution
template<class T>
T Normal<T>::sample() { return distr(generator()); }

// Print the normal distribution
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

    // Constructors
    NormalTime(const std::string& time="00:00:00", 
               const std::string& stdev="00:00:00");
    NormalTime(const Time& time, const std::string& stdev="00:00:00");
    NormalTime(const Time& time, const Time& stdev=Time{0});

    // Queries
    Time mean() const;
    Time stdev() const;

    Time sample();
    
    // I/O
    friend std::ostream& operator<<(std::ostream& oss, const NormalTime& n);

private:

    // The average time of the normal distribution
    Time meanTime;

    // The standard deviation of the normal distribution
    Time stdevTime;

    // A normal distribution of the number of seconds in `stdevTime`
    Normal<double> timeStdev;

};

// Constructor taking in a mean/stdev time in format "HH:MM:SS"
NormalTime::NormalTime(const std::string& time, const std::string& stdev)
: meanTime{time}, stdevTime{stdev}, timeStdev{0, (double)(stdevTime.count())} 
{}

// Constructor taking in a mean time (as type Time) and stdev as "HH:MM:SS"
NormalTime::NormalTime(const Time& time, const std::string& stdev)
: meanTime{time}, stdevTime{stdev}, timeStdev{0, (double)(stdevTime.count())} 
{}

// Constructor taking in a mean/stdev time as type Time
NormalTime::NormalTime(const Time& time, const Time& stdev)
: meanTime{time}, stdevTime{stdev}, timeStdev{0, (double)(stdevTime.count())}
{}

// Return the mean time of the distribution
Time NormalTime::mean() const { return meanTime; }

// Return the standard deviation of the distribution
Time NormalTime::stdev() const { return stdevTime; }

// Returns a value obtained by sampling the distribution
Time NormalTime::sample() 
{ return Time{meanTime + std::chrono::seconds{(int) (timeStdev.sample())}}; }

// Print the normal distribution of times
std::ostream& operator<<(std::ostream& oss, const NormalTime& n) {
    oss << "N(" << n.mean() << "+/-" << n.stdev() << ")";
    return oss;
}

#endif // UTILS_NORMAL_DISTRIBUTION_HPP
