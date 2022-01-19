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

    const int MAX_METAEVENT_CAPACITY = 99999;

} // end namespace

class MetaEventsLoader {
public:

    MetaEventsLoader();
    explicit MetaEventsLoader(const Filename& fname);

    int size() const;
    const MetaEventIDList& getIDs() const;
    const ProbabilityList& getPrs() const;

    void addOutMetaEvent();
    void addLeisureMetaEvent();

    void add(const MetaEvent& me);

    MetaEvent& operator[](MetaEventID id);
    const MetaEvent& operator[](MetaEventID id) const;

    friend std::ostream& operator<<(std::ostream& oss,
                                    const MetaEventsLoader& mel);

    // Iterators
    std::vector<MetaEvent>::iterator begin() { return entries.begin(); }
    std::vector<MetaEvent>::iterator end()   { return entries.end(); }

private:

    MetaEventIDIndexMap loc;
    std::vector<MetaEvent> entries;

    MetaEventIDList ids;
    ProbabilityList prs;

};

MetaEventsLoader::MetaEventsLoader() {}

MetaEventsLoader::MetaEventsLoader(const Filename& fname) {
    std::cout << "... Reading MetaEvents file: " << fname << std::endl;

    rj::Document doc;
    openJSON(fname, doc);
    entries.reserve(doc.Size());

    for (const rj::Value& v : doc.GetArray()) {
        MetaEvent me;
        me.id = parseInt(v, "id");
        me.desc = parseStr(v, "description", "");
        me.pr = parseDouble(v, "probability", 1.0);

        me.selector = SpaceSelector{
                v["spaces"]["space-ids"],
                parseInt(v["spaces"], "number", 1)
        };

        me.tps.reserve(v["time-profiles"].Size());
        me.tpsPrs.reserve(v["time-profiles"].Size());
        for (const rj::Value& x : v["time-profiles"].GetArray()) {
            me.tps.push_back(TimeProfile{x["profile"]});
            me.tpsPrs.push_back(parseDouble(x, "probability", 1.0));
        }

        // Leisure Event
        if (v["capacity"].IsString() && parseStr(v, "capacity") == "inf") {
            me.cap[-1] = std::make_pair(
                    Normal<int>{0,0},
                    Normal<int>{MAX_METAEVENT_CAPACITY,0}
            );
        }
        // Other events
        else if (v["capacity"].IsArray()) {
            for (const rj::Value& x : v["capacity"].GetArray()) {
                MetaPersonID mid = parseInt(x, "metaperson-id");
                if (x.HasMember("lo") && x.HasMember("hi")) {
                    const rj::Value &lo = x["lo"], &hi = x["hi"];
                    me.cap[mid] = std::make_pair(
                            Normal<int>{lo[0].GetDouble(), lo[1].GetDouble()},
                            Normal<int>{hi[0].GetDouble(), hi[1].GetDouble()}
                    );
                }
                else {
                    me.cap[mid] = std::make_pair(
                            Normal<int>{parseDouble(x,"lo-mean"),
                                        parseDouble(x,"lo-stdev")},
                            Normal<int>{parseDouble(x,"hi-mean"),
                                        parseDouble(x,"hi-stdev")}
                    );
                }
            }
        }
        add(me);
    }

    // Will be added if does not exist
    addOutMetaEvent();
    addLeisureMetaEvent();
}

int MetaEventsLoader::size() const { return entries.size(); }

const MetaEventIDList& MetaEventsLoader::getIDs() const { return ids; }

const ProbabilityList& MetaEventsLoader::getPrs() const { return prs; }

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

void MetaEventsLoader::add(const MetaEvent& me) {
    ids.push_back(me.id);
    prs.push_back(me.pr);
    loc[me.id] = entries.size();
    entries.push_back(me);
}

MetaEvent& MetaEventsLoader::operator[](MetaEventID id) {
    return entries[loc[id]];
}

const MetaEvent& MetaEventsLoader::operator[](MetaEventID id) const {
    return entries[loc.at(id)];
}

std::ostream& operator<<(std::ostream& oss, const MetaEventsLoader& mel) {
    oss << "MetaEvents:" << std::endl;
    for (const MetaEvent& me : mel.entries)
        oss << "  " << me << std::endl;
    return oss;
}

#endif // DATALOADER_METAEVENTSLOADER_HPP
