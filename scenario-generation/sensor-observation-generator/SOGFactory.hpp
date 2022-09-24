#ifndef SENSOR_OBSERVATION_GENERATOR_SOGFACTORY_HPP
#define SENSOR_OBSERVATION_GENERATOR_SOGFACTORY_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <type_traits>
#include <typeinfo>
#include <functional>

#include "AbstractObservationGenerator.hpp"

// For ease of use
using AbstractSOG = AbstractObservationGenerator;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// SOGInfo 

// A SensorObservationGenerator must be defined with a unique MetaSensorID and
// a function to create an instance of the generator
struct SOGInfo {
    MetaSensorID msid;
    std::function<std::unique_ptr<AbstractSOG>()> builder;
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// SOGFactory 

class SOGFactory {
public:

    // Singleton instance
    static SOGFactory& instance();

    // Accessors
    const std::vector<std::shared_ptr<SOGInfo>>& allSOGs() const;

    // Modifiers
    template <typename SOGClass>
    void registerSOG(MetaSensorID msid);

    // Creation
    std::unique_ptr<AbstractSOG> make(MetaSensorID msid) const;

private:

    // Private Constructor
    SOGFactory();

    // List of registered SOGs
    std::vector<std::shared_ptr<SOGInfo>> registered;

    // Map of registered SOGs by MetaSensorIDs
    std::map<MetaSensorID, std::shared_ptr<SOGInfo>> registeredByID;

};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Constructors and singleton instance 

// Default Constructor
SOGFactory::SOGFactory() {}

// Singleton instance
SOGFactory& SOGFactory::instance() {
    static SOGFactory factoryInstance;
    return factoryInstance;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Accessors

// Return a list of all registered sensor observation generators
const std::vector<std::shared_ptr<SOGInfo>>& SOGFactory::allSOGs() const
{ return registered; }

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Modifiers

// Function to register a sensor observation generator; it is called by the 
// SOGFactoryRegistration constructor
template <typename SOGClass>
void SOGFactory::registerSOG(MetaSensorID msid) {
    // Ensure that the SOGClass is derived from AbstractObservationGenerator
    static_assert(
        std::is_base_of<AbstractSOG, SOGClass>::value,
        "Error: Class must derive from AbstractObservationGenerator");

    // A duplicate SOG cannot be registered
    auto it = registeredByID.find(msid);
    if (registeredByID.find(msid) != registeredByID.end()) {
        std::cerr << "Error: Duplicate factory registration" << std::endl;
        std::exit(1);
    }

    // Create registration info
    std::shared_ptr<SOGInfo> newSOG{new SOGInfo};
    newSOG->msid = msid;
    newSOG->builder = [=]() {
        return std::unique_ptr<SOGClass>{new SOGClass};
    };

    // Add to registered
    registered.push_back(newSOG);
    registeredByID[msid] = newSOG;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Creation

// Make an instance of the sensor observation generator of the given type
std::unique_ptr<AbstractSOG> SOGFactory::make(MetaSensorID msid) const {
    // Search for the registered SOG by msid
    auto it = registeredByID.find(msid);
    if (it == registeredByID.end()) {
        std::cerr << "Error: Unregistered id: " << msid << std::endl;
        std::exit(1);
    }

    // Return the new SOG
    return it->second->builder();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Registration

template <typename SOGClass>
class SOGFactoryRegistration {
public:

    // Constructor
    explicit SOGFactoryRegistration(MetaSensorID msid);

};

// Constructor; register the provided Sensor Observation Generator
template <typename SOGClass>
SOGFactoryRegistration<SOGClass>::SOGFactoryRegistration(MetaSensorID msid) {
    SOGFactory::instance().registerSOG<SOGClass>(msid);
}

// Macro to register a sensor observation generator. Registration requires the
// name of the observation generator class and the associated MetaSensor ID. 
// Register a Sensor Observation Generator using the following in a .cpp file
// 
//   REGISTER_OBSERVATION_GENERATOR(NameOfClass, MetaSensorID);
// 
// Note, that duplicate registrations with the same MetaSensor ID are not 
// allowed. 
#define REGISTER_OBSERVATION_GENERATOR(SOGClass, msid) \
        SOGFactoryRegistration<SOGClass> SOGClass##__COUNTER__(msid);

#endif // SENSOR_OBSERVATION_GENERATOR_SOGFACTORY_HPP
