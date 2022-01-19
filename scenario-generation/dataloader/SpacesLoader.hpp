#ifndef DATALOADER_SPACESLOADER_HPP
#define DATALOADER_SPACESLOADER_HPP

#include <iostream>
#include <string>
#include <vector>

#include "../include/rapidjson/document.h"

#include "../model/Space.hpp"

#include "../utils/Typedefs.hpp"
#include "../utils/IOUtils.hpp"

class SpacesLoader {
public:

    SpacesLoader();
    SpacesLoader(const Filename& fname, const Filename& cachefile="");

    int size() const;
    const SpaceIDList& getIDs() const;

    SpaceID getOutsideSpaceID() const;
    Space& getOutsideSpace();

    void add(const Space& c);

    Space& operator[](SpaceID id);
    const Space& operator[](SpaceID id) const;

    friend std::ostream& operator<<(std::ostream& oss, const SpacesLoader& sl);

    // Iterators
    std::vector<Space>::iterator begin() { return entries.begin(); }
    std::vector<Space>::iterator end()   { return entries.end(); }

private:

    SpaceIDIndexMap loc;
    std::vector<Space> entries;

    SpaceIDList ids;

};

SpacesLoader::SpacesLoader() {}

SpacesLoader::SpacesLoader(const Filename& fname, const Filename& cache) {
    std::cout << "... Reading Spaces file: " << fname << std::endl;

    rj::Document doc;
    openJSON(fname, doc);
    entries.reserve(doc.Size());

    for (const rj::Value& v : doc.GetArray()) {
        Space c;
        c.id        = parseInt(v, "id");
        c.desc      = parseStr(v, "description", "");
        c.coords    = parseIntArr(v, "coordinates");
        c.cap       = parseInt(v, "capacity");
        c.neighbors = parseIntArr(v, "neighbors");
        add(c);
    }

}

int SpacesLoader::size() const { return entries.size(); }

const SpaceIDList& SpacesLoader::getIDs() const { return ids; }

SpaceID SpacesLoader::getOutsideSpaceID() const { return 0; }
Space& SpacesLoader::getOutsideSpace() { return (*this)[getOutsideSpaceID()]; }

void SpacesLoader::add(const Space& c) {
    ids.push_back(c.id);
    loc[c.id] = entries.size();
    entries.push_back(c);
}

Space& SpacesLoader::operator[](SpaceID id) {
    return entries[loc[id]];
}

const Space& SpacesLoader::operator[](SpaceID id) const {
    return entries[loc.at(id)];
}

std::ostream& operator<<(std::ostream& oss, const SpacesLoader& sl) {
    oss << "Spaces:" << std::endl;
    for (const Space& c : sl.entries)
        oss << "  " << c << std::endl;
    return oss;
}

#endif // DATALOADER_SPACESLOADER_HPP
