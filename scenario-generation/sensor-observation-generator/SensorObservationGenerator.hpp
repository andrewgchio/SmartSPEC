#ifndef SENSOR_OBSERVATION_GENERATOR_SENSOROBSERVATIONGENERATOR_HPP
#define SENSOR_OBSERVATION_GENERATOR_SENSOROBSERVATIONGENERATOR_HPP

#include "../utils/Typedefs.hpp"

#include "../dataloader/DataLoader.hpp"

#include "SOGFactory.hpp"

class SensorObservationGenerator {
public:

    // Constructors
    explicit SensorObservationGenerator(const DataLoader& dl);

    // The main method to generate synthetic sensor observations
    void generateObservations();

private:

    // Dataloader with all scenario data
    DataLoader dl;

};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Constructors / Destructors

// Reads the generated output
SensorObservationGenerator::SensorObservationGenerator(const DataLoader& dl) 
    : dl{dl}
{}

// The main method to generate sensor observations
void SensorObservationGenerator::generateObservations() {
    for (const MetaSensor& ms : dl.MS) {
        auto sog = SOGFactory::instance().make(ms.id);
        sog->setDataLoader(dl);
        sog->setMetaSensor(ms);
        sog->initializeOutputFiles();
        sog->computeUtilityDataStructures();
        sog->generateObservations();
    }
}

#endif // SENSOR_OBSERVATION_GENERATOR_SENSOROBSERVATIONGENERATOR_HPP
