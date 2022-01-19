#ifndef ENTITY_GENERATOR_EVENTSGENERATOR_HPP
#define ENTITY_GENERATOR_EVENTSGENERATOR_HPP

#include <iostream>
#include <string>
#include <random>
#include <algorithm>
#include <utility>

#include "../model/Event.hpp"
#include "../model/MetaEvent.hpp"

#include "../dataloader/EventsLoader.hpp"
#include "../dataloader/MetaEventsLoader.hpp"

#include "../utils/Typedefs.hpp"
#include "../utils/RandomGenerator.hpp"

namespace {

    const int MAX_LEISURE_EVENT_CAP = 99999;

} // end namespace

class EventsGenerator {
public:

    static void generate(MetaEventsLoader& ME, EventsLoader& E, int n);

private:

    static void addLeisureEvent(MetaEventsLoader& ME, EventsLoader& E);
    static CapRange sampleCapRangeDistr(CapRangeDistr& crd);
    static Event sampleMetaEvent(MetaEvent& me);

};

void EventsGenerator::generate(MetaEventsLoader& ME, EventsLoader& E, int n) {
    // Adds special events (out-of-simulation, leisure) if missing
    E.addOutEvent();
    E.addLeisureEvent();

    // Add at most one of each metaevent first until reaching min(n, ME.size())
    RandomSelector<MetaEventID> rs{ME.getIDs(), ME.getPrs()};
    for (MetaEventID mid : rs.selectWeightedN(std::min(n,ME.size()))) {
        Event e = sampleMetaEvent(ME[mid]);
        e.id = E.size();
        E.add(e);
    }

    // Add random additional events until reaching n
    const MetaEventIDList& mids = ME.getIDs();
    const ProbabilityList& prs  = ME.getPrs();
    std::discrete_distribution<MetaEventID> idDistr(prs.begin(), prs.end());

    for (int i = E.size()+1; i <= n+1; ++i) {
        MetaEventID mid = mids[idDistr(generator())];
        Event e = sampleMetaEvent(ME[mid]);
        e.id = E.size()+1;
        E.add(e);
    }

}

Event EventsGenerator::sampleMetaEvent(MetaEvent& me) {
    Event e;
    e.mid = me.id;
    e.desc = me.desc;

    const ProbabilityList& tpsPrs = me.tpsPrs;
    std::discrete_distribution<Index> tpDistr(tpsPrs.begin(), tpsPrs.end());
    e.tp = tpDistr(generator());

    e.spaces = me.selector.select();

    for (auto& caprange : me.cap)
        e.cap[caprange.first] = sampleCapRangeDistr(caprange.second);

    return e;
}

CapRange EventsGenerator::sampleCapRangeDistr(CapRangeDistr& crd) {
    MinCap lo = 1;
    MaxCap hi = 0;
    while (lo > hi) {
        lo = crd.first.sample();
        hi = crd.second.sample();
    }
    return CapRange(lo,hi);
}

#endif // ENTITY_GENERATOR_EVENTSGENERATOR_HPP
