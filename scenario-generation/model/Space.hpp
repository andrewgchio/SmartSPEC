#ifndef MODEL_SPACE_HPP
#define MODEL_SPACE_HPP

#include <iostream>
#include <utility>

#include <boost/icl/interval_map.hpp>

#include "../utils/Typedefs.hpp"
#include "../utils/DateUtils.hpp"
#include "../utils/IOUtils.hpp"

class Space {
public:

    // Attributes
    SpaceID id;
    Description desc;
    Coordinates coords;
    MaxCap cap;
    SpaceIDList neighbors;

    // Queries
    int getOccupancy(const DateTime& dt) const;
    DateTime getNextOpenTime(const DateTime& dt) const;

    // Modifiers
    void insertOccupancy(const DateTime& s, const DateTime& e);

    // I/O
    friend std::ostream& operator<<(std::ostream& oss, const Space& c);

private:

    // Synthetic Data Generation
    boost::icl::interval_map<long, int> occ;
    
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Queries

// Return the occupancy of the space at the given datetime
int Space::getOccupancy(const DateTime& dt) const {
    auto it = occ.find(dt.count());
    return it != occ.end() ? it->second : 0;
}

// Returns the next time that the space is open, or the default datetime if 
// no such time exists on the day
DateTime Space::getNextOpenTime(const DateTime& dt) const {
    DateTime next{dt};
    const DateTime& last = dt.lastTime();
    while (next <= last) {
        if (cap == -1 || getOccupancy(next) <= cap)
            return next;
        next += Time{1};
    }
    return DateTime{};
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Modifiers

// Record that a person will occupy a space between the given datetimes
void Space::insertOccupancy(const DateTime& s, const DateTime& e) {
    occ.add(std::make_pair(
            boost::icl::interval<long>::right_open(s.count(), e.count()),
            1));
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// I/O

// Print a space
std::ostream& operator<<(std::ostream& oss, const Space& c) {
    oss << "Space("
        << "id=" << c.id << ", "
        << "desc=\"" << c.desc << "\", "
        << "cap=" << c.cap << ", "
        << "coords=" << c.coords << ", "
        << "neighbors=" << c.neighbors
        << ")";
    return oss;
}

#endif // MODEL_SPACE_HPP
