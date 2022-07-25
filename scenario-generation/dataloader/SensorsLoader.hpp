#ifndef DATALOADER_SENSORSLOADER_HPP
#define DATALOADER_SENSORSLOADER_HPP

#include <iostream>
#include <string>
#include <vector>

#include "../include/rapidjson/document.h"

#include "../model/Sensor.hpp"

#include "../utils/Typedefs.hpp"
#include "../utils/IOUtils.hpp"

class SensorsLoader {
public:

    // Constructors
    SensorsLoader();
    explicit SensorsLoader(const Filename& fname);

    // Queries
    int size() const;
    const SensorIDList& getIDs() const;

    Sensor& operator[](SensorID id);
    const Sensor& operator[](SensorID id) const;

    // Iterators
    std::vector<Sensor>::iterator begin();
    std::vector<Sensor>::iterator end();

    // Modifiers
    void add(const Sensor& s);

    // I/O
    friend std::ostream& operator<<(std::ostream& oss, const SensorsLoader& sl);

private:
    // A map of the sensor id to the index in the list `entries`
    SensorIDIndexMap loc;

    // A list of sensors
    std::vector<Sensor> entries;

    // A list of sensor ids
    SensorIDList ids;

};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Constructors

// Default Constructor
SensorsLoader::SensorsLoader() {}

// For the given sensors, read and load each sensor
SensorsLoader::SensorsLoader(const Filename& fname) {
    std::cout << "... Reading Sensors file: " << fname << std::endl;

    // JSON reader setup
    rj::Document doc;
    openJSON(fname, doc);
    entries.reserve(doc.Size());

    for (const rj::Value& v : doc.GetArray()) {

        // Initialize a sensor
        Sensor s;

        // id
        s.id = parseInt(v, "id");
        
        // description
        s.desc = parseStr(v, "description", "");

        // interval 
        s.intr = parseInt(v, "interval");

        // mobility
        s.mob = parseStr(v, "mobility");

        // coverage
        if (s.mob == "static") 
            s.cov = parseIntArr(v, "coverage");
        else if (s.mob == "mobile")
            s.cov = PersonIDList{parseInt(v, "coverage")};

        // Add the person to the data loader
        add(s);
    }
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Queries

// Return the number of sensors
int SensorsLoader::size() const { return entries.size(); }

// Return a list of all the sensor ids
const SensorIDList& SensorsLoader::getIDs() const { return ids; }

// Return a reference to the sensor with hte given id
Sensor& SensorsLoader::operator[](SensorID id) { return entries[loc[id]]; }

// Return a reference to the sensor with hte given id (const)
const Sensor& SensorsLoader::operator[](SensorID id) const 
{ return entries[loc.at(id)]; }

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Iterators

// Iterator start
std::vector<Sensor>::iterator SensorsLoader::begin() { return entries.begin(); }

// Iterator end
std::vector<Sensor>::iterator SensorsLoader::end() { return entries.end(); }

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Modifiers

// Add a sensor to the data loader
void SensorsLoader::add(const Sensor& s) {
    ids.push_back(s.id);
    loc[s.id] = entries.size();
    entries.push_back(s);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// I/O

// Print all sensors
std::ostream& operator<<(std::ostream& oss, const SensorsLoader& sl) {
    oss << "Sensors:" << std::endl;
    for (const Sensor& s : sl.entries)
        oss << "  " << s << std::endl;
    return oss;
}

#endif // DATALOADER_SENSORSLOADER_HPP
