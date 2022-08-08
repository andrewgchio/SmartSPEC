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

    // Probability that a previous event will be attended
    const double PAST_PR = 0.8;

    // Constant for 1 day
    const date::days day1{1};

    // Time spent in leisure
    static NormalTime leisureTime{"00:10:00","00:01:00"};

} // end namespace

class SyntheticDataGenerator {
public: 
    
    // Constructor / Destructor
    explicit SyntheticDataGenerator(const DataLoader& dl);
    ~SyntheticDataGenerator();

    // The main method to generate synthetic data logs
    void generateLogs();

private:

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

// The main method to generate synthetic logs
void SyntheticDataGenerator::generateLogs() {
    // Loop over each day of the simulation
    for (date::sys_days d{dl.start}; d <= dl.end; d += day1) {
        coutlog << "=======================" << std::endl;
        coutlog << "Starting day " << d << std::endl;
        coutlog << "=======================" << std::endl;

        // Iterate through all people in random order
        RandomSelector<PersonID> pids{dl.P.getIDs()};
        for (PersonID pid : pids.selectRandomN(dl.P.size())) {

            // Reference to simulated person
            Person& p = dl.P[pid];

            // Determine whether person will be simulated
            TimePeriod active = dl.query(p, d);
            if (active) { // Person attends today
                coutlog << "Person " << pid << ": " << active << std::endl;

                // Initialize currDT to track the person's day
                DateTime currDT{active.start()};

                // Bookkeeping for when person arrives
                arrive(p,currDT);

                // Continue iterating the time until the end of the day
                while (currDT <= active.end()) {

                    // Look for a previous (periodic) event to attend, or 
                    // select a new event to attend
                    EventLogistics el = searchPrevEvents(p, currDT);
                    if (!el) // No previous event was found
                        el = searchNewEvents(p, currDT);

                    // Attend the event
                    attendEvent(p, el, currDT);
                }

                // Bookkeeping for when person leaves
                leave(p,currDT);
            } 
        }

        coutlog << "=======================" << std::endl;
        coutlog << "Finished day " << d << std::endl;
        coutlog << "=======================" << std::endl << std::endl;
    }
}

// Bookkeeping for when person arrives. Record that the person spends the time
// first 00:00 to when they enter the simulated space as "outside".
void SyntheticDataGenerator::arrive(Person& p, DateTime& currDT) {
    record(p, dl.E.getOutEvent(), dl.C[p.getCurrentSpace()], 
            currDT.firstTime(), currDT);
}

// Bookkeeping for when person leaves. Move the person outside and record that
// they stay there until the end of the day.
void SyntheticDataGenerator::leave(Person& p, DateTime& currDT) {
    move(p, dl.E.getOutEvent(), 
            dl.MT.getPath(p.getCurrentSpace(), dl.C.getOutsideSpaceID()), 
            currDT);
    record(p, dl.E.getOutEvent(), dl.C.getOutsideSpace(), 
            currDT, currDT.lastTime());
}

// Look for a previous (periodic) event to attend
EventLogistics SyntheticDataGenerator::searchPrevEvents(
        Person& p, 
        DateTime& currDT) {
    // Previous events will be attended with PAST_PR probability 
    if (getRand() < PAST_PR) 
        return EventLogistics{}; // search for new event

    // Collect a list of previous events. An attendable previous event is
    // determined by whether querying the time profile of the event.
    std::vector<EventLogistics> possible;
    for (EventLogistics el : p.getAttendedEvents()) { 
        TimePeriod tp = dl.query(dl.E[el.eid], currDT);
        if (tp) {
            coutlog << "    considering past event " << el << std::endl;
            el.traj = dl.MT.getPath(p.getCurrentSpace(), el.sid);
            el.tp = tp; 
            possible.push_back(el);
        }
    }

    // If there is at least one possible previous event, then attend it
    if (!possible.empty())
        return selectEvent(possible, p, currDT);

    return EventLogistics{}; // Go to searchNewEvents
}

// Look for a new event to attend
EventLogistics SyntheticDataGenerator::searchNewEvents(
        Person& p, 
        DateTime& currDT) {
    // Collect a list of events that p can attend. An attendable event will be
    // indicated with its associated event logistics.
    std::vector<EventLogistics> possible;
    for (Event& e : dl.E) {
        EventLogistics el = produceLogistics(e, p, currDT);
        if (el) {
            coutlog << "    consider new event " << el << std::endl;
            possible.push_back(el);
        }
    }

    // If there is at least one possible new event, then attend it.
    if (!possible.empty())
        return selectEvent(possible, p, currDT);

    // Otherwise, the leisure event will be chosen.
    EventLogistics leisure;
    leisure.eid  = dl.E.getLeisureEventID();
    leisure.meid = dl.ME.getLeisureMetaEventID();
    leisure.sid  = dl.C.getOutsideSpaceID();
    leisure.traj = dl.MT.getPath(p.getCurrentSpace(), leisure.sid);
    leisure.tp   = TimePeriod{currDT, DateTime{currDT+leisureTime.sample()}};
    return leisure;
}

// For the given event, produce logistics detailing how p can attend e
EventLogistics SyntheticDataGenerator::produceLogistics(
        Event& e, 
        Person& p, 
        DateTime& currDT) {

    // Initialize event logistics
    EventLogistics el;
    el.eid  = e.id;
    el.meid = e.mid;

    // Check event capacity; the leisure event is always attendable
    if (e.cap.find(-1) != e.cap.end() || // leisure event
        e.canAttend(p.mid)) {            // can metaperson attend?

        // Check event space's capacity and trajectory to space
        std::vector<Trajectory> tl;
        for (SpaceID c : e.spaces) {
            const Trajectory& t = dl.MT.getPath(p.getCurrentSpace(), c);
            DateTime expArrival{currDT + t.totalTime()};
            if (dl.C[c].cap == -1 || 
                dl.C[c].getOccupancy(expArrival)+1 < dl.C[c].cap)
                tl.push_back(t);
        }

        // Person cannot attend event: no valid trajectories / space capacity
        if (tl.empty())
            return EventLogistics{};

        // Otherwise, select a random trajectory to the event space
        el.traj = RandomSelector<Trajectory>{tl}.selectRef();

        // Set event space depending on where the person is
        el.sid = el.traj.empty() ? p.getCurrentSpace() : el.traj.dest();

        // Choose event attendance time
        el.tp = dl.query(e, currDT);

        // If a valid time profile does not exist for the person, then they 
        // cannot attend the event. 
        if (!el.tp)
            return EventLogistics{};

        // Check CP, CE, PE constraints
        if (!dl.CS.checkCPConstraints(el.sid, p.id, currDT) || 
            !dl.CS.checkCEConstraints(el.sid, e.id, currDT) ||
            !dl.CS.checkPEConstraints(p.id,   e.id, currDT)) {
            return EventLogistics{};
        }
    }

    return el;
}

// Select the event to attend from among the possible set of events to attend.
// The probabilities of attending events is taken from the corresponding 
// metaevents and normalized
EventLogistics SyntheticDataGenerator::selectEvent(
        std::vector<EventLogistics>& possible,
        Person& p,
        DateTime& currDT) {
    // Initialize a list of ids and probabilities corresponding to `possible`
    MetaEventIDList ids(possible.size());
    ProbabilityList prs(possible.size());
    for (int i = 0; i < possible.size(); ++i) {
        ids[i] = dl.E[possible[i].eid].mid;
        prs[i] = dl.MP[p.mid].aff[ids[i]];
    }

    // Select a random metaevent to attend (with weighted probability) and 
    // remove all other metaevent types from the possible list
    MetaEventID mid = RandomSelector<MetaEventID>{ids, prs}.select();
    possible.erase(
            std::remove_if(possible.begin(), possible.end(), 
                [&](EventLogistics& x) { return dl.E[x.eid].mid != mid; }),
            possible.end());

    // From among the remaining possible events of the same metaevent type, 
    // select one at random
    return RandomSelector<EventLogistics>{possible}.select();
}

// Person p attends the event provided by the event logistics
void SyntheticDataGenerator::attendEvent(
        Person& p, 
        const EventLogistics& el, 
        DateTime& currDT) {
    coutlog << "Person " << p.id << ": " << el << std::endl;
    if (el.eid != dl.E.getLeisureEventID() && // do not record leisure event
        el.eid != dl.E.getOutEventID()) {     // do not record out event
        p.addAttendedEvent(el);
        dl.E[el.eid].enrollMetaPerson(p.mid);
    }
    move(p, dl.E[el.eid], el.traj, currDT);
    record(p,dl.E[el.eid], dl.C[el.sid], currDT, el.tp.end());
    currDT = el.tp.end();
}

// Move the person to the event using the given trajectory
void SyntheticDataGenerator::move(
        Person& p, 
        Event& e, 
        const Trajectory& traj, 
        DateTime& currDT) {
    // If the person is already at the target location
    if (traj.size() == 1) 
        return;

    // Move the person space by space, following the trajectory
    // Start at i=1     : ignore start space
    // End   at i=size-1: ignore end space
    for (int i = 0; i < traj.size()-1; ++i) {
        DateTime exp{currDT+traj.delta[i]};
        DateTime end = dl.C[p.getCurrentSpace()].getNextOpenTime(exp);
        record(p, e, dl.C[traj.traj[i]], currDT, end);
        currDT = end;
    }
}

// Record that person p was attending event e in space c between datetimes 
// (sdt, edt)
void SyntheticDataGenerator::record(
        Person& p, 
        Event& e,
        Space& c,
        const DateTime& sdt,
        const DateTime& edt) {
    p.setCurrentSpace(c.id);
    c.insertOccupancy(sdt,edt); 
    out << p.id << "," 
        << e.id << "," 
        << c.id << ","
        << sdt << ","
        << edt << std::endl;
}

#endif // SYNTHETIC_DATA_GENERATOR_SYNTHETICDATAGENERATOR_HPP
