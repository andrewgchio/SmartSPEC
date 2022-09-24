#ifndef MODEL_METASENSOR_HPP
#define MODEL_METASENSOR_HPP

#include <iostream>

#include "../utils/Typedefs.hpp"
#include "../utils/IOUtils.hpp"

class MetaSensor {
public:

    // Attributes
    MetaSensorID id;
    Description desc;

    // For sensor observation generation
    SensorIDList sids;

    // I/O
    friend std::ostream& operator<<(std::ostream& oss, const MetaSensor& ms);
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// I/O

// Print a metasensor
std::ostream& operator<<(std::ostream& oss, const MetaSensor& ms) {
    oss << "MetaSensor(" 
        << "id=" << ms.id << ", "
        << "desc=" << ms.desc << ", "
        << "sids=" << ms.sids
        << ")";
    return oss;
}

#endif // MODEL_METASENSOR_HPP
