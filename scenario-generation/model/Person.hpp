#ifndef MODEL_PERSON_HPP
#define MODEL_PERSON_HPP

#include <iostream>
#include <map>
#include <set>

#include "../include/rapidjson/document.h"

#include "../utils/Typedefs.hpp"
#include "../utils/EventLogistics.hpp"

class Person {
public:

    // Attributes
    PersonID id;
    MetaPersonID mid;
    Description desc;
    Index tp;

    // Queries
    SpaceID getCurrentSpace() const;
    const std::set<EventLogistics>& getAttendedEvents() const;

    // Modifiers
    void setCurrentSpace(SpaceID cid);
    void addAttendedEvent(EventLogistics el);

    // I/O
    rj::Value dump(rj::Document::AllocatorType& alloc) const;

    friend std::ostream& operator<<(std::ostream& oss, const Person& p);

    // Synthetic Data Generation
    SpaceID currSpace;
    std::set<EventLogistics> attended;

    // Constraints
    std::set<EventID> attendedEventIDs;
    std::map<MetaEventID, int> attendedMetaEventIDs;

};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Queries

// Return the current space that the person is in 
SpaceID Person::getCurrentSpace() const { return currSpace; }

// Return the set of attended events
const std::set<EventLogistics>& Person::getAttendedEvents() const 
{ return attended; }

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Modifiers

// Set the current space that the person is in
void Person::setCurrentSpace(SpaceID cid) { currSpace = cid; }

// Adds the given attended event to this person's list of attended events
void Person::addAttendedEvent(EventLogistics el) { 
    attended.insert(el); 
    attendedEventIDs.insert(el.eid);
    std::cout << "Person::addAttendedEvent try adding 1 to map" << std::endl;
    attendedMetaEventIDs[el.meid] += 1;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// I/O

// Save the person in the given allocator
rj::Value Person::dump(rj::Document::AllocatorType& alloc) const {
    rj::Value v;
    v.SetObject();
    v.AddMember("id", rj::Value(id).Move(), alloc);
    v.AddMember("metaperson-id", rj::Value(mid).Move(), alloc);
    v.AddMember("description", rj::Value(desc.c_str(), alloc), alloc);
    v.AddMember("profile-index", rj::Value(tp).Move(), alloc);
    return v;
}

// Print a person
std::ostream& operator<<(std::ostream& oss, const Person& p) {
    oss << "Person("
        << "id=" << p.id << ", "
        << "maid=" << p.mid << ", "
        << "desc=\"" << p.desc << "\", "
        << "tp=" << p.tp
        << ")";
    return oss;
}

#endif // MODEL_PERSON_HPP
