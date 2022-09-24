#ifndef DATALOADER_DATALOADER_HPP
#define DATALOADER_DATALOADER_HPP

#include <iostream>
#include <string>

#include "ConfigLoader.hpp"
#include "SpacesLoader.hpp"
#include "EventsLoader.hpp"
#include "PeopleLoader.hpp"
#include "SensorsLoader.hpp"
#include "ConstraintsLoader.hpp"

#include "MetaEventsLoader.hpp"
#include "MetaPeopleLoader.hpp"
#include "MetaSensorsLoader.hpp"
#include "MetaTrajectoriesLoader.hpp"

#include "../utils/Typedefs.hpp"
#include "../utils/DateUtils.hpp"

class DataLoader {
public:

    // Constructor
    explicit DataLoader(const Filename& fname);

    // From entity generation
    void loadEvents();
    void loadPeople();

    // Queries for time periods
    TimePeriod query(const Person& p, const date::sys_days& d);
    TimePeriod query(const Person& p, const DateTime& dt);
    TimePeriod query(const Event& e, const date::sys_days& d);
    TimePeriod query(const Event& e, const DateTime& dt);

    // I/O
    friend std::ostream& operator<<(std::ostream& oss, const DataLoader& dm);

    // Data loaders
    ConfigLoader config;

    SensorsLoader S;
    PeopleLoader  P;
    EventsLoader  E;
    SpacesLoader  C;

    ConstraintsLoader CS;

    MetaEventsLoader ME;
    MetaPeopleLoader MP;
    MetaSensorsLoader MS;
    MetaTrajectoriesLoader MT;

    Date start, end;

};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Construtor

// For the given config file, initialize all other data loaders except for 
// events and people (use loadEvents(), loadPeople() instead)
DataLoader::DataLoader(const Filename& fname)
    : config{fname}, 
      C{config("filepaths","spaces"), config("filepaths","spaces-cache","")},
      S{config("filepaths","sensors")},
      CS{config("filepaths","constraints", "none")},
      MP{config("filepaths","metapeople")},
      ME{config("filepaths","metaevents")},
      MS{config("filepaths","metasensors")},
      MT{config("filepaths","metatrajectories","none"), 
         config("filepaths","path-cache","none"),
         C},
      start{config("synthetic-data-generator","start")},
      end{config("synthetic-data-generator","end")}
{
    CS.addSpaces(C);
    CS.addMetaEvents(ME);
    CS.addMetaPeople(MP);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Load data from entity generation

// Load events from the events file, after entity generation
void DataLoader::loadEvents() { 
    E = EventsLoader{config("filepaths", "events")}; 
    CS.addEvents(E);
}

// Load people from the events file, after entity generation
void DataLoader::loadPeople() { 
    P = PeopleLoader{config("filepaths", "people")}; 
    CS.addPeople(P);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Queries for time periods

// Return the time period that a person is in the simulated space on day d
TimePeriod DataLoader::query(const Person& p, const date::sys_days& d) 
{ return query(p,DateTime{d}); }

// Return the time period that a person is in the simulated space on datetime d
TimePeriod DataLoader::query(const Person& p, const DateTime& dt) 
{ return MP[p.mid].tps[p.tp].query(dt, false); }

// Return the time period that an event occurs 
TimePeriod DataLoader::query(const Event& e, const date::sys_days& d) 
{ return query(e,DateTime{d}); }

// Return the time period that an event occurs 
TimePeriod DataLoader::query(const Event& e, const DateTime& dt) 
{ return ME[e.mid].tps[e.tp].query(dt, true); }

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// I/O

// Print each of the contents of the dataloaders
std::ostream& operator<<(std::ostream& oss, const DataLoader& dl) {
    oss << dl.config
        << dl.S
        << dl.C
        << dl.P
        << dl.E
        << dl.CS
        << dl.MS
        << dl.MP
        << dl.ME
        << std::endl;
    return oss;
}

#endif // DATALOADER_DATALOADER_HPP
