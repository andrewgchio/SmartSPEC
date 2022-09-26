#ifndef DATALOADER_SPACESLOADER_HPP
#define DATALOADER_SPACESLOADER_HPP

#include <iostream>
#include <string>
#include <vector>
#include <cmath>

#include "../include/rapidjson/document.h"

#include "../model/Space.hpp"

#include "../utils/Typedefs.hpp"
#include "../utils/IOUtils.hpp"

class SpacesLoader {
public:

    // Constructors
    SpacesLoader();
    SpacesLoader(const Filename& fname, const Filename& cachefile="");

    // Queries
    int size() const;
    const SpaceIDList& getIDs() const;

    double dist(SpaceID cid1, SpaceID cid2) const;

    SpaceID getOutsideSpaceID() const;
    Space& getOutsideSpace();

    Space& operator[](SpaceID id);
    const Space& operator[](SpaceID id) const;

    // Iterators
    std::vector<Space>::iterator begin();
    std::vector<Space>::iterator end();

    // Modifiers
    void add(const Space& c);

    // I/O
    friend std::ostream& operator<<(std::ostream& oss, const SpacesLoader& sl);

private:

    // A map of the space id to the index in the list `entries`
    SpaceIDIndexMap loc;

    // A list of spaces
    std::vector<Space> entries;

    // A list of space ids
    SpaceIDList ids;

};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Constructors

// Default Constructor
SpacesLoader::SpacesLoader() {}

// For the given spaces file, read and load each space
SpacesLoader::SpacesLoader(const Filename& fname, const Filename& cache) {
    std::cout << "... Reading Spaces file: " << fname << std::endl;

    // JSON reader setup
    rj::Document doc;
    openJSON(fname, doc);
    entries.reserve(doc.Size());

    for (const rj::Value& v : doc.GetArray()) {

        // Initialize a space
        Space c;

        // id
        c.id = parseInt(v, "id");

        // description
        c.desc = parseStr(v, "description", "");

        // coordinates
        c.coords = parseDoubleArr(v, "coordinates");

        // capacity
        c.cap = parseInt(v, "capacity");

        // neighbors
        c.neighbors = parseIntArr(v, "neighbors");

        // Add the space to the data loader
        add(c);
    }

}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Queries

// Return the number of spaces
int SpacesLoader::size() const { return entries.size(); }

// Return a list of all the space ids
const SpaceIDList& SpacesLoader::getIDs() const { return ids; }

// Return the manhattan distance between the two ids
double SpacesLoader::dist(SpaceID cid1, SpaceID cid2) const {
    Coordinates coords1 = (*this)[cid1].coords;
    Coordinates coords2 = (*this)[cid2].coords;
    return fabs(coords1[0] - coords2[0]) + 
           fabs(coords1[1] - coords2[1]) + 
           fabs(coords1[2] - coords2[2]);
}

// Return the outside space id
SpaceID SpacesLoader::getOutsideSpaceID() const { return 0; }

// Return a reference to the outside space
Space& SpacesLoader::getOutsideSpace() { return (*this)[getOutsideSpaceID()]; }

// Return a reference to the space with the given id
Space& SpacesLoader::operator[](SpaceID id) { return entries[loc[id]]; }

// Return a reference to the space with the given id (const)
const Space& SpacesLoader::operator[](SpaceID id) const 
{ return entries[loc.at(id)]; }

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Iterators

// Iterator start
std::vector<Space>::iterator SpacesLoader::begin() { return entries.begin(); }

// Iterator end
std::vector<Space>::iterator SpacesLoader::end() { return entries.end(); }

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Modifiers

// Add a space to the data loader
void SpacesLoader::add(const Space& c) {
    ids.push_back(c.id);
    loc[c.id] = entries.size();
    entries.push_back(c);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// I/O

// Print all spaces
std::ostream& operator<<(std::ostream& oss, const SpacesLoader& sl) {
    oss << "Spaces:" << std::endl;
    for (const Space& c : sl.entries)
        oss << "  " << c << std::endl;
    return oss;
}

#endif // DATALOADER_SPACESLOADER_HPP
