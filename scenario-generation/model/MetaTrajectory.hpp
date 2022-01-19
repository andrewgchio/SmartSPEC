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

    friend std::ostream& operator<<(std::ostream& oss, 
                                    const MetaTrajectory& mt);

};

std::ostream& operator<<(std::ostream& oss, const MetaTrajectory& mt) {
    oss << "MetaTrajectory("
        << "src/dest=" << mt.sd << ", "
        << "TODO: MT::operator<<)";
    return oss;
}

#endif // MODEL_METATRAJECTORY_HPP
