#ifndef UTILS_EVENT_LOGISTICS_HPP
#define UTILS_EVENT_LOGISTICS_HPP

#include <iostream>

#include "../model/Trajectory.hpp"

#include "Typedefs.hpp"
#include "DateUtils.hpp"

class EventLogistics {
public:
    
    // Constructors
    EventLogistics();
    EventLogistics(EventID eid, SpaceID sid, Trajectory traj, TimePeriod tp);

    // Queries
    explicit operator bool() const;

    bool operator<(const EventLogistics& other) const;

    // I/O
    friend std::ostream& operator<<(
            std::ostream& oss, 
            const EventLogistics& el);

    // Attributes
    EventID eid;
    SpaceID sid;
    Trajectory traj;
    TimePeriod tp;

    // Bookkeeping information
    MetaEventID meid;

};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Constructors

// Default Constructor
EventLogistics::EventLogistics() {}

// Constructs event logistics from the given arguments
EventLogistics::EventLogistics(
        EventID eid, 
        SpaceID sid, 
        Trajectory traj, 
        TimePeriod tp)
: eid{eid}, sid{sid}, traj{traj}, tp{tp} 
{}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Queries

// Returns true if the event logistics are valid
EventLogistics::operator bool() const { return static_cast<bool>(tp); }

// Returns an ordering for event logistics
bool EventLogistics::operator<(const EventLogistics& other) const 
{ return eid < other.eid || eid == other.eid && sid < other.sid; }

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// I/O

// Print an event logistics entry
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
