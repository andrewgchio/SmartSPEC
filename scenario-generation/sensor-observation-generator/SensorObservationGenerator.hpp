#ifndef SENSOR_OBSERVATION_GENERATOR_SENSOROBSERVATIONGENERATOR_HPP
#define SENSOR_OBSERVATION_GENERATOR_SENSOROBSERVATIONGENERATOR_HPP

#include <iostream>
#include <fstream>
#include <map>
#include <set>
#include <utility>

#include <boost/icl/interval_map.hpp>
#include <boost/iostreams/tee.hpp>
#include <boost/iostreams/stream.hpp>

#include "../utils/Typedefs.hpp"
#include "../utils/DateUtils.hpp"
#include "../utils/IOUtils.hpp"
#include "../utils/NormalDistributions.hpp"
#include "../utils/RandomGenerator.hpp"
#include "../dataloader/DataLoader.hpp"

namespace {

    //const date::days day1{1}; // already defined in Synthetic Data Gen
    const double OBS_PR = 0.8;

} // end namespace

class SensorObservationGenerator {
public:

    explicit SensorObservationGenerator(const DataLoader& dl);
    ~SensorObservationGenerator();

    void generateObservations();

    void initializeAttendance();

    void record(PersonID p, SensorID s, const DateTime& dt);

private:
    
    DataLoader dl;
    std::ofstream out, log;

    // Typedefs for log/cout tee 
    typedef boost::iostreams::tee_device<std::ostream,std::ofstream> TeeDevice;
    typedef boost::iostreams::stream<TeeDevice> TeeStream;

    TeeDevice teedev;
    TeeStream coutlog;

    std::map<SpaceID, boost::icl::interval_map<long, PersonIDSet>> attendance;

};

SensorObservationGenerator::SensorObservationGenerator(const DataLoader& dl) 
    : dl{dl},
      out{dl.config("filepaths","output")+"observations.csv"},
      log{dl.config("filepaths","output")+"observations_log.csv"},
      teedev{std::cout, log},
      coutlog{teedev}
{
    coutlog << "Starting to generate observation data" 
        << std::endl << std::endl;
    out << "PersonID,SpaceID,DateTime" << std::endl;

    initializeAttendance();
}

void SensorObservationGenerator::initializeAttendance() {
    std::ifstream file{dl.config("filepaths","output")+"data.csv"};
    std::string pid,cid,sdt,edt, extra;
    // Skip first line
    std::getline(file, extra);
    while (std::getline(file,pid,',')) {
        std::getline(file,extra,','); // eid
        std::getline(file,cid,',');
        std::getline(file,sdt,',');
        std::getline(file,edt);

        attendance[std::stoi(cid)].add(std::make_pair(
            boost::icl::interval<long>::right_open(
                DateTime(sdt).count(),
                DateTime(edt).count()
            ),
            std::set<PersonID>{std::stoi(pid)}
        ));
    }
}

SensorObservationGenerator::~SensorObservationGenerator() {
    out.flush();
    log.flush();
    coutlog.flush();

    out.close();
    log.close();
    coutlog.close();
}

void SensorObservationGenerator::generateObservations() {
    for (date::sys_days d{dl.start}; d <= dl.end; d += day1) {
        coutlog << "=======================" << std::endl;                      
        coutlog << "Starting day " << d << std::endl;                           
        coutlog << "=======================" << std::endl;   

        DateTime endOfDay = DateTime{d}.lastTime();
        for (const Sensor& s : dl.S) {
            NormalTime obsSep{Time{s.intr*60}, Time{s.intr/5*60}};
            for (DateTime curr{d}; curr <= endOfDay; curr += obsSep.sample()) {
                for (SpaceID cid : s.cov) {
                    const auto& cover = attendance[cid];
                    auto it = cover.find(curr.count());
                    if (it != cover.end()) {
                        for (PersonID pid : it->second) {
                            if (getRand() < OBS_PR) {
                                record(pid, s.id, curr);
                            }
                        }
                    }
                }
            }
        }
    }
}

void SensorObservationGenerator::record(
        PersonID pid,
        SensorID sid,
        const DateTime& dt) {
    coutlog << "Person " << pid 
            << " observed at sensor " << sid 
            << " at " << dt << std::endl;
    out << pid << ","
        << sid << ","
        << dt << std::endl;
}

#endif // SENSOR_OBSERVATION_GENERATOR_SENSOROBSERVATIONGENERATOR_HPP
