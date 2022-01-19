#ifndef MODEL_EVENT_HPP
#define MODEL_EVENT_HPP

#include <iostream>
#include <vector>
#include <map>

#include "../include/rapidjson/document.h"

#include "../utils/Typedefs.hpp"
#include "../utils/IOUtils.hpp"

class Event {
public:

    // Attributes
    EventID id;
    MetaEventID mid;
    Description desc;
    Index tp;
    SpaceIDList spaces;
    PersonCapRange cap;

    // Synthetic Data Generation
    std::map<MetaPersonID, CapRange> enrolled;

    bool canAttend(MetaPersonID mid);
    void enrollMetaPerson(MetaPersonID mid);

    rj::Value dump(rj::Document::AllocatorType& alloc) const;

    friend std::ostream& operator<<(std::ostream& oss, const Event& e);

};

bool Event::canAttend(MetaPersonID mid) {
    std::map<MetaPersonID, CapRange>::const_iterator eit = enrolled.find(mid);
    PersonCapRange::const_iterator rit = cap.find(mid);
    if (eit == enrolled.end()) // mid not found (no attendance yet)
        return rit == cap.end() ? false : rit->second.second != 0;

    return eit->second.second <= rit->second.second;
}

void Event::enrollMetaPerson(MetaPersonID mid) {
    enrolled[mid].second += 1;
}

rj::Value Event::dump(rj::Document::AllocatorType& alloc) const {
    rj::Value v;
    v.SetObject();
    v.AddMember("id", rj::Value(id).Move(), alloc);
    v.AddMember("metaevent-id", rj::Value(mid).Move(), alloc);
    v.AddMember("description", rj::Value(desc.c_str(), alloc), alloc);
    v.AddMember("profile-index", rj::Value(tp).Move(), alloc);

    rj::Value vSpaces;
    vSpaces.SetArray();
    for (SpaceID id : spaces)
        vSpaces.PushBack(rj::Value(id).Move(), alloc);
    v.AddMember("space-ids", vSpaces, alloc);

    rj::Value vCap;
    vCap.SetArray();
    for (auto& x : cap) {
        rj::Value entry;
        entry.SetObject();
        entry.AddMember("metaperson-id", rj::Value(x.first).Move(), alloc);

        rj::Value range;
        range.SetArray();
        range.PushBack(rj::Value(x.second.first).Move(), alloc);
        range.PushBack(rj::Value(x.second.second).Move(), alloc);
        entry.AddMember("range", range.Move(), alloc);

        vCap.PushBack(entry, alloc);
    }

    v.AddMember("capacity", vCap.Move(), alloc);

    return v;
}

std::ostream& operator<<(std::ostream& oss, const Event& e) {
    oss << "Event("
        << "id=" << e.id << ", "
        << "mid=" << e.mid << ", "
        << "desc=\"" << e.desc << "\", "
        << "tp=" << e.tp << ", "
        << "spaces=" << e.spaces << ", "
        << "capacity=" << e.cap
        << ")";
    return oss;
}

#endif // MODEL_EVENT_HPP
