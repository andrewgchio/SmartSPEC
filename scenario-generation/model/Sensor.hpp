#ifndef MODEL_SENSOR_HPP
#define MODEL_SENSOR_HPP

#include <iostream>

#include "../utils/Typedefs.hpp"
#include "../utils/IOUtils.hpp"

class Sensor {
public:

    // Attributes
    SensorID id;
    Description desc;
    Mobility mob;
    Coverage cov;
    Interval intr;

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
        << "desc=\"" << s.desc << "\", "
        << "mob=" << s.mob << ", "
        << "cov=" << s.cov << ", "
        << "intr=" << s.intr
        << ")";
    return oss;
}

#endif // MODEL_SENSOR_HPP
