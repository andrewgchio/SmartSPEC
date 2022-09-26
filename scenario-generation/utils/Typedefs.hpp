#ifndef DATALOADERS_TYPEDEFS_HPP
#define DATALOADERS_TYPEDEFS_HPP

#include <string>
#include <vector>
#include <map>
#include <set>
#include <utility>

#include <boost/iostreams/tee.hpp>
#include <boost/iostreams/stream.hpp>

#include "NormalDistributions.hpp"

// Config
using Section      = std::string;
using Option       = std::string;
using Value        = std::string;
using SectionEntry = std::pair<Option, Value>;
using SectionMap   = std::map<Option, Value>;
using ConfigEntry  = std::pair<Section, SectionMap>;
using ConfigMap    = std::map<Section, SectionMap>;

//General
using Filename        = std::string;
using Index           = int;
using Description     = std::string;
using Probability     = double; 
using ProbabilityList = std::vector<Probability>;
using Coordinates     = std::vector<double>;

// SmartSPEC Concepts
using PersonID     = int;
using EventID      = int;
using SpaceID      = int;
using SensorID     = int;
using MetaPersonID = int;
using MetaEventID  = int;
using MetaSensorID = int;

using PersonIDIndexMap     = std::map<PersonID, Index>;
using EventIDIndexMap      = std::map<EventID, Index>;
using SpaceIDIndexMap      = std::map<SpaceID, Index>;
using SensorIDIndexMap     = std::map<SensorID, Index>;
using MetaPersonIDIndexMap = std::map<MetaPersonID, Index>;
using MetaEventIDIndexMap  = std::map<MetaEventID, Index>;
using MetaSensorIDIndexMap = std::map<MetaSensorID, Index>;

using CPKey   = std::pair<SpaceID, PersonID>;
using CMPKey  = std::pair<SpaceID, MetaPersonID>;
using CEKey   = std::pair<SpaceID, EventID>;
using CMEKey  = std::pair<SpaceID, MetaEventID>;
using PEKey   = std::pair<PersonID, EventID>;
using PMEKey  = std::pair<PersonID, MetaEventID>;
using MPEKey  = std::pair<MetaPersonID, EventID>;
using MPMEKey = std::pair<MetaPersonID, MetaEventID>;

using PersonIDList     = std::vector<PersonID>;
using EventIDList      = std::vector<EventID>;
using SpaceIDList      = std::vector<SpaceID>;
using SensorIDList     = std::vector<SensorID>;
using MetaEventIDList  = std::vector<MetaEventID>;
using MetaPersonIDList = std::vector<MetaPersonID>;
using MetaSensorIDList = std::vector<MetaSensorID>;
 
using Coverage              = SpaceIDList; 
using MinCap                = int;
using MaxCap                = int;
using MinCapDistr           = Normal<MinCap>;
using MaxCapDistr           = Normal<MaxCap>;
using CapRange              = std::pair<MinCap, MaxCap>;
using PersonCapRange        = std::map<MetaPersonID, CapRange>; 
using EventAffinity         = std::map<MetaEventID, Probability>;
using MetaEventRequirements = std::map<MetaEventID, CapRange>;
using CapRangeDistr         = std::pair<MinCapDistr, MaxCapDistr>;
using PersonCapRangeDistr   = std::map<PersonID, CapRangeDistr>;

// SpacesGraph
using SrcDest         = std::pair<SpaceID, SpaceID>;
using SpaceIDSet      = std::set<SpaceID>;
using SrcDestIndexMap = std::map<SrcDest, Index>;

// Sensors Observations
using PersonIDSet         = std::set<PersonID>;
using SensorIDSet         = std::set<SensorID>;
using EventIDSet          = std::set<EventID>;
using AttendanceEntry     = std::tuple<PersonID, EventID, DateTime, DateTime>;
using AttendanceEntryList = std::vector<AttendanceEntry>;
using TrajectoryEntry     = std::tuple<SpaceID, DateTime, DateTime>;
using TrajectoryList      = std::vector<TrajectoryEntry>;

// For outputting to both std::cout and a log file
using TeeDevice = boost::iostreams::tee_device<std::ostream, std::ofstream>;
using TeeStream = boost::iostreams::stream<TeeDevice>;

#endif // DATALOADERS_TYPEDEFS_HPP
