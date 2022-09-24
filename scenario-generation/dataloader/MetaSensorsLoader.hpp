#ifndef DATALOADER_METASENSORSLOADER_HPP
#define DATALOADER_METASENSORSLOADER_HPP

#include <iostream>
#include <string>
#include <vector>

#include "../include/rapidjson/document.h"

#include "../model/MetaSensor.hpp"

#include "../utils/Typedefs.hpp"
#include "../utils/IOUtils.hpp"

class MetaSensorsLoader {
public:

    // Constructors
    MetaSensorsLoader();
    explicit MetaSensorsLoader(const Filename& fname);

    // Queries
    int size() const;
    const MetaSensorIDList& getIDs() const;

    MetaSensor& operator[](MetaSensorID id);
    const MetaSensor& operator[](MetaSensorID id) const;

    // Iterators
    std::vector<MetaSensor>::iterator begin();
    std::vector<MetaSensor>::iterator end();

    // Modifiers
    void add(const MetaSensor& ms);

    // I/O
    friend std::ostream& operator<<(std::ostream& oss, 
                                    const MetaSensorsLoader& ms);

private:

    // A map of the metasensor id to the index in the list `entries`
    MetaSensorIDIndexMap loc;

    // A list of metasensors
    std::vector<MetaSensor> entries;

    // A list of metasensor ids
    MetaSensorIDList ids;

};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Constructors

// Default Constructor
MetaSensorsLoader::MetaSensorsLoader() {}

// For the given metasensors file, read and load each metasensor
MetaSensorsLoader::MetaSensorsLoader(const Filename& fname) {
    std::cout << "... Reading MetaSensors file: " << fname << std::endl;

    // JSON reader setup
    rj::Document doc;
    openJSON(fname, doc);
    entries.reserve(doc.Size());

    for (const rj::Value& v : doc.GetArray()) {

        // Initialize a metasensor
        MetaSensor ms;

        // id 
        ms.id = parseInt(v, "id");

        // description
        ms.desc = parseStr(v, "description", "");

        // Add the metasensor to the data loader
        add(ms);

    }

}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Queries

// Return the number of metasensors
int MetaSensorsLoader::size() const { return entries.size(); }

// Return the list of metasensor ids
const MetaSensorIDList& MetaSensorsLoader::getIDs() const { return ids; }

// Return a refernce to the metasensor with the given id
MetaSensor& MetaSensorsLoader::operator[](MetaSensorID id) 
{ return entries[loc[id]]; }

// Return a refernce to the metasensor with the given id (const)
const MetaSensor& MetaSensorsLoader::operator[](MetaSensorID id) const
{ return entries[loc.at(id)]; }

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Iterators

// Iterator start
std::vector<MetaSensor>::iterator MetaSensorsLoader::begin() 
{ return entries.begin(); }

// Iterator end
std::vector<MetaSensor>::iterator MetaSensorsLoader::end()
{ return entries.end(); }

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Modifiers

// Add a metasensor to the data loader
void MetaSensorsLoader::add(const MetaSensor& ms) {
    ids.push_back(ms.id);
    loc[ms.id] = entries.size();
    entries.push_back(ms);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// I/O

// Print all metasensors
std::ostream& operator<<(std::ostream& oss, const MetaSensorsLoader& msl) { 
    oss << "MetaSensors:" << std::endl;
    for (const MetaSensor& ms : msl.entries)
        oss << "  " << ms << std::endl;
    return oss;
}

#endif // DATALOADER_METASENSORSLOADER_HPP
