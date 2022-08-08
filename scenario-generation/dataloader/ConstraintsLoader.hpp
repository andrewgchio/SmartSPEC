#ifndef DATALOADER_CONSTRAINTSLOADER_HPP
#define DATALOADER_CONSTRAINTSLOADER_HPP

#include <iostream>
#include <string>
#include <vector>
#include <utility>

#include "../include/rapidjson/document.h"

#include "../model/MetaPerson.hpp"
#include "../model/MetaEvent.hpp"
#include "../model/Person.hpp"
#include "../model/Event.hpp"
#include "../model/SpacePersonConstraint.hpp"
#include "../model/SpaceEventConstraint.hpp"
#include "../model/PersonEventConstraint.hpp"

#include "../utils/Typedefs.hpp"
#include "../utils/DateUtils.hpp"
#include "../utils/IOUtils.hpp"
#include "../utils/TimeProfile.hpp"

#include "SpacesLoader.hpp"
#include "EventsLoader.hpp"
#include "PeopleLoader.hpp"
#include "MetaEventsLoader.hpp"
#include "MetaPeopleLoader.hpp"

class ConstraintsLoader {
public:

    // Constructors
    ConstraintsLoader();
    explicit ConstraintsLoader(const Filename& fname);

    // Queries
    bool checkCPConstraints(SpaceID cid, PersonID pid, const DateTime& curr);
    bool checkCEConstraints(SpaceID cid, EventID eid, const DateTime& curr);
    bool checkPEConstraints(PersonID pid, EventID eid, const DateTime& curr);

    bool checkCP(SpaceID cid, PersonID pid, const DateTime& curr);
    bool checkCMP(SpaceID cid, PersonID mpid, const DateTime& curr);
    bool checkCE(SpaceID cid, EventID eid, const DateTime& curr);
    bool checkCME(SpaceID cid, EventID meid, const DateTime& curr);
    bool checkPE(PersonID pid, EventID eid, const DateTime& curr);
    bool checkPME(PersonID pid, EventID meid, const DateTime& curr);
    bool checkMPE(PersonID mpid, EventID eid, const DateTime& curr);
    bool checkMPME(PersonID mpid, EventID meid, const DateTime& curr);

    // Modifiers
    void addCP(SpacePersonConstraint cpc);
    void addCMP(SpacePersonConstraint cpc);
    void addCE(SpaceEventConstraint cec);
    void addCME(SpaceEventConstraint cec);
    void addPE(PersonEventConstraint pec);
    void addPME(PersonEventConstraint pec);
    void addMPE(PersonEventConstraint pec);
    void addMPME(PersonEventConstraint pec);

    void addSpaces(SpacesLoader& C) { this->C = C; }
    void addPeople(PeopleLoader& P) { this->P = P; }
    void addEvents(EventsLoader& E) { this->E = E; }
    void addMetaPeople(MetaPeopleLoader& MP) { this->MP = MP; }
    void addMetaEvents(MetaEventsLoader& ME) { this->ME = ME; }

    friend std::ostream& operator<<(std::ostream& oss, 
                                    const ConstraintsLoader& csl);

private:

    // References to relevant entities
    SpacesLoader C;
    EventsLoader E;
    PeopleLoader P;
    MetaEventsLoader ME;
    MetaPeopleLoader MP;

    // Space-Person constraints
    std::map<CPKey,   SpacePersonConstraint> cpcEntries;

    // Space-MetaPerson constraints
    std::map<CMPKey,  SpacePersonConstraint> cmpcEntries;

    // Space-Event constraints
    std::map<CEKey,   SpaceEventConstraint>  cecEntries;

    // Space-MetaEvent constraints
    std::map<CMEKey,  SpaceEventConstraint>  cmecEntries;

    // Person-Event constraints
    std::map<PEKey,   PersonEventConstraint> pecEntries;

    // Person-MetaEvent constraints
    std::map<PMEKey,  PersonEventConstraint> pmecEntries;

    // MetaPerson-Event constraints
    std::map<MPEKey,  PersonEventConstraint> mpecEntries;

    // MetaPerson-MetaEvent constraints
    std::map<MPMEKey, PersonEventConstraint> mpmecEntries;

};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Constructors

// Default Constructor
ConstraintsLoader::ConstraintsLoader() {}

// For the given constraints file, read and load each constraint
ConstraintsLoader::ConstraintsLoader(const Filename& fname) {
    if (fname == "none")
        return;

    std::cout << "... Reading Constraints file: " << fname << std::endl;
    
    // JSON reader setup
    rj::Document doc;
    openJSON(fname, doc);

    for (const rj::Value& v : doc.GetArray()) {
        // CP constraint
        if (v.HasMember("space-id") && v.HasMember("person-id")) {
            
            // Initialize the CP constraint
            SpacePersonConstraint c;

            // person-id
            c.setPersonID(parseInt(v, "person-id"));

            // space-id
            c.cid = parseInt(v, "space-id");

            // required-event-ids...
            if (v.HasMember("required-event-ids"))
                c.setRequiredEvents(parseIntArr(v, "required-event-ids"));

            // OR required-metaevent-ids
            else if (v.HasMember("required-metaevent-ids")) {
                MetaEventRequirements req;
                for (const rj::Value& x : 
                        v["required-metaevent-ids"].GetArray() ) {
                    MetaEventID meid = parseInt(x, "metaevent-id");
                    req[meid] = std::make_pair(x["range"][0].GetInt(),
                                                   x["range"][1].GetInt());
                }
                c.setRequiredMetaEvents(req);
            }

            // time-profile
            if (v.HasMember("time-profile")) c.setTimeProfile(TimeProfile{v["time-profile"]});

            // Add the constraint to the data loader
            addCP(c);
        }

        // CMP constraint
        else if (v.HasMember("space-id") && v.HasMember("metaperson-id")) {

            // Initialize the CMP constraint
            SpacePersonConstraint c;

            // metaperson-id
            c.setMetaPersonID(parseInt(v, "metaperson-id"));

            // space-id
            c.cid = parseInt(v, "space-id");

            // required-event-ids...
            if (v.HasMember("required-event-ids"))
                c.setRequiredEvents(parseIntArr(v, "required-event-ids"));

            // OR required-metaevent-ids
            else if (v.HasMember("required-metaevent-ids")) {
                MetaEventRequirements req;
                for (const rj::Value& x : 
                        v["required-metaevent-ids"].GetArray() ) {
                    MetaEventID meid = parseInt(x, "metaevent-id");
                    req[meid] = std::make_pair(x["range"][0].GetInt(),
                                                   x["range"][1].GetInt());
                }
                c.setRequiredMetaEvents(req); }

            // time-profile
            if (v.HasMember("time-profile"))
                c.setTimeProfile(TimeProfile{v["time-profile"]});

            // Add the constraint to the data loader
            addCMP(c);
        }

        // CE constraint
        else if (v.HasMember("space-id") && v.HasMember("event-id")) {

            // Initialize the CE constraint
            SpaceEventConstraint c;

            // event-id
            c.setEventID(parseInt(v, "event-id"));

            // space-id
            c.cid = parseInt(v, "space-id");

            // time-profile
            if (v.HasMember("time-profile"))
                c.setTimeProfile(TimeProfile{v["time-profile"]});

            // capacity
            if (v.HasMember("capacity")) {
                c.setCapacity(std::make_pair(v["capacity"][0].GetInt(),
                                             v["capacity"][1].GetInt()));
            }

            // Add the constraint to the data loader
            addCE(c);
        }

        // CME constraint
        else if (v.HasMember("space-id") && v.HasMember("metaevent-id")) {

            // Initialize the CME constraint
            SpaceEventConstraint c;

            // metaevent-id
            c.setMetaEventID(parseInt(v, "metaevent-id"));

            // space-id
            c.cid = parseInt(v, "space-id");

            // time-profile
            if (v.HasMember("time-profile"))
                c.setTimeProfile(TimeProfile{v["time-profile"]});

            // capacity
            if (v.HasMember("capacity")) {
                c.setCapacity(std::make_pair(v["capacity"][0].GetInt(),
                                             v["capacity"][1].GetInt()));
            }

            // Add the constraint to the data loader
            addCME(c);
        }

        // PE constraint
        else if (v.HasMember("person-id") && v.HasMember("event-id")) {
            
            // Initialize the PE constraint
            PersonEventConstraint c;

            // person-id
            c.setPersonID(parseInt(v, "person-id"));

            // event-id
            c.setEventID(parseInt(v, "event-id"));

            // range
            if (v.HasMember("range")) {
                c.setRange(std::make_pair(v["range"][0].GetInt(),
                                          v["range"][1].GetInt()));
            }
            
            // countdown
            if (v.HasMember("countdown")) {
                c.setCountdown(NormalTime{
                        v["countdown"][0].GetString(),
                        v["countdown"][1].GetString()});
            }

            // Add the constraint to the data loader
            addPE(c);
        }

        // PME constraint
        else if (v.HasMember("person-id") && v.HasMember("metaevent-id")) {

            // Initialize the PE constraint
            PersonEventConstraint c;

            // person-id
            c.setPersonID(parseInt(v, "person-id"));

            // metaevent-id
            c.setMetaEventID(parseInt(v, "metaevent-id"));

            // range
            if (v.HasMember("range")) {
                c.setRange(std::make_pair(v["range"][0].GetInt(),
                                          v["range"][1].GetInt()));
            }
            
            // countdown
            if (v.HasMember("countdown")) {
                c.setCountdown(NormalTime{
                        v["countdown"][0].GetString(),
                        v["countdown"][1].GetString()});
            }

            // Add the constraint to the data loader
            addPME(c);
        }

        // MPE constraint
        else if (v.HasMember("metaperson-id") && v.HasMember("event-id")) {

            // Initialize the PE constraint
            PersonEventConstraint c;

            // metaperson-id
            c.setMetaPersonID(parseInt(v, "metaperson-id"));

            // event-id
            c.setEventID(parseInt(v, "event-id"));

            // range
            if (v.HasMember("range")) {
                c.setRange(std::make_pair(v["range"][0].GetInt(),
                                          v["range"][1].GetInt()));
            }
            
            // countdown
            if (v.HasMember("countdown")) {
                c.setCountdown(NormalTime{
                        v["countdown"][0].GetString(),
                        v["countdown"][1].GetString()});
            }

            // Add the constraint to the data loader
            addMPE(c);
        }

        // MPME constraint
        else if (v.HasMember("metaperson-id") && v.HasMember("metaevent-id")) {

            // Initialize the PE constraint
            PersonEventConstraint c;

            // metaperson-id
            c.setMetaPersonID(parseInt(v, "metaperson-id"));

            // metaevent-id
            c.setMetaEventID(parseInt(v, "metaevent-id"));

            // range
            if (v.HasMember("range")) {
                c.setRange(std::make_pair(v["range"][0].GetInt(),
                                          v["range"][1].GetInt()));
            }
            
            // countdown
            if (v.HasMember("countdown")) {
                c.setCountdown(NormalTime{
                        v["countdown"][0].GetString(),
                        v["countdown"][1].GetString()});
            }

            // Add the constraint to the data loader
            addMPME(c);
        }

    }
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Queries

// Check all space-person constraints
bool ConstraintsLoader::checkCPConstraints(SpaceID cid, PersonID pid, 
        const DateTime& curr) 
{ return checkCP(cid,pid,curr) && checkCMP(cid,pid,curr); }

// Check all space-event constraints
bool ConstraintsLoader::checkCEConstraints(SpaceID cid, EventID eid,
        const DateTime& curr) 
{ return checkCE(cid,eid,curr) && checkCME(cid,eid,curr); }

// Check all person-event constraints
bool ConstraintsLoader::checkPEConstraints(PersonID pid, EventID eid,
        const DateTime& curr) {
    return checkPE(pid,eid,curr) && checkPME(pid,eid,curr) && 
           checkMPE(pid,eid,curr) && checkMPME(pid,eid,curr);
}

// Check space-person constraint
bool ConstraintsLoader::checkCP(SpaceID cid, PersonID pid, 
        const DateTime& curr) {

    // Get entities
    Space&  c = C[cid];
    Person& p = P[pid];

    // Find CP constraints
    CPKey key = std::make_pair(cid, pid);
    auto cit = cpcEntries.find(key);
    if (cit == cpcEntries.end()) // no cp constraints exist
        return true;

    // Get constraint 
    SpacePersonConstraint& cs = cit->second;

    // Check required events / metaevents
    if (cs.whichEvent) { // required-event-ids
        for (EventID x : cs.requiredEventIDs) {
            if (p.attendedEventIDs.find(x) == p.attendedEventIDs.end())
                return false;
        }
    } 
    else { // required-metaevent-ids
        for (const auto& x : cs.requiredMetaEventIDs) {
            int count = p.attendedMetaEventIDs[x.first];
            if ((x.second.first > count && x.second.first != -1) || 
                (count > x.second.second && x.second.second != -1))
                return false;
        }
    }

    // Check time profile
    if (cs.isActiveTP) {
        TimePeriod period = cs.tp.query(curr, false);
        if (!period)
            return false;
    }

    return true;
}

// Check space-metaperson constraint
bool ConstraintsLoader::checkCMP(SpaceID cid, PersonID pid,
        const DateTime& curr) {

    // Get entities
    Space&  c = C[cid];
    Person& p = P[pid];

    // Find CMP constraints
    CMPKey key = std::make_pair(cid, p.mid);
    auto cit = cmpcEntries.find(key);
    if (cit == cmpcEntries.end()) // no cmp constraints exist
        return true;

    // Get constraint 
    SpacePersonConstraint& cs = cit->second;

    // Check required events / metaevents
    if (cs.whichEvent) { // required-event-ids
        for (EventID x : cs.requiredEventIDs) {
            if (p.attendedEventIDs.find(x) == p.attendedEventIDs.end())
                return false;
        }
    } 
    else { // required-metaevent-ids
        for (const auto& x : cs.requiredMetaEventIDs) {
            int count = p.attendedMetaEventIDs[x.first];
            if ((x.second.first > count && x.second.first != -1) || 
                (count > x.second.second && x.second.second != -1))
                return false;
        }
    }

    // Check time profile
    if (cs.isActiveTP) {
        TimePeriod period = cs.tp.query(curr, false);
        if (!period)
            return false;
    }

    return true;
}

// Check space-event constraint
bool ConstraintsLoader::checkCE(SpaceID cid, EventID eid,
        const DateTime& curr) {

    // Get entities
    Space& c = C[cid];
    Event& e = E[eid];

    // Find CE constraints
    CEKey key = std::make_pair(cid, eid);
    auto cit = cecEntries.find(key);
    if (cit == cecEntries.end()) // no constraints exist
        return true;

    // Get constraint
    SpaceEventConstraint& cs = cit->second;

    // Check time profile
    if (cs.isActiveTP) {
        TimePeriod period = cs.tp.query(curr, false);
        if (!period)
            return false;
    }

    // Check capacity
    if (cs.isActiveCapacity) {
        int tcap = e.totalCapacity();
        if ((cs.cap.first > tcap && cs.cap.first != -1) || 
            (tcap > cs.cap.second && cs.cap.second != -1))
            return false;
    }

    return true;
}

// Check space-metaevent constraint
bool ConstraintsLoader::checkCME(SpaceID cid, EventID eid,
        const DateTime& curr) {

    // Get entities
    Space& c = C[cid];
    Event& e = E[eid];
    
    // Find CME constraints
    CMEKey key = std::make_pair(cid, e.mid);
    auto cit = cmecEntries.find(key);
    if (cit == cmecEntries.end()) // no cme constraints exist
        return true;

    // Get constraint
    SpaceEventConstraint& cs = cit->second;

    // Check time profile
    if (cs.isActiveTP) {
        TimePeriod period = cs.tp.query(curr, false);
        if (!period)
            return false;
    }

    // Check capacity
    if (cs.isActiveCapacity) {
        int tcap = e.totalCapacity();
        if ((cs.cap.first > tcap && cs.cap.first != -1) || 
            (tcap > cs.cap.second && cs.cap.second != -1))
            return false;
    }

    return true;
}

// Check person-event constraint
bool ConstraintsLoader::checkPE(PersonID pid, EventID eid,
        const DateTime& curr) {
    
    // Get entities
    Person& p = P[pid];
    Event&  e = E[eid];

    // Find PE constraints
    PEKey key = std::make_pair(pid, eid);
    auto cit = pecEntries.find(key);
    if (cit == pecEntries.end()) // no pe constraints exist
        return true;

    // Get constraint
    PersonEventConstraint& cs = cit->second;

    // Check countdown
    // TODO
    // Check range
    // TODO
    return true;
}

// Check person-metaevent constraint
bool ConstraintsLoader::checkPME(PersonID pid, EventID eid,
        const DateTime& curr) {

    // Get entities
    Person& p = P[pid];
    Event&  e = E[eid];

    // Find PME constraints
    PMEKey key = std::make_pair(pid, e.mid);
    auto cit = pmecEntries.find(key);
    if (cit == pmecEntries.end()) // no pme constraints exist
        return true;

    // Get constraint 
    PersonEventConstraint& cs = cit->second;

    // Check countdown
    // TODO
    // Check range
    // TODO
    return true;
}

// Check metaperson-event constraint
bool ConstraintsLoader::checkMPE(PersonID pid, EventID eid,
        const DateTime& curr) {

    // Get entities
    Person& p = P[pid];
    Event&  e = E[eid];

    // Find MPE constraints
    MPEKey key = std::make_pair(p.mid, eid);
    auto cit = mpecEntries.find(key);
    if (cit == mpecEntries.end()) // no mpe constraints exist
        return true;

    // Get constraint 
    PersonEventConstraint& c = cit->second;

    // Check countdown
    // TODO
    // Check range
    // TODO
    return true;
}

// Check metaperson-metaevent constraint
bool ConstraintsLoader::checkMPME(PersonID pid, EventID eid,
        const DateTime& curr) {

    // Get entities
    Person& p = P[pid];
    Event&  e = E[eid];

    // Find MPME constraints
    MPMEKey key = std::make_pair(p.mid, e.mid);
    auto cit = mpmecEntries.find(key);
    if (cit == mpmecEntries.end()) // no mpme constraints exist
        return true;

    // Get constraint
    PersonEventConstraint& c = cit->second;

    // Check countdown
    // TODO
    // Check range
    // TODO
    return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Modifiers

// Add a space-person constraint
void ConstraintsLoader::addCP(SpacePersonConstraint cpc) 
{ cpcEntries[std::make_pair(cpc.cid, cpc.pid)] = cpc; }

// Add a space-metaperson constraint
void ConstraintsLoader::addCMP(SpacePersonConstraint cpc) 
{ cmpcEntries[std::make_pair(cpc.cid, cpc.mpid)] = cpc; }

// Add a space-event constraint
void ConstraintsLoader::addCE(SpaceEventConstraint cec) 
{ cecEntries[std::make_pair(cec.cid, cec.eid)] = cec; }

// Add a space-metaevent constraint
void ConstraintsLoader::addCME(SpaceEventConstraint cec) 
{ cmecEntries[std::make_pair(cec.cid, cec.meid)] = cec; }

// Add a person-event constraint
void ConstraintsLoader::addPE(PersonEventConstraint pec) 
{ pecEntries[std::make_pair(pec.pid, pec.eid)] = pec; }

// Add a person-metaevent constraint
void ConstraintsLoader::addPME(PersonEventConstraint pec) 
{ pmecEntries[std::make_pair(pec.pid, pec.meid)] = pec; }

// Add a metaperson-event constraint
void ConstraintsLoader::addMPE(PersonEventConstraint pec) 
{ mpecEntries[std::make_pair(pec.mpid, pec.eid)] = pec; }

// add a metaperson-metaevent constraint
void ConstraintsLoader::addMPME(PersonEventConstraint pec) 
{ mpmecEntries[std::make_pair(pec.mpid, pec.meid)] = pec; }

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// I/O

// Print all constraints
std::ostream& operator<<(std::ostream& oss, const ConstraintsLoader& csl) {
    oss << "Space-Person Constraints:" << std::endl;
    for (const auto& e : csl.cpcEntries)
        oss << "  " << e.second << std::endl;
    for (const auto& e : csl.cmpcEntries)
        oss << "  " << e.second << std::endl;

    oss << "Space-Event Constraints:" << std::endl;
    for (const auto& e : csl.cecEntries)
        oss << "  " << e.second << std::endl;
    for (const auto& e : csl.cmecEntries)
        oss << "  " << e.second << std::endl;

    oss << "Person-Event Constraints:" << std::endl;
    for (const auto& e : csl.pecEntries)
        oss << "  " << e.second << std::endl;
    for (const auto& e : csl.pmecEntries)
        oss << "  " << e.second << std::endl;
    for (const auto& e : csl.mpecEntries)
        oss << "  " << e.second << std::endl;
    for (const auto& e : csl.mpmecEntries)
        oss << "  " << e.second << std::endl;

    return oss;
}

#endif // DATALOADER_CONSTRAINTSLOADER_HPP
