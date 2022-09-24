#ifndef SENSOR_OBSERVATION_GENERATOR_TEMPERATUREOBSERVATIONGENERATION_HPP
#define SENSOR_OBSERVATION_GENERATOR_TEMPERATUREOBSERVATIONGENERATION_HPP

#include <iostream>
#include <string>
#include <map>

#include "../utils/Typedefs.hpp"
#include "../utils/DateUtils.hpp"
#include "../utils/IOUtils.hpp"
#include "../model/Sensor.hpp"

#include "EnvironmentalObservationGenerator.hpp"

class TemperatureObservationGenerator 
    : public EnvironmentalObservationGenerator {
public:

    // Default Constructor
    TemperatureObservationGenerator();

    // A struct to store the current state of the sensor
    struct State {
        DateTime dt;
        double value = 70; // Set initial temperature to 70
    };

    // A map storing the state of each sensor
    std::map<SensorID, State> statesMap;

    // Methods to override to update and record the state
    void updateState(const Sensor& s, const DateTime& dt) override;
    void recordState(const Sensor& s) override;

    // Override timestep to produce values with different periodicity
    Time step() override;

    // I/O
    void writeHeader() override;

};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Constructors

// Default constructor.
TemperatureObservationGenerator::TemperatureObservationGenerator() {}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// State management

void TemperatureObservationGenerator::updateState(
        const Sensor& s, const DateTime& dt) {

    // Count number of people in spaces covered by sensor at dt
    int nPeople = 0;
    for (SpaceID cid : s.cov) {
        nPeople += getAttendance(cid, dt).size();
    }

    // Update the expected current temperature
    statesMap[s.id].value = 0.4 * statesMap[s.id].value + 
                            0.6 * (70+0.25*nPeople);
    statesMap[s.id].dt    = dt;
}

void TemperatureObservationGenerator::recordState(const Sensor& s) {
    const State& state = statesMap[s.id];
    out << s.id << "," << state.dt << "," << state.value << std::endl;
    coutlog << "Sensor " << s.id 
            << " at datetime " << state.dt 
            << " records value = " << state.value << std::endl;
}

Time TemperatureObservationGenerator::step() { 
    static Time mins15{60*15}; // 15 minute intervals
    return mins15; 
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// I/O

void TemperatureObservationGenerator::writeHeader() {
    out << "SensorID,DateTime,Temperature" << std::endl;
    coutlog << "Temperature data from metasensor " << ms.id << std::endl;
}

#endif // SENSOR_OBSERVATION_GENERATOR_TEMPERATUREOBSERVATIONGENERATION_HPP
