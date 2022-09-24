#ifndef SENSOR_OBSERVATION_GENERATOR_DOOROBSERVATIONGENERATOR_HPP
#define SENSOR_OBSERVATION_GENERATOR_DOOROBSERVATIONGENERATOR_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>

#include <boost/icl/interval_set.hpp>

#include "../utils/Typedefs.hpp"
#include "../utils/DateUtils.hpp"

#include "../model/Space.hpp"
#include "../model/Sensor.hpp"
#include "../model/Person.hpp"

#include "../dataloader/DataLoader.hpp"

#include "OccupancyObservationGenerator.hpp"

class DoorObservationGenerator : public OccupancyObservationGenerator {
public:

    // Default constructor
    DoorObservationGenerator();

    struct PersonState {
    };

    struct SensorState {
        boost::icl::interval_set<long> opened;
    };

    // A map storing the state of each person
    std::map<PersonID, PersonState> pstatesMap;
    std::map<SensorID, SensorState> sstatesMap;

    // Methods to override to update and record the state
    void updateState(const Person& p, const TrajectoryEntry& te) override;
    void recordState(const Person& p) override;
    void recordRest() override;

    // I/O
    void writeHeader() override;

};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Constructor

// Default constructor
DoorObservationGenerator::DoorObservationGenerator() {}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// State management

void DoorObservationGenerator::updateState(
        const Person& p, const TrajectoryEntry& te) {

    // Threshold to determine moving or stationary
    static Time mins5{60*5};

    // Unpack trajectory entry te
    const Space& c           = dl->C[std::get<0>(te)];
    const SensorIDSet& cover = getCoverage(c);
    const DateTime& startDT  = std::get<1>(te);
    const DateTime& endDT    = std::get<2>(te);

    // If no sensor observes the room, then no observation should be made
    if (cover.empty()) {
        return;
    }

    // Only one door sensor per space
    SensorID sid = *cover.begin();

    // Get the covering sensor state
    SensorState& state = sstatesMap[sid];

    // Moving through a space; determine when door is activated
    if (endDT - startDT < mins5) {
        int nSecs = randInt((endDT - startDT).count());
        DateTime sdt{startDT + Time{nSecs}};
        DateTime edt{startDT + Time{nSecs+10}};

        sstatesMap[sid].opened.insert(
            boost::icl::interval<long>::right_open(sdt.count(), edt.count())
        );
    }

}

void DoorObservationGenerator::recordState(const Person& p) {}

void DoorObservationGenerator::recordRest() {
    for (const auto& entry : sstatesMap) {
        for (const auto& intr : entry.second.opened) {
            DateTime dt{intr.lower()};
            out << entry.first << "," << dt << std::endl;
            coutlog << "Sensor " << entry.first
                    << " activated at datetime " << dt << std::endl;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// I/O

void DoorObservationGenerator::writeHeader() {
    out << "SensorID,DateTime" << std::endl;
    coutlog << "Door Observation data from metasensor " << ms.id << std::endl;
}

#endif // SENSOR_OBSERVATION_GENERATOR_DOOROBSERVATIONGENERATOR_HPP
