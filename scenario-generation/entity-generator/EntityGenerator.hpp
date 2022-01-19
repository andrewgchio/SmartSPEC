#ifndef ENTITY_GENERATOR_ENTITYGENERATOR_HPP
#define ENTITY_GENERATOR_ENTITYGENERATOR_HPP

#include <iostream>
#include <string>
#include <algorithm>

#include "../dataloader/EventsLoader.hpp" 
#include "../dataloader/PeopleLoader.hpp" 
#include "../dataloader/MetaEventsLoader.hpp" 
#include "../dataloader/MetaPeopleLoader.hpp" 

#include "../utils/Typedefs.hpp"

#include "EventGenerator.hpp"
#include "PersonGenerator.hpp"

class EntityGenerator {
public:
    
    static void generateEvents(
            MetaEventsLoader& ME, 
            EventsLoader& E, 
            int n,
            const std::string& mode,
            const Filename& fname);

    static void generatePeople(
            MetaPeopleLoader& MP, 
            PeopleLoader& P, 
            int n, 
            const std::string& mode, 
            const Filename& fname);

};

void EntityGenerator::generateEvents(
        MetaEventsLoader& ME,
        EventsLoader& E, 
        int n,
        const std::string& mode,
        const Filename& fname) {
    std::cout << "... Generating Events" << std::endl;

    switch(mode[0]) {
        case 'n' : case 'N': // none
            E = EventsLoader{fname};
            break;
        case 'a' : case 'A': // all
            EventsGenerator::generate(ME,E,n);
            break;
        case 'd' : case 'D': // diff
            E = EventsLoader(fname);
            EventsGenerator::generate(ME,E,std::max(0,n-E.size()));
            break;
        default:
            std::cerr << "events generation invalid mode" << std::endl;
            std::exit(1);
    }

    // Save new set of events into file
    E.dump(fname);
}

void EntityGenerator::generatePeople(
        MetaPeopleLoader& MP,
        PeopleLoader& P, 
        int n, 
        const std::string& mode,
        const Filename& fname) {
    std::cout << "... Generating People" << std::endl;

    switch(mode[0]) {
        case 'n' : case 'N': // none
            P = PeopleLoader{fname};
            break;
        case 'a' : case 'A': // all
            PeopleGenerator::generate(MP,P,n);
            break;
        case 'd' : case 'D': // diff
            P = PeopleLoader(fname);
            PeopleGenerator::generate(MP,P,std::max(0,n-P.size()));
            break;
        default:
            std::cerr << "people generation invalid mode" << std::endl;
            std::exit(1);
    }

    // Save new set of people into file
    P.dump(fname);
}

#endif // ENTITY_GENERATOR_ENTITYGENERATOR_HPP
