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

    SensorsLoader();
    explicit SensorsLoader(const Filename& fname);

    int size() const;
    const SensorIDList& getIDs() const;

    void add(const Sensor& s);

    Sensor& operator[](SensorID id);
    const Sensor& operator[](SensorID id) const;

    friend std::ostream& operator<<(std::ostream& oss, const SensorsLoader& sl);

    // Iterators
    std::vector<Sensor>::iterator begin() { return entries.begin(); }
    std::vector<Sensor>::iterator end()   { return entries.end(); }

private:

    SensorIDIndexMap loc;
    std::vector<Sensor> entries;
    SensorIDList ids;

};

SensorsLoader::SensorsLoader() {}

SensorsLoader::SensorsLoader(const Filename& fname) {
    std::cout << "... Reading Sensors file: " << fname << std::endl;

    rj::Document doc;
    openJSON(fname, doc);
    entries.reserve(doc.Size());

    for (const rj::Value& v : doc.GetArray()) {
        Sensor s;
        s.id = parseInt(v, "id");
        s.desc = parseStr(v, "description", "");
        s.intr = parseInt(v, "interval");
        s.mob = parseStr(v, "mobility");

        if (s.mob == "static") 
            s.cov = parseIntArr(v, "coverage");
        else if (s.mob == "mobile")
            s.cov = PersonIDList{parseInt(v, "coverage")};

        add(s);
    }
}

int SensorsLoader::size() const { return entries.size(); }

const SensorIDList& SensorsLoader::getIDs() const { return ids; }

void SensorsLoader::add(const Sensor& s) {
    ids.push_back(s.id);
    loc[s.id] = entries.size();
    entries.push_back(s);
}

Sensor& SensorsLoader::operator[](SensorID id) {
    return entries[loc[id]];
}

const Sensor& SensorsLoader::operator[](SensorID id) const {
    return entries[loc.at(id)];
}

std::ostream& operator<<(std::ostream& oss, const SensorsLoader& sl) {
    oss << "Sensors:" << std::endl;
    for (const Sensor& s : sl.entries)
        oss << "  " << s << std::endl;
    return oss;
}

#endif // DATALOADER_SENSORSLOADER_HPP
