#ifndef DATALOADER_DATALOADER_HPP
#define DATALOADER_DATALOADER_HPP

#include <iostream>
#include <string>

#include "ConfigLoader.hpp"
#include "SpacesLoader.hpp"
#include "EventsLoader.hpp"
#include "PeopleLoader.hpp"
#include "SensorsLoader.hpp"

#include "MetaEventsLoader.hpp"
#include "MetaPeopleLoader.hpp"
#include "MetaTrajectoriesLoader.hpp"

#include "../utils/Typedefs.hpp"
#include "../utils/DateUtils.hpp"

class DataLoader {
public:

    explicit DataLoader(const Filename& fname);

    void loadEvents();
    void loadPeople();

    // Query functions
    TimePeriod query(const Person& p, const date::sys_days& d);
    TimePeriod query(const Person& p, const DateTime& dt);
    TimePeriod query(const Event& e, const date::sys_days& d);
    TimePeriod query(const Event& e, const DateTime& dt);

    friend std::ostream& operator<<(std::ostream& oss, const DataLoader& dm);

    ConfigLoader config;

    SensorsLoader S;
    PeopleLoader  P;
    EventsLoader  E;
    SpacesLoader  C;

    MetaEventsLoader ME;
    MetaPeopleLoader MP;
    MetaTrajectoriesLoader MT;

    Date start, end;

};

DataLoader::DataLoader(const Filename& fname)
    : config{fname}, 
      C{config("filepaths","spaces"), config("filepaths","spaces-cache","")},
      S{config("filepaths","sensors")},
      MP{config("filepaths","metapeople")},
      ME{config("filepaths","metaevents")},
      MT{config("filepaths","metatrajectories","none"), 
         config("filepaths","shortest-path-cache","none"),
         C},
      start{config("synthetic-data-generator","start")},
      end{config("synthetic-data-generator","end")}
{}

void DataLoader::loadEvents() 
{ E = EventsLoader{config("filepaths", "events")}; }

void DataLoader::loadPeople() 
{ P = PeopleLoader{config("filepaths", "people")}; }

TimePeriod DataLoader::query(const Person& p, const date::sys_days& d) 
{ return query(p,DateTime{d}); }

TimePeriod DataLoader::query(const Person& p, const DateTime& dt) 
{ return MP[p.mid].tps[p.tp].query(dt, false); }

TimePeriod DataLoader::query(const Event& e, const date::sys_days& d) 
{ return query(e,DateTime{d}); }

TimePeriod DataLoader::query(const Event& e, const DateTime& dt) 
{ return ME[e.mid].tps[e.tp].query(dt, true); }

std::ostream& operator<<(std::ostream& oss, const DataLoader& dl) {
    oss << dl.config
        << dl.S
        << dl.C
        << dl.P
        << dl.E
        << dl.MP
        << dl.ME
        << std::endl;
    return oss;
}

#endif // DATALOADER_DATALOADER_HPP
