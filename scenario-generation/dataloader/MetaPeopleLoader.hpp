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

    MetaPeopleLoader();
    explicit MetaPeopleLoader(const Filename& fname);

    int size() const;
    const MetaPersonIDList& getIDs() const;
    const ProbabilityList& getPrs() const;

    void add(const MetaPerson& mp);

    MetaPerson& operator[](MetaPersonID id);
    const MetaPerson& operator[](MetaPersonID id) const;

    friend std::ostream& operator<<(std::ostream& oss, 
                                    const MetaPeopleLoader& mpl);

    // Iterators
    std::vector<MetaPerson>::iterator begin() { return entries.begin(); }
    std::vector<MetaPerson>::iterator end()   { return entries.end(); }

private: 
    
    MetaPersonIDIndexMap loc;
    std::vector<MetaPerson> entries;

    MetaPersonIDList ids;
    ProbabilityList prs;

};

MetaPeopleLoader::MetaPeopleLoader() {}

MetaPeopleLoader::MetaPeopleLoader(const Filename& fname) {
    std::cout << "... Reading MetaPeople file: " << fname << std::endl;

    rj::Document doc;
    openJSON(fname, doc);
    entries.reserve(doc.Size());

    for (const rj::Value& v : doc.GetArray()) {
        MetaPerson mp;
        mp.id = parseInt(v, "id");
        mp.pr = parseDouble(v, "probability");
        mp.desc = parseStr(v, "description", "");

        mp.tps.reserve(v["time-profiles"].Size());
        mp.tpsPrs.reserve(v["time-profiles"].Size());
        for (const rj::Value& x : v["time-profiles"].GetArray()) {
            mp.tps.push_back(TimeProfile{x["profile"]});
            mp.tpsPrs.push_back(parseDouble(x, "probability"));
        }

        for (const rj::Value& x : v["event-affinity"].GetArray())
            mp.aff[parseInt(x, "metaevent-id")] = parseDouble(x, "probability");

        add(mp);
    }
}

int MetaPeopleLoader::size() const { return entries.size(); }

const MetaPersonIDList& MetaPeopleLoader::getIDs() const { return ids; }

const ProbabilityList& MetaPeopleLoader::getPrs() const { return prs; }

void MetaPeopleLoader::add(const MetaPerson& mp) {
    ids.push_back(mp.id);
    prs.push_back(mp.pr);
    loc[mp.id] = entries.size();
    entries.push_back(mp);
}

MetaPerson& MetaPeopleLoader::operator[](MetaPersonID id) {
    return entries[loc[id]];
}

const MetaPerson& MetaPeopleLoader::operator[](MetaPersonID id) const {
    return entries[loc.at(id)];
}

std::ostream& operator<<(std::ostream& oss, const MetaPeopleLoader& mpl) {
    oss << "MetaPeople:" << std::endl;
    for (const MetaPerson& mp : mpl.entries)
        oss << "  " << mp << std::endl;
    return oss;
}

#endif // DATALOADER_METAPERSONLOADER_HPP
