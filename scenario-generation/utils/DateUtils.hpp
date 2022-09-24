#ifndef UTILS_DATEUTILS_HPP
#define UTILS_DATEUTILS_HPP

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <utility>

#include "../include/date/date.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Date

class Date : public date::year_month_day {
public:

    // Constructors
    explicit Date(const std::string& s="1970-01-01");
    Date(const date::sys_days& d);

    // I/O
    friend std::ostream& operator<<(
            std::ostream& oss, 
            const date::year_month_day& d);

};

// Construct a date from a string "YYYY-MM-DD"
Date::Date(const std::string& s) {
    std::istringstream iss{s};
    iss >> date::parse("%F", *this);
}

// Construct a date from the date::sys_days type
Date::Date(const date::sys_days& d) : date::year_month_day{d} {}

// Print a date
std::ostream& operator<<(std::ostream& oss, const date::year_month_day& d) 
{ return date::operator<<(oss, d); }

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Time

class Time : public std::chrono::seconds {
public:

    // Constructors
    explicit Time(const std::string& s="00:00:00");
    explicit Time(long n);
    Time(const std::chrono::seconds& s);

    // I/O
    friend std::ostream& operator<<(
            std::ostream& oss, 
            const std::chrono::seconds& t);
};

// Construct a time from a string "HH:MM:SS"
Time::Time(const std::string& s) {
    std::istringstream iss{s};
    iss >> date::parse("%T", *this);
}

// Construct a time from a long representing number of seconds
Time::Time(long n) : std::chrono::seconds{n} {}

// Construct a time from the std::chrono::seconds type
Time::Time(const std::chrono::seconds& s) : std::chrono::seconds{s} {}

// Print a time
std::ostream& operator<<(std::ostream& oss, const std::chrono::seconds& t) 
{ return oss << date::make_time(t); }

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// DateTime

class DateTime : public date::sys_seconds {
public:

    // Constructors
    explicit DateTime(const std::string& s="1970-01-01 00:00:00");
    explicit DateTime(const date::sys_days& d);
    explicit DateTime(const date::sys_seconds& s);
    explicit DateTime(long s);
    DateTime(const Date& d, const Time& t);

    // Queries
    Date date() const;
    Time time() const;

    long count() const;

    DateTime firstTime() const;
    DateTime lastTime() const;

    // I/O
    friend std::ostream& operator<<(std::ostream& oss, const DateTime& dt);

};

// Construct a datetime from a string "YYYY-MM-DD HH:MM:SS"
DateTime::DateTime(const std::string& s) 
: DateTime{Date{s.substr(0,10)}, Time{s.substr(11,8)}} 
{}

// Construct a datetime from the date::sys_days type
DateTime::DateTime(const date::sys_days& d) : date::sys_seconds{d} {}

// Construct a datetime from the date::sys_seconds type
DateTime::DateTime(const date::sys_seconds& s) : date::sys_seconds{s} {}

// Construct a datetime from the number of seconds since epoch
DateTime::DateTime(long s) : date::sys_seconds(std::chrono::seconds{s}) {}

// Construct a datetime from the given date and time
DateTime::DateTime(const Date& d, const Time& t) 
: date::sys_seconds{date::sys_days{d}+t} 
{}

// Return the date corresponding to the datetime
Date DateTime::date() const { return Date{date::floor<date::days>(*this)}; }

// Return the time corresponding to the datetime
Time DateTime::time() const 
{ return Time{*this-date::floor<date::days>(*this)}; }

// Return the time since epoch; used to generate a unique number
long DateTime::count() const { return this->time_since_epoch().count(); }

// Return the date with the time 00:00:00
DateTime DateTime::firstTime() const 
{ return DateTime{date(), Time{0}}; }

// Return the date with the time 23:59:59
DateTime DateTime::lastTime() const 
{ return DateTime{date(), Time{24*60*60-1}}; }

// Print a datetime
std::ostream& operator<<(std::ostream& oss, const DateTime& dt) 
{ return date::operator<<(oss, dt); }

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// TimePeriod 

class TimePeriod {
public:
    
    // Constructors
    TimePeriod();
    TimePeriod(const DateTime& s, const DateTime& e);

    // Queries
    const DateTime& start() const;
    const DateTime& end() const;

    explicit operator bool() const;

    // I/O
    friend std::ostream& operator<<(std::ostream& oss, const TimePeriod& tp);

private: 

    DateTime s, e;

};

// Default Constructor
TimePeriod::TimePeriod() {}

// Construct a time period from the given datetimes
TimePeriod::TimePeriod(const DateTime& s, const DateTime& e) : s{s}, e{e} {}

// Return the start datetime of the time period
const DateTime& TimePeriod::start() const { return s; }

// Return the end datetime of the time period
const DateTime& TimePeriod::end()   const { return e; }

// Return whether the time period is valid
TimePeriod::operator bool() const { return !(s == e); }

// Print a time period
std::ostream& operator<<(std::ostream& oss, const TimePeriod& tp) 
{ return oss << "(" << tp.s << ", " << tp.e << ")"; }

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Typedefs, and operations

// A list of dates
typedef std::vector<Date> DateList;

// A list of times
typedef std::vector<Time> TimeList;

// A list of datetimes
typedef std::vector<DateTime> DateTimeList;

#endif // UTILS_DATEUTILS_HPP
