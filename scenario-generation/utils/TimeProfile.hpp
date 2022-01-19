#ifndef UTILS_TIME_PROFILE_HPP
#define UTILS_TIME_PROFILE_HPP

#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <algorithm>
#include <utility>
#include <memory>

#include "../include/rapidjson/document.h"
#include "../include/date/date.h"
#include "../include/date/iso_week.h"

#include "DateUtils.hpp"
#include "IOUtils.hpp"
#include "NormalDistributions.hpp"

namespace {

    const std::string DEF_STDEV = "00:10:00";

} // end namespace

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Period Details (part of TP Pattern)

class PD {
public: 
    PD();
    PD(const rj::Value& det, const Date& s, const Date& e);
    
    const DateList& dates() const;

    std::ostream& print(std::ostream& oss);

protected: 
    
    int rep=1;
    Date s, e;
    DateList dl;

};

class DayPD : public PD {
public:
    DayPD(const rj::Value& det, const Date& s, const Date& e);
};

class WeekPD : public PD {
public:
    WeekPD(const rj::Value& det, const Date& s, const Date& e);
};

class MonthPD_Day : public PD {
public:
    MonthPD_Day(const rj::Value& det, const Date& s, const Date& e);
};

class MonthPD_Week : public PD {
public:
    MonthPD_Week(const rj::Value& det, const Date& s, const Date& e);
};

class YearPD_Day : public PD {
public:
    YearPD_Day(const rj::Value& det, const Date& s, const Date& e);
};

class YearPD_Week : public PD {
public:
    YearPD_Week(const rj::Value& det, const Date& s, const Date& e);
};

class YearPD_MonthDay : public PD {
public:
    YearPD_MonthDay(const rj::Value& det, const Date& s, const Date& e);
};

class YearPD_MonthWeek : public PD {
public:
    YearPD_MonthWeek(const rj::Value& det, const Date& s, const Date& e);
};

class TPPattern {
public:

    enum Period {DAY, WEEK, MONTH, YEAR};

    TPPattern();
    explicit TPPattern(const rj::Value& pat);

    const DateList& dates() const;

    friend std::ostream& operator<<(std::ostream& oss, const TPPattern& p);

private: 

    Date s, e;
    Period period;
    std::shared_ptr<PD> det;

};

class TimeProfile {
public:
    
    explicit TimeProfile(const rj::Value& profileList);

    struct TimeProfileEntry {
        TPPattern pat;
        NormalTime start, end;
        NormalTime req;
        bool recur;
    };

    TimePeriod query(const DateTime& eta, bool useETA=true);

    friend std::ostream& operator<<(std::ostream& oss, const TimeProfile& tp);

private:

    std::vector<TimeProfileEntry> tp;

};

// Useful Typedefs
using TimeProfileList = std::vector<TimeProfile>;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Implementations for Period Details

PD::PD() {}

PD::PD(const rj::Value& det, const Date& s, const Date& e) 
    : s{s}, e{e} {
    auto it = det.FindMember("repeat-every");
    if (it != det.MemberEnd())
        rep = it->value.GetInt();
}

const DateList& PD::dates() const { return dl; }

std::ostream& PD::print(std::ostream& oss) {
    oss << "[";
    if (!dl.empty()) {
        DateList::const_iterator it = dl.begin();
        oss << *it++;
        for (; it != dl.end(); ++it)
            oss << ", " << *it;
    }
    oss << "]";
    return oss;
}

DayPD::DayPD(const rj::Value& det, const Date& s, const Date& e)
    : PD{det, s, e} {
    date::days dInc{rep};
    for (date::sys_days curr = s; curr <= e; curr += dInc)
        dl.push_back(Date{curr});
}

WeekPD::WeekPD(const rj::Value& det, const Date& s, const Date& e)
    : PD{det, s, e} {
    date::days wInc{7 * rep};
    int startDay = date::weekday(s).c_encoding();
    for (const rj::Value& v : det["days-of-week"].GetArray()) {
        date::days diff{(v.GetInt() - startDay + 7) % 7};
        for (date::sys_days curr = date::sys_days{s}+diff; 
                curr <= e; 
                curr += wInc)
            dl.push_back(Date{curr});
    }
}

MonthPD_Day::MonthPD_Day(const rj::Value& det, const Date& s, const Date& e)
    : PD{det, s, e} {
    date::months mInc{rep};
    for (const rj::Value& v : det["days-of-month"].GetArray()) {
        for (Date curr = s; curr <= e; curr += mInc) {
            Date next{curr.year()/curr.month()/v.GetInt()};
            if (s <= next && next <= e && next.ok())
                dl.push_back(next);
        }
    }
}

MonthPD_Week::MonthPD_Week(const rj::Value& det, const Date& s, const Date& e)
    : PD{det, s, e} {
    date::months mInc{rep};
    for (const rj::Value& w : det["weeks-of-month"].GetArray()) {
        for (const rj::Value& d : det["days-of-week"].GetArray()) {
            for (Date curr = s; curr <= e; curr += mInc) {
                Date next{curr.year()/
                        curr.month()/
                        date::weekday(d.GetInt())[w.GetInt()]};
                if (s <= next && next <= e && next.ok())
                    dl.push_back(next);
            }
        }
    }
}

YearPD_Day::YearPD_Day(const rj::Value& det, const Date& s, const Date& e)
    : PD{det, s, e} {
    date::years yInc{rep};
    for (const rj::Value& v : det["days-of-year"].GetArray()) {
        for (Date curr = s; curr <= e; curr += yInc) {
            Date next{date::sys_days{curr.year()/curr.month()/0} + 
                      date::days{v.GetInt()}};
            if (s <= next && next <= e)
                dl.push_back(next);
        }
    }
}

YearPD_Week::YearPD_Week(const rj::Value& det, const Date& s, const Date& e)
    : PD{det, s, e} {
    date::years yInc{rep};
    for (const rj::Value& w : det["weeks-of-year"].GetArray()) {
        for (const rj::Value& d : det["days-of-week"].GetArray()) {
            for (Date curr = s; curr <= e; curr += yInc) {
                iso_week::year_weeknum_weekday yww =
                        iso_week::year_weeknum_weekday{curr}.year()/
                        w.GetInt()/
                        d.GetInt();
                Date next{yww};
                if (s <= next && next <= e)
                    dl.push_back(next);
            }
        }
    }
}

YearPD_MonthDay::YearPD_MonthDay(
        const rj::Value& det, 
        const Date& s, 
        const Date& e)
    : PD{det, s, e} {
    date::years yInc{rep};
    for (const rj::Value& m : det["months-of-year"].GetArray()) {
        for (const rj::Value& d : det["days-of-month"].GetArray()) {
            for (Date curr = s; curr <= e; curr += yInc) {
                Date next{curr.year()/m.GetInt()/d.GetInt()};
                if (s <= next && next <= e)
                    dl.push_back(next);
            }
        }
    }
}

YearPD_MonthWeek::YearPD_MonthWeek(
        const rj::Value& det, 
        const Date& s, 
        const Date& e)
    : PD{det, s, e} {
    date::years yInc{rep};
    for (const rj::Value& m : det["months-of-year"].GetArray()) {
        for (const rj::Value& w : det["weeks-of-month"].GetArray()) {
            for (const rj::Value& d : det["days-of-week"].GetArray()) {
                for (Date curr = s; curr <= e; curr += yInc) {
                    Date next{curr.year()/
                            m.GetInt()/
                            date::weekday(d.GetInt())[w.GetInt()]};
                    if (s <= next && next <= e && next.ok())
                        dl.push_back(next);
                }
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// TP Pattern  

TPPattern::TPPattern() {}

TPPattern::TPPattern(const rj::Value& pat) 
    : s{pat["start-date"].GetString()}, e{pat["end-date"].GetString()} {
    const rj::Value& pd = pat["period-details"];

    switch (pat["period"].GetString()[0]) {
        case 'd': case 'D': // Day
            period = TPPattern::Period::DAY;
            det = std::shared_ptr<PD>{new DayPD{pd, s, e}};
            break;
        case 'w': case 'W': // Week
            period = TPPattern::Period::WEEK;
            det = std::shared_ptr<PD>{new WeekPD{pd, s, e}};
            break;
        case 'm': case 'M': // Month
            period = TPPattern::Period::MONTH;
            if (pd.HasMember("days-of-month"))
                det = std::shared_ptr<PD>{new MonthPD_Day{pd, s, e}};
            else
                det = std::shared_ptr<PD>{new MonthPD_Week{pd, s, e}};
            break;
        case 'y': case 'Y': // Year
            period = TPPattern::Period::YEAR;
            if (pd.HasMember("days-of-year"))
                det = std::shared_ptr<PD>{new YearPD_Day{pd, s, e}};
            else if (pd.HasMember("weeks-of-year")) // members: days-of-week
                det = std::shared_ptr<PD>{new YearPD_Week{pd, s, e}};
            else if (pd.HasMember("days-of-month")) // members: months-of-year
                det = std::shared_ptr<PD>{new YearPD_MonthDay{pd, s, e}};
            else // members: months-of-year, weeks-of-month, days-of-week
                det = std::shared_ptr<PD>{new YearPD_MonthWeek{pd, s, e}};
            break;
        default:
            std::cerr << "TPPattern: Period " << pat["period"].GetString() 
                      << " not recognized" << std::endl;
            std::exit(1);
    }

}

const DateList& TPPattern::dates() const { return det->dates(); }

std::ostream& operator<<(std::ostream& oss, const TPPattern& p) 
{ return p.det->print(oss); }


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// TimeProfile

TimeProfile::TimeProfile(const rj::Value& profileList) {
    tp.reserve(profileList.Size());
    for (const rj::Value& v : profileList.GetArray()) {
        TimeProfileEntry e;

        e.pat = TPPattern{v["pattern"]};
        if (v["duration"]["start-time"].IsArray()) {
            e.start = NormalTime{
                    v["duration"]["start-time"][0].GetString(),
                    v["duration"]["start-time"][1].GetString()
            };
        }
        else {
            e.start = NormalTime{
                    v["duration"]["start-time"].GetString(),
                    v["duration"]["start-time-stdev"].GetString()
            };
        }

        if (v["duration"]["end-time"].IsArray()) {
            e.end = NormalTime{
                    v["duration"]["end-time"][0].GetString(),
                    v["duration"]["end-time"][1].GetString()
            };
        }
        else {
            e.end = NormalTime{
                    v["duration"]["end-time"].GetString(),
                    v["duration"]["end-time-stdev"].GetString()
            };
        }

        if (v["duration"].HasMember("required")) {
            if (v["duration"]["required"].IsArray()) {
                e.req = NormalTime{
                        v["duration"]["required"][0].GetString(),
                        v["duration"]["required"][1].GetString()
                };
            }
            else {
                e.req = NormalTime{
                        v["duration"]["required"].GetString(),
                        v["duration"]["required-stdev"].GetString(),
                };
            }
        } 
        else {
            // If required is not specified, require for difference in mean
            // times, with the default stdev 
            e.req = NormalTime(Time{e.end.mean()-e.start.mean()}, DEF_STDEV);
        }

        e.recur = parseInt(v["duration"], "recurring", 0);

        tp.push_back(e);
    }
}

TimePeriod TimeProfile::query(const DateTime& eta, bool useETA) {
    const Date& d = eta.date();
    const Time& t = eta.time();
    for (TimeProfileEntry& e : tp) {
        const DateList& dl = e.pat.dates();
        if (std::find(dl.begin(), dl.end(), d) != dl.end()) {
            Time start = e.start.sample();
            if (useETA &&                     // consider the current time
                t + e.start.stdev() < start)  // current time is too early
                continue;
            start = std::max(start, t);

            Time end = e.end.sample();
            if (start > end)                  // end time before start time
                continue;

            Time req{start + e.req.sample()}; 
            if (req > end)                    // cannot attend required time
                continue;
            end = req;

            return TimePeriod{DateTime{d, start}, DateTime{d, end}};
        }
    }
    return TimePeriod{}; // Nothing found.
}

std::ostream& operator<<(std::ostream& oss, const TimeProfile& tp) {
    oss << "TimeProfile(";
    if (!tp.tp.empty()) {
        auto it = tp.tp.begin();
        oss << "Profile(" 
            << "pattern=" << it->pat << ", "
            << "duration=[" << it->start << ", " << it->end << "])";
        ++it;
        for (; it != tp.tp.end(); ++it) {
            oss << ", Profile("
                << "pattern=" << it->pat << ", "
                << "duration=[" << it->start << ", " << it->end << "])";
        }
    }
    oss << ")";
    return oss;
}

#endif // UTILS_TIME_PROFILE_HPP
