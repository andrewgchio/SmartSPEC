#ifndef MODEL_METAEVENT_HPP
#define MODEL_METAEVENT_HPP

#include <iostream>

#include "../utils/Typedefs.hpp"
#include "../utils/IOUtils.hpp"
#include "../utils/TimeProfile.hpp"
#include "../utils/Selectors.hpp"

class MetaEvent {
public: 

    // Attributes
    MetaEventID id;
    Description desc;
    Probability pr;
    SpaceSelector selector;
    TimeProfileList tps;
    ProbabilityList tpsPrs;
    PersonCapRangeDistr cap;

    // I/O
    friend std::ostream& operator<<(std::ostream& oss, const MetaEvent& me);
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// I/O

// Print a metaevent
std::ostream& operator<<(std::ostream& oss, const MetaEvent& me) {
    oss << "MetaEvent("
        << "id=" << me.id << ", "
        << "desc=\"" << me.desc << "\", "
        << "pr=" << me.pr << ", "
        << "selector=" << me.selector << ", "
        << "tps=" << me.tps << ", "
        << "tpsPrs=" << me.tpsPrs << ", "
        << "cap=" << me.cap
        << ")";
    return oss;
}

#endif // MODEL_METAEVENT_HPP
