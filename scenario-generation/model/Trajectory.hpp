#ifndef MODEL_TRAJECTORY_HPP
#define MODEL_TRAJECTORY_HPP

#include <iostream>

#include "../utils/Typedefs.hpp"
#include "../utils/DateUtils.hpp"

class Trajectory {
public:

    // Attributes
    SpaceIDList traj;
    TimeList delta;

    int size() const;
    Time totalTime() const;

    explicit operator bool() const;

    friend std::ostream& operator<<(std::ostream& oss, const Trajectory& t);

};

Time Trajectory::totalTime() const {
    Time total{0};
    for (const Time& t : delta)
        total += t;
    return total;
}


int Trajectory::size() const { return traj.size(); }
Trajectory::operator bool() const { return delta.empty(); }

std::ostream& operator<<(std::ostream& oss, const Trajectory& t) {
    oss << "<";
    if (!t) {
        typename SpaceIDList::const_iterator it = t.traj.begin();
        oss << *it++;
        for (; it != t.traj.end(); ++it)
            oss << ", "  << *it;
    }
    oss << ">";
    return oss;
}

#endif // MODEL_TRAJECTORY_HPP
