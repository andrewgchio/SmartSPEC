#ifndef MODEL_PERSON_HPP
#define MODEL_PERSON_HPP

#include <iostream>

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

    // Synthetic Data Generation
    SpaceID currSpace;
    std::set<EventLogistics> attended;

    rj::Value dump(rj::Document::AllocatorType& alloc) const;

    friend std::ostream& operator<<(std::ostream& oss, const Person& p);
};

rj::Value Person::dump(rj::Document::AllocatorType& alloc) const {
    rj::Value v;
    v.SetObject();
    v.AddMember("id", rj::Value(id).Move(), alloc);
    v.AddMember("metaperson-id", rj::Value(mid).Move(), alloc);
    v.AddMember("description", rj::Value(desc.c_str(), alloc), alloc);
    v.AddMember("profile-index", rj::Value(tp).Move(), alloc);
    return v;
}

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
