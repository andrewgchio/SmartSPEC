#ifndef DATALOADER_EVENTSLOADER_HPP
#define DATALOADER_EVENTSLOADER_HPP

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <utility>

#include "../include/rapidjson/document.h"

#include "../model/Event.hpp"

#include "../utils/Typedefs.hpp"
#include "../utils/IOUtils.hpp"

namespace {

    // The maximum capacity of an event
    const int MAX_EVENT_CAPACITY = 99999;

} // end namespace

class EventsLoader {
public:

    // Construtors
    EventsLoader();
    explicit EventsLoader(const Filename& fname);

    // Queries
    int size() const;
    const EventIDList& getIDs() const;

    EventID getLeisureEventID() const;
    EventID getOutEventID() const;
    Event& getLeisureEvent();
    Event& getOutEvent();

    Event& operator[](EventID id);
    const Event& operator[](EventID id) const;

    // Iterators
    std::vector<Event>::iterator begin();
    std::vector<Event>::iterator end();

    // Modifiers
    void add(const Event& e);
    void addOutEvent();
    void addLeisureEvent();

    // I/O
    void dump(const Filename& fname);

    friend std::ostream& operator<<(std::ostream& oss, const EventsLoader& el);

private:

    // A map of the event id to the index in the list `entries`
    EventIDIndexMap loc;

    // A list of events
    std::vector<Event> entries;

    // A list of event ids
    EventIDList ids;

};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Constructors

// Default Constructor
EventsLoader::EventsLoader() {}

// For the given events file, read and load each event
EventsLoader::EventsLoader(const Filename& fname) {
    std::cout << "... Reading Events file: " << fname << std::endl;

    // JSON reader setup
    rj::Document doc;
    openJSON(fname, doc);
    entries.reserve(doc.Size());

    for (const rj::Value& v : doc.GetArray()) {

        // Initialize an event
        Event e;

        // id
        e.id = parseInt(v, "id");

        // metaevent-id
        e.mid = parseInt(v, "metaevent-id");

        // description
        e.desc = parseStr(v, "description", "");

        // profile-index
        e.tp = parseInt(v, "profile-index");

        // spaces
        e.spaces = parseIntArr(v, "space-ids");

        // capacity
        if (v["capacity"].IsString() && parseStr(v, "capacity") == "inf") {
            // events with infinite capacity
            e.cap[-1] = std::make_pair(MAX_EVENT_CAPACITY, 0);
        }
        else if (v["capacity"].IsArray()) {
            // read each metaperson capacities for event
            for (const rj::Value& x : v["capacity"].GetArray()) {
                MetaPersonID mid = parseInt(x, "metaperson-id");
                e.cap[mid] = std::make_pair(x["range"][0].GetInt(),
                                            x["range"][1].GetInt());
            }
        } 

        // Add the event to the data loader
        add(e);
    }

    // Add a default out event if one was not specified
    addOutEvent();

    // Add a default leisure event if one was not specified
    addLeisureEvent();

}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Queries

// Return the number of events
int EventsLoader::size() const { return entries.size(); }

// Return a list of all the event ids 
const EventIDList& EventsLoader::getIDs() const { return ids; }

// Return the leisure event id
EventID EventsLoader::getLeisureEventID() const { return 0; }

// Return the out-of-simulation event id
EventID EventsLoader::getOutEventID() const { return -1; }

// Return a reference to the leisure event
Event& EventsLoader::getLeisureEvent() { return (*this)[getLeisureEventID()]; }

// Return a reference to the out-of-simulation event
Event& EventsLoader::getOutEvent() { return (*this)[getOutEventID()]; }

// Return a reference to the event with the given id
Event& EventsLoader::operator[](EventID id) { return entries[loc[id]]; }

// Return a reference to the event with the given id (const)
const Event& EventsLoader::operator[](EventID id) const 
{ return entries[loc.at(id)]; }

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Iterators

// Iterator start
std::vector<Event>::iterator EventsLoader::begin() { return entries.begin(); }

// Iterator end
std::vector<Event>::iterator EventsLoader::end() { return entries.end(); }

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Modifiers

// Add an event to the data loader
void EventsLoader::add(const Event& e) {
    ids.push_back(e.id);
    loc[e.id] = entries.size();
    entries.push_back(e);
}

// Add the out-of-simulation event to the data loader if it does not exist. 
// The out-of-simulation event occurs in the outside space 0.
void EventsLoader::addOutEvent() {
    EventIDList::const_iterator it = std::find(ids.begin(), ids.end(), -1);
    if (it == ids.end()) {
        Event e;
        e.id = -1;
        e.mid = -1;
        e.desc = "out-of-simulation (added)";
        e.tp = 0;
        e.spaces = SpaceIDList{0};
        add(e);
    }
}

// Add the leisure event to the data loader if it does not exist
// The leisure event occurs in the outside space 0.
void EventsLoader::addLeisureEvent() {
    EventIDList::const_iterator it = std::find(ids.begin(), ids.end(), 0);
    if (it == ids.end()) {
        Event e;
        e.id = 0;
        e.mid = 0;
        e.desc = "leisure (added)";
        e.tp = 0;
        e.spaces = SpaceIDList{0};
        e.cap[-1] = std::make_pair(MAX_EVENT_CAPACITY, 0);
        add(e);
    }
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// I/O

// Save the events in the data loader into a file
void EventsLoader::dump(const Filename& fname) {
    rj::Document doc;
    doc.SetArray();
    rj::Document::AllocatorType& alloc = doc.GetAllocator();
    for (const Event& e : entries)
        doc.PushBack(e.dump(alloc), alloc);
    dumpJSON(fname, doc);
}

// Print all events
std::ostream& operator<<(std::ostream& oss, const EventsLoader& el) {
    oss << "Events:" << std::endl;
    for (const Event& e : el.entries)
        oss << "  " << e << std::endl;
    return oss;
}

#endif // DATALOADER_EVENTSLOADER_HPP
