// obsgen.cpp
//
// Generate observation data using the produced synthetic logs
// 
// Compile: g++ -std=c++17 obsgen.cpp -o obsgen
// Run    : obsgen <config-file>

#include <iostream>
#include <string>

#include "dataloader/DataLoader.hpp"
#include "sensor-observation-generator/SensorObservationGenerator.hpp"

// Include SOGFactory.hpp for registration macros
#include "sensor-observation-generator/SOGFactory.hpp"

// Register all sensor observation generators for each metasensor as follows:
// REGISTER_OBSERVATION_GENERATOR(ClassName, MetaSensorID)

#include "sensor-observation-generator/WiFiObservationGenerator.hpp"
REGISTER_OBSERVATION_GENERATOR(WiFiObservationGenerator, 1);

#include "sensor-observation-generator/DoorObservationGenerator.hpp"
REGISTER_OBSERVATION_GENERATOR(DoorObservationGenerator, 2);

#include "sensor-observation-generator/TemperatureObservationGenerator.hpp"
REGISTER_OBSERVATION_GENERATOR(TemperatureObservationGenerator, 3);

int main(int argc, char* argv[]) {

    // Load Appropriate Data
    DataLoader dl(argv[1]);
    dl.loadEvents();
    dl.loadPeople();

    // Print out data if desired
    // std::cout << dl << std::endl;

    // Generate sensor observation data
    SensorObservationGenerator sog{dl};
    sog.generateObservations();

    return 0;
}
