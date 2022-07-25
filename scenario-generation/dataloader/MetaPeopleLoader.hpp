#ifndef DATALOADER_METAPERSONLOADER_HPP
#define DATALOADER_METAPERSONLOADER_HPP

#include <iostream>
#include <string>
#include <vector>
#include <utility>

#include "../include/rapidjson/document.h"

#include "../model/MetaPerson.hpp"

#include "../utils/Typedefs.hpp"
#include "../utils/IOUtils.hpp"
#include "../utils/TimeProfile.hpp"

class MetaPeopleLoader {
public:

    // Constructors
    MetaPeopleLoader();
    explicit MetaPeopleLoader(const Filename& fname);

    // Queries
    int size() const;
    const MetaPersonIDList& getIDs() const;
    const ProbabilityList& getPrs() const;

    MetaPerson& operator[](MetaPersonID id);
    const MetaPerson& operator[](MetaPersonID id) const;

    // Iterators
    std::vector<MetaPerson>::iterator begin();
    std::vector<MetaPerson>::iterator end();

    // Modifiers
    void add(const MetaPerson& mp);

    // I/O
    friend std::ostream& operator<<(std::ostream& oss, 
                                    const MetaPeopleLoader& mpl);

private: 
    
    // A map of the metaperson id to the index in the list `entries`
    MetaPersonIDIndexMap loc;

    // A list of metapeople
    std::vector<MetaPerson> entries;

    // A list of metaperson ids
    MetaPersonIDList ids;

    // A list of metaperson probabilities
    ProbabilityList prs;

};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Constructors

// Default Constructor
MetaPeopleLoader::MetaPeopleLoader() {}

// For the given metapeople file, read and load each metaperson
MetaPeopleLoader::MetaPeopleLoader(const Filename& fname) {
    std::cout << "... Reading MetaPeople file: " << fname << std::endl;

    // JSON reader setup
    rj::Document doc;
    openJSON(fname, doc);
    entries.reserve(doc.Size());

    for (const rj::Value& v : doc.GetArray()) {
        
        // Initialize a metaperson
        MetaPerson mp;

        // id 
        mp.id = parseInt(v, "id");

        // description
        mp.desc = parseStr(v, "description", "");

        // probability
        mp.pr = parseDouble(v, "probability");

        // time-profiles
        mp.tps.reserve(v["time-profiles"].Size());
        mp.tpsPrs.reserve(v["time-profiles"].Size());
        for (const rj::Value& x : v["time-profiles"].GetArray()) {
            mp.tps.push_back(TimeProfile{x["profile"]});
            mp.tpsPrs.push_back(parseDouble(x, "probability"));
        }

        // event-affinity
        for (const rj::Value& x : v["event-affinity"].GetArray())
            mp.aff[parseInt(x, "metaevent-id")] = parseDouble(x, "probability");

        // Add the metaperson to the data loader
        add(mp);
    }
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Queries

// Return the number of metapeople
int MetaPeopleLoader::size() const { return entries.size(); }

// Return a list of all metaperson ids
const MetaPersonIDList& MetaPeopleLoader::getIDs() const { return ids; }

// Return a list of all metaperson probabilities
const ProbabilityList& MetaPeopleLoader::getPrs() const { return prs; }

// Return a reference to the metaperson with the given id
MetaPerson& MetaPeopleLoader::operator[](MetaPersonID id) 
{ return entries[loc[id]]; }

// Return a reference to the metaperson with the given id (const)
const MetaPerson& MetaPeopleLoader::operator[](MetaPersonID id) const 
{ return entries[loc.at(id)]; }

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Iterators

// Iterator start
std::vector<MetaPerson>::iterator MetaPeopleLoader::begin() 
{ return entries.begin(); }

// Iterator end
std::vector<MetaPerson>::iterator MetaPeopleLoader::end()
{ return entries.end(); }

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Modifiers

// Add a metaperson to the data loader
void MetaPeopleLoader::add(const MetaPerson& mp) {
    ids.push_back(mp.id);
    prs.push_back(mp.pr);
    loc[mp.id] = entries.size();
    entries.push_back(mp);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// I/O

// Print all metapeople
std::ostream& operator<<(std::ostream& oss, const MetaPeopleLoader& mpl) {
    oss << "MetaPeople:" << std::endl;
    for (const MetaPerson& mp : mpl.entries)
        oss << "  " << mp << std::endl;
    return oss;
}

#endif // DATALOADER_METAPERSONLOADER_HPP
