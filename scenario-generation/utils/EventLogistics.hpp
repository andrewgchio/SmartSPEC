#ifndef UTILS_EVENT_LOGISTICS_HPP
#define UTILS_EVENT_LOGISTICS_HPP

#include <iostream>

#include "../model/Trajectory.hpp"

#include "Typedefs.hpp"
#include "DateUtils.hpp"

class EventLogistics {
public:
    
    EventLogistics();
    EventLogistics(EventID eid, SpaceID sid, Trajectory traj, TimePeriod tp);

    explicit operator bool() const;

    bool operator<(const EventLogistics& other) const;

    friend std::ostream& operator<<(
            std::ostream& oss, 
            const EventLogistics& el);

    EventID eid;
    SpaceID sid;
    Trajectory traj;
    TimePeriod tp;

};

EventLogistics::EventLogistics() {}

EventLogistics::EventLogistics(
        EventID eid, 
        SpaceID sid, 
        Trajectory traj, 
        TimePeriod tp)
: eid{eid}, sid{sid}, traj{traj}, tp{tp} 
{}

EventLogistics::operator bool() const { return static_cast<bool>(tp); }

bool EventLogistics::operator<(const EventLogistics& other) const 
{ return eid < other.eid || eid == other.eid && sid < other.sid; }

std::ostream& operator<<(std::ostream& oss, const EventLogistics& el) {
    oss << "EventLogistics("
        << "eid=" << el.eid << ", "
        << "sid=" << el.sid << ", "
        << "traj=" << el.traj << ", "
        << "tp=" << el.tp
        << ")";
    return oss;
}

#endif // UTILS_EVENT_LOGISTICS_HPP
