#ifndef SENSOR_OBSERVATION_GENERATOR_WIFIOBSERVATIONGENERATOR_HPP
#define SENSOR_OBSERVATION_GENERATOR_WIFIOBSERVATIONGENERATOR_HPP

#include <iostream>
#include <string>
#include <map>
#include <tuple>
#include <algorithm>

#include "../utils/Typedefs.hpp"
#include "../utils/DateUtils.hpp"
#include "../utils/IOUtils.hpp"
#include "../utils/RandomGenerator.hpp"
#include "../utils/NormalDistributions.hpp"
#include "../utils/Selectors.hpp"

#include "../model/Space.hpp"
#include "../model/Sensor.hpp"
#include "../model/Person.hpp"

#include "../dataloader/DataLoader.hpp"

#include "OccupancyObservationGenerator.hpp"

class WiFiObservationGenerator : public OccupancyObservationGenerator {
public:

    // Default Constructor
    WiFiObservationGenerator();

    struct State {
        SensorID lastWiFiAP;
        DateTime lastTime;
        std::vector<SensorID> wifiaps;
        std::vector<DateTime> times;
    };

    // A map storing the state of each person
    std::map<PersonID, State> statesMap;

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
WiFiObservationGenerator::WiFiObservationGenerator() {}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// State management

void WiFiObservationGenerator::updateState(
        const Person& p, const TrajectoryEntry& te) {

    // Threshold to determine moving or stationary
    static Time mins5{60*5};
    // Connection step size if staying in a space
    static NormalTime connStep{"00:15:00", "00:03:00"};

    // Unpack trajectory entry te
    const Space& c            = dl->C[std::get<0>(te)];
    const SensorIDSet& cover  = getCoverage(c);
    const DateTime& startDT   = std::get<1>(te);
    const DateTime& endDT     = std::get<2>(te);

    // Get the person's state
    State& state = statesMap[p.id];

    // If no sensor observes the room, then no observation should be made
    if (cover.empty()) {
        state.wifiaps.clear();
        state.times.clear();
        return;
    }

    // Set lastWiFiAP if not already set
    if (state.wifiaps.empty())
        state.lastWiFiAP = RandomSelector<SensorID>{cover}.select();

    // Clear previously stored state(s)
    state.wifiaps.clear();
    state.times.clear();

    // Staying in a space
    if (endDT - startDT > mins5) {
        for (DateTime dt{startDT}; dt <= endDT; dt += connStep.sample()) {
            state.wifiaps.push_back(state.lastWiFiAP);
            state.times.push_back(dt);
        }
        state.lastTime = state.times.back();
    }

    // Moving through a space
    else {

        // Not within range of last wifiap; connect to new wifiap 
        auto it = cover.find(state.lastWiFiAP);
        if (it == cover.end()) {
            state.lastWiFiAP = RandomSelector<SensorID>{cover}.select();
            state.wifiaps.push_back(state.lastWiFiAP);

            int nSecs = randInt((endDT - startDT).count());
            state.lastTime = DateTime{startDT + Time{nSecs}};
            state.times.push_back(state.lastTime);
        }

        // Within range of last wifiap; connect if time > mins5
        else if (startDT - state.lastTime > mins5) {
            state.wifiaps.push_back(state.lastWiFiAP);

            int nSecs = randInt((endDT - startDT).count());
            state.lastTime = DateTime{startDT + Time{nSecs}};
            state.times.push_back(state.lastTime);
        }

    }

}

void WiFiObservationGenerator::recordState(const Person& p) {
    const State& state = statesMap[p.id];
    for (int i = 0; i < state.wifiaps.size(); ++i) {
        out << p.id << "," 
            << state.times[i] << "," 
            << state.wifiaps[i] << std::endl;
        coutlog << "Person " << p.id
                << " at datetime " << state.times[i]
                << " connects to " << state.wifiaps[i] << std::endl;
    }
}

// Nothing else to record
void WiFiObservationGenerator::recordRest() {}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// I/O

void WiFiObservationGenerator::writeHeader() {
    out << "PersonID,DateTime,WiFiAP" << std::endl;
    coutlog << "WiFi Observation data from metasensor " << ms.id << std::endl;
}

#endif // SENSOR_OBSERVATION_GENERATOR_WIFIOBSERVATIONGENERATOR_HPP
