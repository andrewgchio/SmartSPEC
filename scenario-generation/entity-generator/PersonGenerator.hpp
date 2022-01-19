#ifndef ENTITY_GENERATOR_PERSONGENERATOR_HPP
#define ENTITY_GENERATOR_PERSONGENERATOR_HPP

#include <iostream>
#include <string>
#include <random>

#include "../model/Person.hpp"
#include "../model/MetaPerson.hpp"

#include "../dataloader/PeopleLoader.hpp"
#include "../dataloader/MetaPeopleLoader.hpp"

#include "../utils/Typedefs.hpp"
#include "../utils/RandomGenerator.hpp"

class PeopleGenerator {
public:

    static void generate(MetaPeopleLoader& MP, PeopleLoader& P, int n);

};

void PeopleGenerator::generate(MetaPeopleLoader& MP, PeopleLoader& P, int n) {

    const MetaPersonIDList& mids = MP.getIDs();
    const ProbabilityList&  prs  = MP.getPrs();
    std::discrete_distribution<int> idDistr(prs.begin(), prs.end());

    // Add n people to P
    for (int i = 1; i <= n; ++i) {
        Person p;
        p.id = i;
        p.mid = mids[idDistr(generator())];
        p.desc = MP[p.mid].desc;

        const ProbabilityList& tpsPrs = MP[p.mid].tpsPrs;
        std::discrete_distribution<int> tpDistr(tpsPrs.begin(), tpsPrs.end());
        p.tp = tpDistr(generator());

        P.add(p);
    }

}

#endif // ENTITY_GENERATOR_PERSONGENERATOR_HPP
