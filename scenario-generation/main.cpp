// main.cpp
//
// Main program for the scenario generation component of SmartSPEC
// 
// Compile: g++ -std=c++17 main.cpp -o main
// Run    : main <config-file>

#include <iostream>
#include <string>

#include "dataloader/DataLoader.hpp"
#include "synthetic-data-generator/SyntheticDataGenerator.hpp"

int main(int argc, char* argv[]) {

    // Load Appropriate Data
    DataLoader dl(argv[1]);
    
    // Print out data if desired
    // std::cout << dl << std::endl;

    // Note that new entities should have already been created
    dl.loadEvents();
    dl.loadPeople();

    // Generate synthetic data logs
    SyntheticDataGenerator sdg{dl};
    sdg.generateLogs();

    return 0;
}
