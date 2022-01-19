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

    explicit Date(const std::string& s="1970-01-01");
    Date(const date::sys_days& d);

    friend std::ostream& operator<<(
            std::ostream& oss, 
            const date::year_month_day& d);

};

Date::Date(const std::string& s) {
    std::istringstream iss{s};
    iss >> date::parse("%F", *this);
}

Date::Date(const date::sys_days& d) : date::year_month_day{d} {}

std::ostream& operator<<(std::ostream& oss, const date::year_month_day& d) 
{ return date::operator<<(oss, d); }

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Time

class Time : public std::chrono::seconds {
public:

    explicit Time(const std::string& s="00:00:00");
    explicit Time(long n);
    Time(const std::chrono::seconds& s);

    friend std::ostream& operator<<(
            std::ostream& oss, 
            const std::chrono::seconds& t);
};

Time::Time(const std::string& s) {
    std::istringstream iss{s};
    iss >> date::parse("%T", *this);
}

Time::Time(long n) : std::chrono::seconds{n} {}
Time::Time(const std::chrono::seconds& s) : std::chrono::seconds{s} {}

std::ostream& operator<<(std::ostream& oss, const std::chrono::seconds& t) 
{ return oss << date::make_time(t); }

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// DateTime

class DateTime : public date::sys_seconds {
public:

    explicit DateTime(const std::string& s="1970-01-01 00:00:00");
    explicit DateTime(const date::sys_days& d);
    explicit DateTime(const date::sys_seconds& s);
    DateTime(const Date& d, const Time& t);

    Date date() const;
    Time time() const;

    long count() const;

    DateTime firstTime() const;
    DateTime lastTime() const;

    friend std::ostream& operator<<(std::ostream& oss, const DateTime& dt);

};

DateTime::DateTime(const std::string& s) 
: DateTime{Date{s.substr(0,10)}, Time{s.substr(11,8)}} 
{}

DateTime::DateTime(const date::sys_days& d) : date::sys_seconds{d} {}

DateTime::DateTime(const date::sys_seconds& s) : date::sys_seconds{s} {}

DateTime::DateTime(const Date& d, const Time& t) 
: date::sys_seconds{date::sys_days{d}+t} 
{}

Date DateTime::date() const { return Date{date::floor<date::days>(*this)}; }

Time DateTime::time() const 
{ return Time{*this-date::floor<date::days>(*this)}; }

long DateTime::count() const { return this->time_since_epoch().count(); }

DateTime DateTime::firstTime() const 
{ return DateTime{date(), Time{0}}; }

DateTime DateTime::lastTime() const 
{ return DateTime{date(), Time{24*60*60-1}}; }

std::ostream& operator<<(std::ostream& oss, const DateTime& dt) 
{ return date::operator<<(oss, dt); }

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// TimePeriod 

class TimePeriod {
public:
    
    TimePeriod();
    TimePeriod(const DateTime& s, const DateTime& e);

    const DateTime& start() const;
    const DateTime& end() const;

    explicit operator bool() const;

    friend std::ostream& operator<<(std::ostream& oss, const TimePeriod& tp);

private: 

    DateTime s, e;

};

TimePeriod::TimePeriod() {}

TimePeriod::TimePeriod(const DateTime& s, const DateTime& e) : s{s}, e{e} {}

const DateTime& TimePeriod::start() const { return s; }
const DateTime& TimePeriod::end()   const { return e; }

TimePeriod::operator bool() const { return !(s == e); }

std::ostream& operator<<(std::ostream& oss, const TimePeriod& tp) 
{ return oss << "(" << tp.s << ", " << tp.e << ")"; }

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Typedefs, and operations

typedef std::vector<Date> DateList;
typedef std::vector<Time> TimeList;
typedef std::vector<DateTime> DateTimeList;

#endif // UTILS_DATEUTILS_HPP
