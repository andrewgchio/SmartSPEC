#ifndef SENSOR_OBSERVATION_GENERATOR_OCCUPANCYOBSERVATIONGENERATOR_HPP
#define SENSOR_OBSERVATION_GENERATOR_OCCUPANCYOBSERVATIONGENERATOR_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>  
#include <tuple>

#include "../utils/Typedefs.hpp"
#include "../utils/DateUtils.hpp"

#include "../model/Person.hpp"
#include "../model/Sensor.hpp"

#include "AbstractObservationGenerator.hpp"

class OccupancyObservationGenerator : public AbstractObservationGenerator {
public:

    // Default Constructor.
    OccupancyObservationGenerator();

    void generateObservations() override;

    virtual void updateState(const Person& p, const TrajectoryEntry& te) = 0;
    virtual void recordState(const Person& p) = 0;
    virtual void recordRest() = 0;

    void computeUtilityDataStructures() override;

    // To aid in the generation process, the trajectories of people in spaces 
    // over time is precomputed.
    void computeTrajectoryMap();
    const TrajectoryList& getTrajectory(const Person& p) const;
    std::map<PersonID, TrajectoryList> trajectory;

    // As well as the sensor coverage for each space
    void computeCoverageMap(); 
    const SensorIDSet& getCoverage(const Space& c) const;
    std::map<SpaceID, SensorIDSet> coverage;

};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Constructor

// Default constructor.
OccupancyObservationGenerator::OccupancyObservationGenerator() {}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Observation Generation loop structure

void OccupancyObservationGenerator::generateObservations() {
    
    // Loop over each simulated person
    for (PersonID pid : dl->P.getIDs()) {
        const Person& p = dl->P[pid];

        // Get the person's trajectory over time
        const TrajectoryList& traj = getTrajectory(p);

        // For each entry in the trajectory
        for (const TrajectoryEntry& te : traj) {
            // Update and record state for covering sensors
            updateState(p, te);
            recordState(p);
        }
    }

    recordRest();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Utility: Trajectory Computation

void OccupancyObservationGenerator::computeUtilityDataStructures() { 
    computeTrajectoryMap(); 
    computeCoverageMap(); 
}

void OccupancyObservationGenerator::computeTrajectoryMap() {
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

        trajectory[std::stoi(pid)].push_back(std::make_tuple(
                std::stoi(cid), DateTime(sdt), DateTime(edt)));
    }
}

const TrajectoryList& OccupancyObservationGenerator::getTrajectory(
        const Person& p) const {
    static TrajectoryList empty;

    const auto& it = trajectory.find(p.id);
    if (it == trajectory.end())
        return empty;

    return it->second;
}

void OccupancyObservationGenerator::computeCoverageMap() {
    for (SensorID sid : dl->S.getIDs(ms.id)) {
        for (SpaceID cid : dl->S[sid].cov) {
            coverage[cid].insert(sid);
        }
    }
}

const SensorIDSet& OccupancyObservationGenerator::getCoverage(
        const Space& c) const {
    static SensorIDSet empty;

    const auto& it = coverage.find(c.id);
    if (it == coverage.end())
        return empty;

    return it->second;
}

#endif // SENSOR_OBSERVATION_GENERATOR_OCCUPANCYOBSERVATIONGENERATOR_HPP
