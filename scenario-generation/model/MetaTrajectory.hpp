#ifndef MODEL_METATRAJECTORY_HPP
#define MODEL_METATRAJECTORY_HPP

#include <iostream>
#include <vector>

#include "../utils/Typedefs.hpp"

#include "Trajectory.hpp"

class MetaTrajectory {
public:

    // Attributes
    SrcDest sd;
    std::vector<Trajectory> trajs;

    // I/O
    friend std::ostream& operator<<(std::ostream& oss, 
                                    const MetaTrajectory& mt);

};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// I/O

// Print a metatrajectory
std::ostream& operator<<(std::ostream& oss, const MetaTrajectory& mt) {
    oss << "MetaTrajectory("
        << "src/dest=" << mt.sd << ", "
        << "TODO: MT::operator<<)";
    return oss;
}

#endif // MODEL_METATRAJECTORY_HPP
