#ifndef MODEL_SPACEPERSONCONSTRAINT_HPP
#define MODEL_SPACEPERSONCONSTRAINT_HPP

#include <iostream>

#include "../utils/Typedefs.hpp"
#include "../utils/IOUtils.hpp"
#include "../utils/TimeProfile.hpp"

class SpacePersonConstraint {
public:

    // Attributes
    bool whichPerson; // true = pid, false = mpid
    PersonID pid;
    MetaPersonID mpid;

    SpaceID cid;

    bool whichEvent; // true = requiredEvents, false = requiredMetaEvents
    EventIDList requiredEventIDs;
    MetaEventRequirements requiredMetaEventIDs;

    bool isActiveTP = false; // true = active TP, false = inactive TP
    TimeProfile tp;

    // Modifiers
    void setPersonID(PersonID pid);
    void setMetaPersonID(MetaPersonID mpid);
    void setRequiredEvents(EventIDList req);
    void setRequiredMetaEvents(MetaEventRequirements req);
    void setTimeProfile(TimeProfile tp);

    // I/O
    friend std::ostream& operator<<(std::ostream& oss, 
                                    const SpacePersonConstraint& cpc);
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Modifiers

// Set the person id (and flag)
void SpacePersonConstraint::setPersonID(PersonID pid) {
    whichPerson = true;
    this->pid = pid;
}

// Set the metaperson id (and flag)
void SpacePersonConstraint::setMetaPersonID(MetaPersonID mpid) {
    whichPerson = false;
    this->mpid = mpid;
}

// Set the events list (and flag)
void SpacePersonConstraint::setRequiredEvents(EventIDList req) {
    whichEvent = true;
    this->requiredEventIDs = req;
}

// Set the metaevents list (and flag)
void SpacePersonConstraint::setRequiredMetaEvents(MetaEventRequirements req) {
    whichEvent = false;
    this->requiredMetaEventIDs = req;
}

// Set the time profile (and flag)
void SpacePersonConstraint::setTimeProfile(TimeProfile tp) {
    isActiveTP = true;
    this->tp = tp;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// I/O

// Print a space-person constraint
std::ostream& operator<<(std::ostream& oss, const SpacePersonConstraint& cpc) {
    oss << "SpacePersonConstraint(";

    if (cpc.whichPerson)
        oss << "pid=" << cpc.pid << ", ";
    else
        oss << "mpid=" << cpc.mpid << ", ";

    oss << "cid=" << cpc.cid;

    if (cpc.whichEvent)
        oss << ", eids=" << cpc.requiredEventIDs;
    else
        oss << ", meids=" << cpc.requiredMetaEventIDs;

    if (cpc.isActiveTP)
        oss << ", tp=" << cpc.tp;

    oss << ")";
    return oss;
}

#endif // MODEL_SPACEPERSONCONSTRAINT_HPP
