#ifndef DATALOADER_PEOPLELOADER_HPP
#define DATALOADER_PEOPLELOADER_HPP

#include <iostream>
#include <string>
#include <vector>

#include "../include/rapidjson/document.h"

#include "../model/Person.hpp"

#include "../utils/Typedefs.hpp"

class PeopleLoader {
public:

    PeopleLoader();
    explicit PeopleLoader(const Filename& fname);

    int size() const;
    const PersonIDList& getIDs() const;

    void add(const Person& p);

    void dump(const Filename& fname);

    Person& operator[](PersonID id);
    const Person& operator[](PersonID id) const;

    friend std::ostream& operator<<(std::ostream& oss, const PeopleLoader& pl);

    // Iterators
    std::vector<Person>::iterator begin() { return entries.begin(); }
    std::vector<Person>::iterator end()   { return entries.end(); }

private:

    PersonIDIndexMap loc;
    std::vector<Person> entries;

    PersonIDList ids;

};

PeopleLoader::PeopleLoader() {}

PeopleLoader::PeopleLoader(const Filename& fname) {
    if (fname == "none")
        return;

    std::cout << "... Reading People file: " << fname << std::endl;

    rj::Document doc;
    openJSON(fname, doc);
    entries.reserve(doc.Size());

    for (const rj::Value& v : doc.GetArray()) {
        Person p;
        p.id   = parseInt(v, "id");
        p.mid  = parseInt(v, "metaperson-id");
        p.desc = parseStr(v, "description", "");
        p.tp   = parseInt(v, "profile-index");
        add(p);
    }
}

int PeopleLoader::size() const { return entries.size(); }

const PersonIDList& PeopleLoader::getIDs() const { return ids; }

void PeopleLoader::add(const Person& p) {
    ids.push_back(p.id);
    loc[p.id] = entries.size();
    entries.push_back(p);
}

void PeopleLoader::dump(const Filename& fname) {
    rj::Document doc;
    doc.SetArray();
    rj::Document::AllocatorType& alloc = doc.GetAllocator();
    for (const Person& p : entries)
        doc.PushBack(p.dump(alloc), alloc);
    dumpJSON(fname, doc);
}

Person& PeopleLoader::operator[](PersonID id) {
    return entries[loc[id]];
}

const Person& PeopleLoader::operator[](PersonID id) const {
    return entries[loc.at(id)];
}

std::ostream& operator<<(std::ostream& oss, const PeopleLoader& pl) {
    oss << "People:" << std::endl;
    for (const Person& p : pl.entries)
        oss << "  " << p << std::endl;
    return oss;
}

#endif // DATALOADER_PEOPLELOADER_HPP
