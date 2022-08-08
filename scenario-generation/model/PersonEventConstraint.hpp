#ifndef MODEL_PERSONEVENTCONSTRAINT_HPP
#define MODEL_PERSONEVENTCONSTRAINT_HPP

#include <iostream>

#include "../utils/Typedefs.hpp"
#include "../utils/IOUtils.hpp"
#include "../utils/TimeProfile.hpp"

class PersonEventConstraint {
public:

    // Attributes
    bool whichPerson; // true = pid, false = mpid
    PersonID pid;
    MetaPersonID mpid;

    bool whichEvent; // true = eid, false = meid
    EventID eid;
    MetaEventID meid;

    bool isActiveCountdown = false; 
    NormalTime countdown;

    bool isActiveCapRange = false;
    CapRange range;

    // Modifiers
    void setPersonID(PersonID pid);
    void setMetaPersonID(MetaPersonID mpid);
    void setEventID(EventID eid);
    void setMetaEventID(MetaEventID meid);
    void setCountdown(NormalTime countdown);
    void setRange(CapRange range);

    // I/O
    friend std::ostream& operator<<(std::ostream& oss, 
                                    const SpacePersonConstraint& cpc);
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Modifiers

// Set the person id (and flag)
void PersonEventConstraint::setPersonID(PersonID pid) {
    whichPerson = true;
    this->pid = pid;
}

// Set the metaperson id (and flag)
void PersonEventConstraint::setMetaPersonID(MetaPersonID mpid) {
    whichPerson = false;
    this->mpid = mpid;
}

// Set the event id (and flag)
void PersonEventConstraint::setEventID(EventID eid) {
    whichEvent = true;
    this->eid = eid;
}

// Set the metaevent id (and flag)
void PersonEventConstraint::setMetaEventID(MetaEventID meid) {
    whichEvent = false;
    this->meid = meid;
}

// Set the countdown (and flag)
void PersonEventConstraint::setCountdown(NormalTime countdown) {
    isActiveCountdown = true;
    this->countdown = countdown;
}

// Set the range (and flag)
void PersonEventConstraint::setRange(CapRange range) {
    isActiveCapRange = true;
    this->range = range;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// I/O

// Print a person-event constraint
std::ostream& operator<<(std::ostream& oss, const PersonEventConstraint& pec) {
    oss << "PersonEventConstraint(";

    if (pec.whichPerson)
        oss << "pid=" << pec.pid << ", ";
    else
        oss << "mpid=" << pec.mpid << ", ";

    if (pec.whichEvent)
        oss << ", eid=" << pec.eid;
    else
        oss << ", meid=" << pec.meid;

    if (pec.isActiveCountdown)
        oss << ", countdown=" << pec.countdown;

    if (pec.isActiveCapRange)
        oss << ", cap=" << pec.range;

    oss << ")";
    return oss;
}

#endif // MODEL_PERSONEVENTCONSTRAINT_HPP
