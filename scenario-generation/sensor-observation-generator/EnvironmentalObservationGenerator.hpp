#ifndef SENSOR_OBSERVATION_GENERATOR_ENVIRONMENTALOBSERVATIONGENERATOR_HPP
#define SENSOR_OBSERVATION_GENERATOR_ENVIRONMENTALOBSERVATIONGENERATOR_HPP

#include <iostream>
#include <fstream>
#include <string>

#include <boost/icl/interval_map.hpp>

#include "../utils/Typedefs.hpp"
#include "../utils/DateUtils.hpp"
#include "../model/Sensor.hpp"
#include "../dataloader/DataLoader.hpp"

#include "AbstractObservationGenerator.hpp"

class EnvironmentalObservationGenerator : public AbstractObservationGenerator {
public:

    // Default Constructor.
    EnvironmentalObservationGenerator();

    // The method generateObservations() loops over each sensor and calls 
    // updateState(), recordState() over datetimes incremented by step(). 
    void generateObservations() override;

    // The method updateState() updates the current state of the sensor
    // observation generator. This mainly enails computing any observation
    // values to produce as output and storing it in an internal state in the
    // derived class.
    virtual void updateState(const Sensor& s, const DateTime& dt) = 0;

    // The method record() saves the 3-tuple (sid, dt, value) into the 
    // `out` filestream and outputs a user-readable message to `coutlog`
    virtual void recordState(const Sensor& s) = 0;

    // The method step() determines the periodicity at which sensor observation
    // will be made. The default timestep is 1 minute.
    virtual Time step();

    // The method computeUtilityDataStructures() initializes the attendance map
    void computeUtilityDataStructures() override;

    // To aid in the generation process, the attendance of people in spaces 
    // over time is precomputed. 
    void computeAttendanceMap();
    const PersonIDSet& getAttendance(SpaceID cid, const DateTime& dt) const;
    std::map<SpaceID, boost::icl::interval_map<long, PersonIDSet>> attendance;

};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Constructor

// Default constructor.
EnvironmentalObservationGenerator::EnvironmentalObservationGenerator() {}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Observation Generation loop structure

void EnvironmentalObservationGenerator::generateObservations() {
    
    static date::days day1{1};

    // Loop over each simulated day
    for (date::sys_days d{dl->start}; d <= dl->end; d += day1) {
        coutlog << "=======================" << std::endl;
        coutlog << "Starting day " << d << std::endl;
        coutlog << "=======================" << std::endl;

        // Generate sensor observations until end of day
        DateTime endOfDay = DateTime{d}.lastTime(); 

        // Iterate through all sensors with the appropriate metasensor id
        for (SensorID sid : dl->S.getIDs(ms.id)) {
            const Sensor& s = dl->S[sid];
            
            // Periodically update and record state
            for (DateTime curr{d}; curr <= endOfDay; curr += step()) {
                updateState(s, curr);
                recordState(s);
            }
        }
    }
}

Time EnvironmentalObservationGenerator::step() { 
    static Time min1{60}; // 1 minute
    return min1; 
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Utility: Attendance Computation

void EnvironmentalObservationGenerator::computeUtilityDataStructures() 
{ computeAttendanceMap(); }

void EnvironmentalObservationGenerator::computeAttendanceMap() {
    std::ifstream file{dl->config("filepaths", "output") + "data.csv"};
    std::string pid, cid, sdt, edt, extra;

    // Skip first line (header)
    std::getline(file, extra);

    // Read remainder of file
    while (std::getline(file, pid, ',')) {
        std::getline(file, extra, ','); // eid
        std::getline(file, cid, ',');
        std::getline(file, sdt, ',');
        std::getline(file, edt);

        attendance[std::stoi(cid)].add(
                std::make_pair(
                    boost::icl::interval<long>::right_open(
                        DateTime(sdt).count(), 
                        DateTime(edt).count()), 
                    PersonIDSet{std::stoi(pid)}
                ));
    }
}

const PersonIDSet& EnvironmentalObservationGenerator::getAttendance(
        SpaceID cid, const DateTime& dt) const {
    static PersonIDSet empty;

    const auto& cit = attendance.find(cid);
    if (cit == attendance.end()) 
        return empty;

    const auto& pit = cit->second.find(dt.count());
    if (pit == cit->second.end()) 
        return empty;

    return pit->second;
}

#endif // SENSOR_OBSERVATION_GENERATOR_ENVIRONMENTALOBSERVATIONGENERATOR_HPP
