// main.cpp
//
// Main program for the scenario generation component of SmartSPEC
// 
// Compile: g++ -std=c++17 main.cpp -o main
// Run    : main <config-file>

#include <iostream>
#include <string>

#include "dataloader/DataLoader.hpp"
#include "entity-generator/EntityGenerator.hpp"
#include "synthetic-data-generator/SyntheticDataGenerator.hpp"

int main(int argc, char* argv[]) {

    // Load Appropriate Data
    DataLoader dl(argv[1]);
    
    // Print out data if desired
    // std::cout << dl << std::endl;

    // Generate Events and People according to the config file
    EntityGenerator::generateEvents(
            dl.ME,
            dl.E, 
            std::stoi(dl.config("events","number", "0")),
            dl.config("events","generation"),
            dl.config("filepaths","events")
    );
    EntityGenerator::generatePeople(
            dl.MP,
            dl.P, 
            std::stoi(dl.config("people","number", "0")),
            dl.config("people","generation"),
            dl.config("filepaths","people")
    );

    // Generate synthetic data logs
    SyntheticDataGenerator sdg{dl};
    sdg.generateLogs();

    return 0;
}
