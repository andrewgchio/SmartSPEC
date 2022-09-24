#ifndef MODEL_SENSOR_HPP
#define MODEL_SENSOR_HPP

#include <iostream>

#include "../utils/Typedefs.hpp"
#include "../utils/IOUtils.hpp"

class Sensor {
public:

    // Attributes
    SensorID id;
    MetaSensorID mid;
    Description desc;
    Coordinates coords;
    Coverage cov;

    // I/O
    friend std::ostream& operator<<(std::ostream& oss, const Sensor& s);
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// I/O

// Print a sensor
std::ostream& operator<<(std::ostream& oss, const Sensor& s) {
    oss << "Sensor("
        << "id=" << s.id << ", "
        << "msid=" << s.mid << ", "
        << "desc=\"" << s.desc << "\", "
        << "coords=" << s.coords << ", "
        << "cov=" << s.cov
        << ")";
    return oss;
}

#endif // MODEL_SENSOR_HPP
