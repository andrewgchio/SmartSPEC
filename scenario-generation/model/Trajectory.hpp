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

    // Queries
    int size() const;
    Time totalTime() const;
    bool empty() const;
    SpaceID dest() const;

    explicit operator bool() const;

    SpaceID operator[](int i);
    SpaceID operator[](int i) const;

    // I/O
    friend std::ostream& operator<<(std::ostream& oss, const Trajectory& t);

};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Queries

// Return the length of the trajectory
int Trajectory::size() const { return traj.size(); }

// Return the total time taken for the trajectory
Time Trajectory::totalTime() const {
    Time total{0};
    for (const Time& t : delta)
        total += t;
    return total;
}

// Return whether the trajectory is empty
bool Trajectory::empty() const { return traj.empty(); }

// Return the destination space of the trajectory
SpaceID Trajectory::dest() const { return traj.back(); }

// Return whether the trajectory is null (empty)
Trajectory::operator bool() const { return delta.empty(); }

// Return the ith space id in the trajectory
SpaceID Trajectory::operator[](int i) { return traj[i]; }

// Return the ith space id in the trajectory (const)
SpaceID Trajectory::operator[](int i) const { return traj[i]; }

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// I/O

// Print a trajectory
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
