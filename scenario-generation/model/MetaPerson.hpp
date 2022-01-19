#ifndef MODEL_METAPERSON_HPP
#define MODEL_METAPERSON_HPP

#include <iostream>

#include "../utils/Typedefs.hpp"
#include "../utils/IOUtils.hpp"
#include "../utils/TimeProfile.hpp"

class MetaPerson {
public:

    // Attributes
    MetaPersonID id;
    Probability pr;
    Description desc;
    TimeProfileList tps;
    ProbabilityList tpsPrs;
    EventAffinity aff;

    friend std::ostream& operator<<(std::ostream& oss, const MetaPerson& mp);
};

std::ostream& operator<<(std::ostream& oss, const MetaPerson& mp) {
    oss << "MetaPerson("
        << "id=" << mp.id << ", "
        << "desc=\"" << mp.desc << "\", "
        << "pr=" << mp.pr << ", "
        << "tps=" << mp.tps << ", "
        << "tpsPrs=" << mp.tpsPrs << ", "
        << "aff=" << mp.aff
        << ")";
    return oss;
}

#endif // MODEL_METAPERSON_HPP
