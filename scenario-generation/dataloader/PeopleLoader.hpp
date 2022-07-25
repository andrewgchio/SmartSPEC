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

    // Constructors
    PeopleLoader();
    explicit PeopleLoader(const Filename& fname);

    // Queries
    int size() const;
    const PersonIDList& getIDs() const;

    Person& operator[](PersonID id);
    const Person& operator[](PersonID id) const;

    // Iterators
    std::vector<Person>::iterator begin();
    std::vector<Person>::iterator end();

    // Modifiers
    void add(const Person& p);

    // I/O
    void dump(const Filename& fname);

    friend std::ostream& operator<<(std::ostream& oss, const PeopleLoader& pl);

private:

    // A map of the person id to the index in the list `entries`
    PersonIDIndexMap loc;
    
    // A list of people
    std::vector<Person> entries;

    // A list of person ids
    PersonIDList ids;

};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Constructors

// Default Constructor
PeopleLoader::PeopleLoader() {}

// For the given people file, read and load each person
PeopleLoader::PeopleLoader(const Filename& fname) {
    std::cout << "... Reading People file: " << fname << std::endl;

    // JSON reader setup
    rj::Document doc;
    openJSON(fname, doc);
    entries.reserve(doc.Size());

    for (const rj::Value& v : doc.GetArray()) {

        // Initialize a person
        Person p;

        // id 
        p.id   = parseInt(v, "id");

        // metaperson-id
        p.mid  = parseInt(v, "metaperson-id");

        // description
        p.desc = parseStr(v, "description", "");

        // profile-index
        p.tp   = parseInt(v, "profile-index");

        // Add the person the data loader
        add(p);
    }
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Queries

// Return the number of people
int PeopleLoader::size() const { return entries.size(); }

// Return a list fo all the person ids
const PersonIDList& PeopleLoader::getIDs() const { return ids; }

// Return a reference to the person with the given id
Person& PeopleLoader::operator[](PersonID id) { return entries[loc[id]]; }

// Return a reference to the person with the given id (const)
const Person& PeopleLoader::operator[](PersonID id) const 
{ return entries[loc.at(id)]; }

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Iterators

// Iterator start
std::vector<Person>::iterator PeopleLoader::begin() { return entries.begin(); }

// Iterator end
std::vector<Person>::iterator PeopleLoader::end() { return entries.end(); }

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Modifiers

// Add a person to the data loader
void PeopleLoader::add(const Person& p) {
    ids.push_back(p.id);
    loc[p.id] = entries.size();
    entries.push_back(p);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// I/O

// Save the people in the data loader into a file
void PeopleLoader::dump(const Filename& fname) {
    rj::Document doc;
    doc.SetArray();
    rj::Document::AllocatorType& alloc = doc.GetAllocator();
    for (const Person& p : entries)
        doc.PushBack(p.dump(alloc), alloc);
    dumpJSON(fname, doc);
}


// Print all people
std::ostream& operator<<(std::ostream& oss, const PeopleLoader& pl) {
    oss << "People:" << std::endl;
    for (const Person& p : pl.entries)
        oss << "  " << p << std::endl;
    return oss;
}

#endif // DATALOADER_PEOPLELOADER_HPP
