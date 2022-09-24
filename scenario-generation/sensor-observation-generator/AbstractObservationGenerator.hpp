#ifndef SENSOR_OBSERVATION_GENERATOR_ABSTRACTOBSERVATIONGENERATOR_HPP
#define SENSOR_OBSERVATION_GENERATOR_ABSTRACTOBSERVATIONGENERATOR_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "../utils/Typedefs.hpp"
#include "../model/MetaSensor.hpp"
#include "../dataloader/DataLoader.hpp"

// A top-level class that all general sensors should inherit from. It provides
// basic I/O support for derived classes. Users should derive from this
// class' direct children, which should provide better support for the
// operations necessary in generating observations. 
class AbstractObservationGenerator {
public:

    // Constructor. The derived class should only define a default constructor. 
    AbstractObservationGenerator();

    // Virtual Destructor. Declared since the class is an abstract base class.
    virtual ~AbstractObservationGenerator();

    // The method generateObservations() implements the general loop structure
    // necessary for generating sensor observations, and calls the appropriate
    // methods for updating the state of the sensor observation generator and
    // recording values. 
    virtual void generateObservations() = 0;

    // The method initializeOutputFiles() creates both observation files and 
    // observation log files in the output directory specified in the
    // dataloader. The function writeHeader() is called once when output files
    // are initalized, and must be defined by the derived class. 
    void initializeOutputFiles(); 
    virtual void writeHeader() = 0;
    
    // The filestream `out` is responsible for storing all observation outputs
    // for a specific metasensor. It is initialized after calling the method
    // initializeOutputFiles(). Note that the derived class must output CSV
    // values to this file manually.
    std::ofstream out;

    // The filestream `log` is responsible for storing all observation log 
    // outputs for a specific metasensor. It is initialized after calling the 
    // method initializeOutputFiles(). Note: the derived class is encouraged to
    // use the TeeStream `coutlog` instead to output to both filestreams 
    // `std::cout` and `log`.
    std::ofstream log;

    // A stream to tee to the observation log file and std::cout
    // The tee stream `coutlog` is a stream that will tee its input to both 
    // `std::cout` and the filestream `log`. The method initializeOutputFiles()
    // should be called first before this stream can be used. 
    TeeStream coutlog;

    // The method computeUtilityDataStructures() is used to initialize any 
    // data structures needed by derived classes. 
    virtual void computeUtilityDataStructures();

    // The method setDataLoader() saves a pointer to the dataloader dl for use
    // in derived classes
    void setDataLoader(const DataLoader& dl);

    // The method setMetaSensor() saves a reference to the metasensor for which
    // to generate observations.
    void setMetaSensor(const MetaSensor& ms);

protected:

    // A pointer to the dataloader, in case any configuration values are needed
    const DataLoader* dl;

    // MetaSensor ms to model
    MetaSensor ms;

};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Constructors / Destructors

// Constructor. Initialize coutlog to tee cout into log. 
AbstractObservationGenerator::AbstractObservationGenerator() 
    : coutlog{TeeDevice{std::cout, log}}
{}

// Destructor. flush() and close() all open streams.
AbstractObservationGenerator::~AbstractObservationGenerator() {
    out.flush();
    log.flush();
    coutlog.flush();

    out.close();
    log.close();
    coutlog.close();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Private helper methods

void AbstractObservationGenerator::setDataLoader(const DataLoader& dl) 
{ this->dl = &dl; }

void AbstractObservationGenerator::setMetaSensor(const MetaSensor& ms) 
{ this->ms = ms; }

void AbstractObservationGenerator::initializeOutputFiles() {
    const Filename& outdir = dl->config("filepaths", "output");
    std::ostringstream oss;
    oss << outdir << "obs_msid_" << ms.id << ".csv";
    out.open(oss.str(), std::ios_base::app);

    oss.str("");
    oss << outdir << "obs_log_msid_" << ms.id << ".csv";
    log.open(oss.str(), std::ios_base::app);

    writeHeader();
}

void AbstractObservationGenerator::computeUtilityDataStructures() {}

#endif // SENSOR_OBSERVATION_GENERATOR_ABSTRACTOBSERVATIONGENERATOR_HPP
