#ifndef SENSOR_OBSERVATION_GENERATOR_MOTIONDETECTOROBSERVATIONGENERATOR_HPP
#define SENSOR_OBSERVATION_GENERATOR_MOTIONDETECTOROBSERVATIONGENERATOR_HPP

#include <iostream>
#include <string>
#include <map>

#include "../utils/Typedefs.hpp"
#include "../utils/DateUtils.hpp"
#include "../utils/IOUtils.hpp"
#include "../utils/RandomGenerator.hpp"
#include "../utils/NormalDistributions.hpp"

#include "../model/Space.hpp"
#include "../model/Sensor.hpp"
#include "../model/Person.hpp"

#include "../dataloader/DataLoader.hpp"

#include "OccupancyObservationGenerator.hpp"

class MotionDetectorObservationGenerator 
    : public OccupancyObservationGenerator {
public:

    // Default Constructor
    MotionDetectorObservationGenerator();

    struct State {
        std::vector<DateTime> times;
        std::vector<SensorID> detectors;
    };

    // A map storing the state of each space
    std::map<PersonID, State> statesMap;

    // Methods to override to update and record the state
    void updateState(const Person& p, const TrajectoryEntry& te) override;
    void recordState(const Person& p) override;

    // I/O
    void writeHeader() override;

};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Contstructor

// Default constructor
MotionDetectorObservationGenerator::MotionDetectorObservationGenerator() {}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// State management

void MotionDetectorObservationGenerator::updateState(
        const Person& p, const TrajectoryEntry& te) {

    // Threshold to determine moving or stationary
    static Time mins5{60*5};

    // Motion near start/end of an event
    static NormalTime motion{"00:05:00", "00:01:00"};

    // Probability that motion detector will capture movement
    static double OBS_PR = 0.7;
    
    // Unpack trajectory entry te
    const Space& c           = dl->C[std::get<0>(te)];
    const SensorIDSet& cover = getCoverage(c);
    const DateTime& startDT  = std::get<1>(te);
    const DateTime& endDT    = std::get<2>(te);

    // Get the person's state
    State& state = statesMap[p.id];

    // Clear previously stored states
    state.detectors.clear();
    state.times.clear();

    // If no sensor observes the space, then no observation should be made
    if (cover.empty()) {
        return;
    }

    // Staying in a space
    if (endDT - startDT < mins5) {
        // All covering sensors have OBS_PR chance of detecting towards the 
        // beginning and end of the event
        for (SensorID sid : cover) {
            if (getRand() < OBS_PR) {
                // Motion near start of event
                state.detectors.push_back(sid);
                state.times.push_back(DateTime{startDT + motion.sample()});

                // Motion near end of event
                state.detectors.push_back(sid);
                state.times.push_back(DateTime{endDT - motion.sample()});
            }
        }
    }
    // Moving through a space
    else {
        // All covering sensors have OBS_PR chance of detecting
        for (SensorID sid : cover) {
            if (getRand() < OBS_PR) {
                // Sensor makes an observation
                state.detectors.push_back(sid);

                // Determine time of detection
                int nSecs = randInt((endDT - startDT).count());
                state.times.push_back(DateTime{startDT + Time{nSecs}});
            }
        }
    }

}

void MotionDetectorObservationGenerator::recordState(const Person& p) {
    const State& state = statesMap[p.id];
    for (int i = 0; i < state.detectors.size(); ++i) {
        out << state.times[i] << ","
            << state.detectors[i] << std::endl;
        coutlog << "Detector " << state.detectors[i]
                << " detected motion at " << state.times[i] << std::endl;
    }
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// I/O

void MotionDetectorObservationGenerator::writeHeader() {
    out << "SensorID,DateTime" << std::endl;
    coutlog << "Motion Detection data from metasensor " << ms.id << std::endl;
}

#endif // SENSOR_OBSERVATION_GENERATOR_MOTIONDETECTOROBSERVATIONGENERATOR_HPP
