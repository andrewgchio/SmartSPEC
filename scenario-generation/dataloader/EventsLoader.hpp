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

    const int MAX_EVENT_CAPACITY = 99999;

} // end namespace

class EventsLoader {
public:

    EventsLoader();
    explicit EventsLoader(const Filename& fname);

    int size() const;
    const EventIDList& getIDs() const;

    EventID getLeisureEventID() const;
    EventID getOutEventID() const;
    Event& getLeisureEvent();
    Event& getOutEvent();

    void addOutEvent();
    void addLeisureEvent();

    void add(const Event& e);

    void dump(const Filename& fname);

    Event& operator[](EventID id);
    const Event& operator[](EventID id) const;

    friend std::ostream& operator<<(std::ostream& oss, const EventsLoader& el);

    // Iterators
    std::vector<Event>::iterator begin() { return entries.begin(); }
    std::vector<Event>::iterator end()   { return entries.end(); }

private:

    EventIDIndexMap loc;
    std::vector<Event> entries;
    EventIDList ids;

};

EventsLoader::EventsLoader() {}

EventsLoader::EventsLoader(const Filename& fname) {
    std::cout << "... Reading Events file: " << fname << std::endl;

    rj::Document doc;
    openJSON(fname, doc);
    entries.reserve(doc.Size());

    for (const rj::Value& v : doc.GetArray()) {
        Event e;
        e.id = parseInt(v, "id");
        e.mid = parseInt(v, "metaevent-id");
        e.desc = parseStr(v, "description", "");
        e.tp = parseInt(v, "profile-index");

        e.spaces = parseIntArr(v, "space-ids");

        // Leisure event
        if (v["capacity"].IsString() && parseStr(v, "capacity") == "inf") {
            e.cap[-1] = std::make_pair(MAX_EVENT_CAPACITY, 0);
        }
        // Other events
        else if (v["capacity"].IsArray()) {
            for (const rj::Value& x : v["capacity"].GetArray()) {
                MetaPersonID mid = parseInt(x, "metaperson-id");
                if (x.HasMember("range")) {
                    e.cap[mid] = std::make_pair(x["range"][0].GetInt(),
                                                x["range"][1].GetInt());
                }
                else {
                    e.cap[mid] = std::make_pair(parseInt(x, "lo", 0), 
                                                parseInt(x, "hi"));
                }
            }
        } 

        add(e);
    }

    // Will be added it does not exist
    addOutEvent();
    addLeisureEvent();

}

int EventsLoader::size() const { return entries.size(); }

const EventIDList& EventsLoader::getIDs() const { return ids; }

EventID EventsLoader::getLeisureEventID() const { return 0; }
EventID EventsLoader::getOutEventID() const { return -1; }

Event& EventsLoader::getLeisureEvent() { return (*this)[getLeisureEventID()]; }
Event& EventsLoader::getOutEvent() { return (*this)[getOutEventID()]; }

void EventsLoader::addOutEvent() {
    // Add Out (of simulation) event if not exists
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

void EventsLoader::addLeisureEvent() {
    // Add Leisure event if not exists
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

void EventsLoader::add(const Event& e) {
    ids.push_back(e.id);
    loc[e.id] = entries.size();
    entries.push_back(e);
}

void EventsLoader::dump(const Filename& fname) {
    rj::Document doc;
    doc.SetArray();
    rj::Document::AllocatorType& alloc = doc.GetAllocator();
    for (const Event& e : entries)
        doc.PushBack(e.dump(alloc), alloc);
    dumpJSON(fname, doc);
}

Event& EventsLoader::operator[](EventID id) {
    return entries[loc[id]];
}

const Event& EventsLoader::operator[](EventID id) const {
    return entries[loc.at(id)];
}

std::ostream& operator<<(std::ostream& oss, const EventsLoader& el) {
    oss << "Events:" << std::endl;
    for (const Event& e : el.entries)
        oss << "  " << e << std::endl;
    return oss;
}

#endif // DATALOADER_EVENTSLOADER_HPP
