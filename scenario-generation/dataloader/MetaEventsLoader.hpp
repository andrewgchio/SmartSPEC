#ifndef DATALOADER_METAEVENTSLOADER_HPP
#define DATALOADER_METAEVENTSLOADER_HPP

#include <iostream>
#include <vector>
#include <algorithm>

#include "../include/rapidjson/document.h"

#include "../model/MetaEvent.hpp"

#include "../utils/Typedefs.hpp"
#include "../utils/TimeProfile.hpp"
#include "../utils/IOUtils.hpp"

namespace {

    // The maximum capacity of a metaevent
    const int MAX_METAEVENT_CAPACITY = 99999;

} // end namespace

class MetaEventsLoader {
public:

    // Constructors
    MetaEventsLoader();
    explicit MetaEventsLoader(const Filename& fname);

    // Queries
    int size() const;
    const MetaEventIDList& getIDs() const;
    const ProbabilityList& getPrs() const;

    MetaEvent& operator[](MetaEventID id);
    const MetaEvent& operator[](MetaEventID id) const;

    // Iterators
    std::vector<MetaEvent>::iterator begin();
    std::vector<MetaEvent>::iterator end();

    // Modifiers
    void add(const MetaEvent& me);
    void addOutMetaEvent();
    void addLeisureMetaEvent();

    // I/O
    friend std::ostream& operator<<(std::ostream& oss,
                                    const MetaEventsLoader& mel);

private:

    // A map of the metaevent id to the index in the list `entries`
    MetaEventIDIndexMap loc;

    // A list of metaevents
    std::vector<MetaEvent> entries;

    // A list of metaevent ids
    MetaEventIDList ids;

    // A list of metaevent probabilities 
    ProbabilityList prs;

};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Constructors

// Default Constructor
MetaEventsLoader::MetaEventsLoader() {}

// For the given metaevents file, read and load each metaevent
MetaEventsLoader::MetaEventsLoader(const Filename& fname) {
    std::cout << "... Reading MetaEvents file: " << fname << std::endl;

    // JSON reader setup
    rj::Document doc;
    openJSON(fname, doc);
    entries.reserve(doc.Size());

    for (const rj::Value& v : doc.GetArray()) {
    
        // Initialize a metaevent
        MetaEvent me;

        // id
        me.id = parseInt(v, "id");

        // description
        me.desc = parseStr(v, "description", "");

        // probability
        me.pr = parseDouble(v, "probability", 1.0);

        // spaces: ids and number
        me.selector = SpaceSelector{
                v["spaces"]["space-ids"],
                parseInt(v["spaces"], "number", 1)
        };

        // time-profiles
        me.tps.reserve(v["time-profiles"].Size());
        me.tpsPrs.reserve(v["time-profiles"].Size());
        for (const rj::Value& x : v["time-profiles"].GetArray()) {
            me.tps.push_back(TimeProfile{x["profile"]});
            me.tpsPrs.push_back(parseDouble(x, "probability", 1.0));
        }

        // capacity
        if (v["capacity"].IsString() && parseStr(v, "capacity") == "inf") {
            // metaevents with infinite capacity
            me.cap[-1] = std::make_pair(
                    Normal<int>{0,0},
                    Normal<int>{MAX_METAEVENT_CAPACITY,0}
            );
        }
        else if (v["capacity"].IsArray()) {
            // read each metaperson capacity range for metaevent
            for (const rj::Value& x : v["capacity"].GetArray()) {
                MetaPersonID mid = parseInt(x, "metaperson-id");
                const rj::Value &lo = x["lo"], &hi = x["hi"];
                me.cap[mid] = std::make_pair(
                        Normal<int>{lo[0].GetDouble(), lo[1].GetDouble()},
                        Normal<int>{hi[0].GetDouble(), hi[1].GetDouble()}
                );
            }
        }

        // Add the metaevent to the data loader
        add(me);
    }

    // Add a default out metaevent if one was not specified
    addOutMetaEvent();

    // Add a default leisure event if one was not specified
    addLeisureMetaEvent();

}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Queries

// Return the number of metaevents
int MetaEventsLoader::size() const { return entries.size(); }

// Return a list of all metaevent ids
const MetaEventIDList& MetaEventsLoader::getIDs() const { return ids; }

// Return a list of all metaevent probabilities
const ProbabilityList& MetaEventsLoader::getPrs() const { return prs; }

// Return a reference to the metaevent with the given id
MetaEvent& MetaEventsLoader::operator[](MetaEventID id) 
{ return entries[loc[id]]; }

// Return a reference to the metaevent with the given id (const)
const MetaEvent& MetaEventsLoader::operator[](MetaEventID id) const 
{ return entries[loc.at(id)]; }

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Iterators

// Iterator start
std::vector<MetaEvent>::iterator MetaEventsLoader::begin() 
{ return entries.begin(); }

// Iterator end
std::vector<MetaEvent>::iterator MetaEventsLoader::end() 
{ return entries.end(); }

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Modifiers

// Add a metaevent to the data loader
void MetaEventsLoader::add(const MetaEvent& me) {
    ids.push_back(me.id);
    prs.push_back(me.pr);
    loc[me.id] = entries.size();
    entries.push_back(me);
}


// Add the out-of-simulation metaevent to the data loader if it does not exist.
// The corresponding out-of-simulation event will be added when the event 
// constructor is called
void MetaEventsLoader::addOutMetaEvent() { 
    MetaEventIDList::const_iterator it = std::find(ids.begin(), ids.end(), -1); 
    if (it == ids.end()) {
        MetaEvent me;
        me.id = -1;
        me.desc = "out-of-simulation (added)";
        me.pr = 1.0;
        me.selector = SpaceSelector{SpaceIDList{0}};
        add(me);
    }
}

// Add the leisure metaevent to the data loader if it does not exist.
// The corresponding leisure event will be added when the event constructor is
// called.
void MetaEventsLoader::addLeisureMetaEvent() {
    MetaEventIDList::const_iterator it = std::find(ids.begin(), ids.end(), 0); 
    if (it == ids.end()) {
        MetaEvent me;
        me.id = 0;
        me.desc = "leisure (added)";
        me.pr = 1.0;
        me.selector = SpaceSelector{SpaceIDList{0}};
        add(me);
    }
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// I/O

// Print all metaevents
std::ostream& operator<<(std::ostream& oss, const MetaEventsLoader& mel) {
    oss << "MetaEvents:" << std::endl;
    for (const MetaEvent& me : mel.entries)
        oss << "  " << me << std::endl;
    return oss;
}

#endif // DATALOADER_METAEVENTSLOADER_HPP
