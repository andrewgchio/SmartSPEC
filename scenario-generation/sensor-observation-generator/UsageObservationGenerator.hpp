#ifndef SENSOR_OBSERVATION_GENERATOR_USAGEOBSERVATIONGENERATOR_HPP
#define SENSOR_OBSERVATION_GENERATOR_USAGEOBSERVATIONGENERATOR_HPP

#include <iostream>
#include <map>
#include <tuple>

#include "../utils/Typedefs.hpp"
#include "../utils/DateUtils.hpp"

#include "AbstractObservationGenerator.hpp"

class UsageObservationGenerator : public AbstractObservationGenerator {
public:

    // Default Constructor.
    UsageObservationGenerator();

    // The method generateObservations() 
    void generateObservations() override;

    virtual void updateState(const Sensor& s, const AttendanceEntry& ae) = 0;
    virtual void recordState(const Sensor& s) = 0;

    void computeUtilityDataStructures() override;

    void computeEventAttendanceMap();
    const AttendanceEntryList& getEventAttendance(const Space& c) const;
    std::map<SpaceID, AttendanceEntryList> eventAttendance;

};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Constructor

// Default constructor.
UsageObservationGenerator::UsageObservationGenerator() {}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Observation Generation loop structure

void UsageObservationGenerator::generateObservations() {

    // Iterate through all sensors with the appropriate metasensor id
    for (SensorID sid : dl->S.getIDs(ms.id)) {
        const Sensor& s = dl->S[sid];

        // For each space that contributes to the usage on the sensor
        for (SpaceID cid : s.cov) {
            const Space& c = dl->C[cid];

            // Determine usage from individual person's attendances
            for (const AttendanceEntry& att : getEventAttendance(c)) {
                updateState(s, att);
            }
        }

        // Record relevant usage amounts for sensor
        recordState(s);
    }
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Utility: Event Attendance

void UsageObservationGenerator::computeUtilityDataStructures() {
    computeEventAttendanceMap();
}

void UsageObservationGenerator::computeEventAttendanceMap() {
    std::ifstream file{dl->config("filepaths", "output") + "data.csv"};
    std::string pid, eid, cid, sdt, edt, extra;

    // Skip first line (header)
    std::getline(file, extra);

    // In general, we only record the attendance of the person in the event, 
    // not the entire trajectory of the person. We use the heuristic that
    // the last entry with the same (person id, event id) on the same day
    // dictates an actual event that was attended. 
    std::map<std::tuple<PersonID, EventID, Date>, 
             std::tuple<SpaceID, DateTime, DateTime>> lastAttendance;

    // Read remainder of file
    while (std::getline(file, pid, ',')) {
        std::getline(file, eid, ',');
        std::getline(file, cid, ',');
        std::getline(file, sdt, ',');
        std::getline(file, edt);

        lastAttendance[std::make_tuple(
                std::stoi(pid), std::stoi(eid), DateTime(sdt).date())] = 
            std::make_tuple(std::stoi(cid), DateTime(sdt), DateTime(edt));
    }

    for (const auto& entry : lastAttendance) {
        eventAttendance[std::get<0>(entry.second)].push_back( // cid key
            std::make_tuple(
                std::get<0>(entry.first),  // pid
                std::get<1>(entry.first),  // eid
                std::get<1>(entry.second), // sdt
                std::get<2>(entry.second)  // edt
        ));
    }
}

const AttendanceEntryList& UsageObservationGenerator::getEventAttendance(
        const Space& c) const {
    static AttendanceEntryList empty;

    const auto& it = eventAttendance.find(c.id);
    if (it == eventAttendance.end())
        return empty;

    return it->second;
}

#endif // SENSOR_OBSERVATION_GENERATOR_USAGEOBSERVATIONGENERATOR_HPP
