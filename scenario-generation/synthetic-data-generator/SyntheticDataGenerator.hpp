#ifndef SYNTHETIC_DATA_GENERATOR_SYNTHETICDATAGENERATOR_HPP
#define SYNTHETIC_DATA_GENERATOR_SYNTHETICDATAGENERATOR_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <utility> 
#include <algorithm>

#include <boost/iostreams/tee.hpp>
#include <boost/iostreams/stream.hpp>

#include "../include/date/date.h"

#include "../utils/Typedefs.hpp"
#include "../utils/DateUtils.hpp"
#include "../utils/IOUtils.hpp"
#include "../utils/NormalDistributions.hpp"
#include "../utils/RandomGenerator.hpp"
#include "../utils/EventLogistics.hpp"
#include "../dataloader/DataLoader.hpp"

namespace {

    const date::days day1{1};
    const double PAST_PR = 0.8;

    static NormalTime leisureTime{"00:10:00","00:01:00"};


};

class SyntheticDataGenerator {
public: 
    
    explicit SyntheticDataGenerator(const DataLoader& dl);
    ~SyntheticDataGenerator();

    void generateLogs();

    void arrive(Person& p, DateTime& cdt);
    void leave(Person& p, DateTime& cdt);

    EventLogistics searchPrevEvents(Person& p, DateTime& currDT);
    EventLogistics searchNewEvents(Person& p, DateTime& currDT);
    void attendEvent(Person& p, const EventLogistics& el, DateTime& currDT);
    EventLogistics produceLogistics(Event& e, Person& p, DateTime& currDT);

    EventLogistics selectEvent(
        std::vector<EventLogistics>& possible,
        Person& p,
        DateTime& currDT);

    void move(
            Person& p, 
            Event& e, 
            const Trajectory& traj, 
            DateTime& currDT);

    void record(
            Person& p, 
            Event& e,
            Space& c, 
            const DateTime& sdt,
            const DateTime& edt);

private:

    DataLoader dl;
    std::ofstream out, log;

    // Typedefs for log/cout tee
    typedef boost::iostreams::tee_device<std::ostream,std::ofstream> TeeDevice;
    typedef boost::iostreams::stream<TeeDevice> TeeStream;

    TeeDevice teedev;
    TeeStream coutlog;

};

SyntheticDataGenerator::SyntheticDataGenerator(const DataLoader& dl)
    : dl{dl}, 
      out{dl.config("filepaths","output")+"data.csv"},
      log{dl.config("filepaths","output")+"data_log.txt"},
      teedev{std::cout, log},
      coutlog{teedev}
{
    coutlog << "Starting to generate synthetic data" << std::endl << std::endl;
    out << "PersonID,EventID,SpaceID,StartDateTime,EndDateTime" << std::endl;
}

SyntheticDataGenerator::~SyntheticDataGenerator() {
    out.flush();
    log.flush();
    coutlog.flush();

    out.close();
    log.close();
    coutlog.close();
}

void SyntheticDataGenerator::generateLogs() {
    for (date::sys_days d{dl.start}; d <= dl.end; d += day1) {
        coutlog << "=======================" << std::endl;
        coutlog << "Starting day " << d << std::endl;
        coutlog << "=======================" << std::endl;

        RandomSelector<PersonID> pids{dl.P.getIDs()};
        for (PersonID pid : pids.selectRandomN(dl.P.size())) {
            Person& p = dl.P[pid];
            TimePeriod active = dl.query(p, d);
            if (active) { // Person attends today
                coutlog << "Person " << pid << ": " << active << std::endl;

                DateTime currDT{active.start()};
                arrive(p,currDT);
                while (currDT <= active.end()) {
                    EventLogistics el = searchPrevEvents(p, currDT);
                    if (!el) // No previous event was found
                        el = searchNewEvents(p, currDT);
                    attendEvent(p, el, currDT);
                }
                leave(p,currDT);
            } 
        }

        coutlog << "=======================" << std::endl;
        coutlog << "Finished day " << d << std::endl;
        coutlog << "=======================" << std::endl << std::endl;
    }
}

void SyntheticDataGenerator::arrive(Person& p, DateTime& currDT) {
    record(p, dl.E.getOutEvent(), dl.C[p.currSpace], 
            currDT.firstTime(), currDT);
}

void SyntheticDataGenerator::leave(Person& p, DateTime& currDT) {
    move(p, dl.E.getOutEvent(), 
            dl.MT.getPath(p.currSpace, dl.C.getOutsideSpaceID()), 
            currDT);
    record(p, dl.E.getOutEvent(), dl.C.getOutsideSpace(), 
            currDT, currDT.lastTime());
}

EventLogistics SyntheticDataGenerator::searchPrevEvents(
        Person& p, 
        DateTime& currDT) {
    if (getRand() < PAST_PR) // attend previous events with PAST_PR probability
        return EventLogistics{}; // Go to searchNewEvents

    std::vector<EventLogistics> possible;
    for (EventLogistics el : p.attended) { 
        TimePeriod tp = dl.query(dl.E[el.eid], currDT);
        if (tp) {
            coutlog << "    considering past event " << el << std::endl;
            el.traj = dl.MT.getPath(p.currSpace, el.sid);
            el.tp = tp; 
            possible.push_back(el);
        }
    }

    if (!possible.empty())
        return selectEvent(possible, p, currDT);

    return EventLogistics{}; // Go to searchNewEvents
}

EventLogistics SyntheticDataGenerator::searchNewEvents(
        Person& p, 
        DateTime& currDT) {
    std::vector<EventLogistics> possible;
    for (Event& e : dl.E) {
        EventLogistics el = produceLogistics(e, p, currDT);
        if (el) {
            coutlog << "    consider new event " << el << std::endl;
            possible.push_back(el);
        }
    }

    if (!possible.empty())
        return selectEvent(possible, p, currDT);

    EventLogistics leisure;
    leisure.eid  = dl.E.getLeisureEventID();
    leisure.sid  = dl.C.getOutsideSpaceID();
    leisure.traj = dl.MT.getPath(p.currSpace, leisure.sid);
    leisure.tp   = TimePeriod{currDT, DateTime{currDT+leisureTime.sample()}};
    return leisure;
}

EventLogistics SyntheticDataGenerator::produceLogistics(
        Event& e, 
        Person& p, 
        DateTime& currDT) {
    EventLogistics el;
    el.eid = e.id;

    // Check event capacity; the leisure event is always attendable
    if (e.cap.find(-1) != e.cap.end() || // leisure event
        e.canAttend(p.mid)) {            // can metaperson attend?

        // Check event space and trajectory to space
        std::vector<Trajectory> tl;
        for (SpaceID c : e.spaces) {
            const Trajectory& t = dl.MT.getPath(p.currSpace, c);
            DateTime expArrival{currDT + t.totalTime()};
            if (dl.C[c].cap == -1 || 
                dl.C[c].getOccupancy(expArrival)+1 < dl.C[c].cap)
                tl.push_back(t);
        }

        if (tl.empty())
            return EventLogistics{};

        el.traj = RandomSelector<Trajectory>{tl}.selectRef();

        el.sid = el.traj.traj.empty() ? p.currSpace : el.traj.traj.back();

        // Choose event attendance time
        el.tp = dl.query(e, currDT);
        if (!el.tp)
            return EventLogistics{};
    }

    return el;
}

EventLogistics SyntheticDataGenerator::selectEvent(
        std::vector<EventLogistics>& possible,
        Person& p,
        DateTime& currDT) {
    MetaEventIDList ids(possible.size());
    ProbabilityList prs(possible.size());
    for (int i = 0; i < possible.size(); ++i) {
        ids[i] = dl.E[possible[i].eid].mid;
        prs[i] = dl.MP[p.mid].aff[ids[i]];
    }

    MetaEventID mid = RandomSelector<MetaEventID>{ids, prs}.select();
    possible.erase(
            std::remove_if(possible.begin(), possible.end(), 
                [&](EventLogistics& x) { return dl.E[x.eid].mid != mid; }),
            possible.end());

    // Select event of type mid using another mechanism?
    return RandomSelector<EventLogistics>{possible}.select();
}

void SyntheticDataGenerator::attendEvent(
        Person& p, 
        const EventLogistics& el, 
        DateTime& currDT) {
    coutlog << "Person " << p.id << ": " << el << std::endl;
    if (el.eid != dl.E.getLeisureEventID() && el.eid != dl.E.getOutEventID()) {
        p.attended.insert(el);
        dl.E[el.eid].enrollMetaPerson(p.mid);
    }
    move(p, dl.E[el.eid], el.traj, currDT);
    record(p,dl.E[el.eid], dl.C[el.sid], currDT, el.tp.end());
    currDT = el.tp.end();
}

void SyntheticDataGenerator::move(
        Person& p, 
        Event& e, 
        const Trajectory& traj, 
        DateTime& currDT) {
    if (traj.size() == 1) // already at target space
        return;

    // Start at i=1     : ignore start space
    // End   at i=size-1: ignore end space
    for (int i = 0; i < traj.traj.size()-1; ++i) {
        // DateTime end{currDT+traj.delta[i]};
        DateTime end = dl.C[p.currSpace].getNextOpenTime(
                                DateTime{currDT+traj.delta[i]});

        record(p, e, dl.C[traj.traj[i]], currDT, end);
        currDT = end;
    }
}

void SyntheticDataGenerator::record(
        Person& p, 
        Event& e,
        Space& c,
        const DateTime& sdt,
        const DateTime& edt) {
    p.currSpace = c.id;
    c.insertOccupancy(sdt,edt); 
    out << p.id << "," 
        << e.id << "," 
        << c.id << ","
        << sdt << ","
        << edt << std::endl;
}

#endif // SYNTHETIC_DATA_GENERATOR_SYNTHETICDATAGENERATOR_HPP
