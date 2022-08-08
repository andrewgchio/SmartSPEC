#ifndef MODEL_SPACEEVENTCONSTRAINT_HPP
#define MODEL_SPACEEVENTCONSTRAINT_HPP

#include <iostream>

#include "../utils/Typedefs.hpp"
#include "../utils/IOUtils.hpp"
#include "../utils/TimeProfile.hpp"

class SpaceEventConstraint {
public:

    // Attributes
    bool whichEvent; // true = eid, false = meid
    EventID eid;
    MetaEventID meid;

    SpaceID cid;

    bool isActiveTP = false; // true = active TP, false = inactive TP
    TimeProfile tp;

    bool isActiveCapacity = false; // true = active cap, false = inactive cap
    CapRange cap;

    // Modifiers
    void setEventID(EventID eid);
    void setMetaEventID(MetaEventID meid);
    void setTimeProfile(TimeProfile tp);
    void setCapacity(CapRange cap);

    // I/O
    friend std::ostream& operator<<(std::ostream& oss, 
                                    const SpaceEventConstraint& cec);
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Modifiers

// Set the event id (and flag)
void SpaceEventConstraint::setEventID(EventID eid) {
    whichEvent = true;
    this->eid = eid;
}

// Set the metaevent id (and flag)
void SpaceEventConstraint::setMetaEventID(MetaEventID meid) {
    whichEvent = false;
    this->meid = meid;
}

// Set the time profile (and flag)
void SpaceEventConstraint::setTimeProfile(TimeProfile tp) {
    isActiveTP = true;
    this->tp = tp;
}

// Set the capacity range (and flag)
void SpaceEventConstraint::setCapacity(CapRange cap) {
    isActiveCapacity = true;
    this->cap = cap;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// I/O

// Print a space-event constraint
std::ostream& operator<<(std::ostream& oss, const SpaceEventConstraint& cec) {
    oss << "SpaceEventConstraint(";

    if (cec.whichEvent)
        oss << "eid=" << cec.eid << ", ";
    else
        oss << "meid=" << cec.meid << ", ";

    oss << "cid=" << cec.cid;

    if (cec.isActiveTP)
        oss << ", tp=" << cec.tp;

    if (cec.isActiveCapacity)
        oss << ", cap=" << cec.cap;

    oss << ")";
    return oss;
}

#endif // MODEL_SPACEEVENTCONSTRAINT_HPP
