#ifndef SENSOR_OBSERVATION_GENERATOR_WATERUSAGEOBSERVATIONGENERATOR_HPP
#define SENSOR_OBSERVATION_GENERATOR_WATERUSAGEOBSERVATIONGENERATOR_HPP

#include <iostream>
#include <string>
#include <map>

#include "../utils/Typedefs.hpp"
#include "../utils/DateUtils.hpp"
#include "../utils/IOUtils.hpp"
#include "../model/Sensor.hpp"
#include "../model/Event.hpp"
#include "../model/Person.hpp"

#include "UsageObservationGenerator.hpp"

class WaterUsageObservationGenerator : public UsageObservationGenerator {
public:
    
    // Default Constructor
    WaterUsageObservationGenerator();

    // A struct to store the current state of the sensor. It is stated here for
    // consistency between other observation generators, but is not needed as
    // the parent class stores an interval map
    struct State {
        boost::icl::interval_map<long, double> usage;
    };

    // A map storing the state of each sensor
    std::map<SensorID, State> statesMap;

    // Methods to override to update and record the state
    void updateState(const Sensor& s, const AttendanceEntry& ae) override;
    void recordState(const Sensor& s) override;

    // Utility methods to help with adding usage to interval map
    void addUsage(const DateTime& start, const DateTime& end, 
            const Sensor& s, double amt);

    // I/O
    void writeHeader() override;

};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Constructor

WaterUsageObservationGenerator::WaterUsageObservationGenerator() {}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// State management

void WaterUsageObservationGenerator::updateState(
        const Sensor& s, const AttendanceEntry& ae) {
    boost::icl::interval_map<long, double>& usage = statesMap[s.id].usage;

    // Unpack attendance entry ae
    PersonID        pid = std::get<0>(ae);
    EventID         eid = std::get<1>(ae);
    const DateTime& sdt = std::get<2>(ae);
    const DateTime& edt = std::get<3>(ae);

    // For demo, we assume that people use water every N(30, 5) minutes, for
    // N(3, 0.5) minutes
    static NormalTime usageTimestep{"00:30:00", "00:05:00"};
    static NormalTime usageDuration{"00:03:00", "00:00:30"};
    
    // Determine time and amount of usage
    for (DateTime dt{sdt}; dt <= edt; dt += usageTimestep.sample()) {
        double amt = 1;
        addUsage(dt, DateTime(dt+usageDuration.sample()), s, amt);
    }

}

void WaterUsageObservationGenerator::recordState(const Sensor& s) {
    const State& state = statesMap[s.id];
    const boost::icl::interval_map<long, double>& usage = state.usage;

    for (const auto& intr : usage) {
        DateTime dt{intr.first.lower()};
        out << s.id << "," << dt << "," << intr.second << std::endl;
        coutlog << "Sensor " << s.id 
                << " at datetime " << dt 
                << " recorded value " << intr.second << std::endl;
    }

}

void WaterUsageObservationGenerator::addUsage(
        const DateTime& start, const DateTime& end, 
        const Sensor& s, double amt) {
    statesMap[s.id].usage += std::make_pair(
        boost::icl::interval<long>::right_open(start.count(), end.count()),
        amt
    );
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// I/O

void WaterUsageObservationGenerator::writeHeader() {
    out << "SensorID,DateTime,WaterUsage" << std::endl;
    coutlog << "WaterUsage data from metasensor " << ms.id << std::endl;
}

#endif // SENSOR_OBSERVATION_GENERATOR_WATERUSAGEOBSERVATIONGENERATOR_HPP
